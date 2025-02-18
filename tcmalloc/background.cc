// Copyright 2019 The TCMalloc Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <errno.h>

#include "absl/base/internal/sysinfo.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"
#include "tcmalloc/cpu_cache.h"
#include "tcmalloc/internal/logging.h"
#include "tcmalloc/internal/percpu.h"
#include "tcmalloc/internal_malloc_extension.h"
#include "tcmalloc/malloc_extension.h"
#include "tcmalloc/parameters.h"
#include "tcmalloc/static_vars.h"

// Release memory to the system at a constant rate.
void MallocExtension_Internal_ProcessBackgroundActions() {
  tcmalloc::MallocExtension::MarkThreadIdle();

  absl::Time prev_time = absl::Now();
  constexpr absl::Duration kSleepTime = absl::Seconds(1);

  // Reclaim inactive per-cpu caches once per kCpuCacheReclaimPeriod.
  //
  // We use a longer 30 sec reclaim period to make sure that caches are indeed
  // idle. Reclaim drains entire cache, as opposed to cache shuffle for instance
  // that only shrinks a cache by a few objects at a time. So, we might have
  // larger performance degradation if we use a shorter reclaim interval and
  // drain caches that weren't supposed to.
  constexpr absl::Duration kCpuCacheReclaimPeriod = absl::Seconds(30);
  absl::Time last_reclaim = absl::Now();

  // Shuffle per-cpu caches once per kCpuCacheShufflePeriod secs.
  constexpr absl::Duration kCpuCacheShufflePeriod = absl::Seconds(5);
  absl::Time last_shuffle = absl::Now();

  while (true) {
    absl::Time now = absl::Now();
    const ssize_t bytes_to_release =
        static_cast<size_t>(tcmalloc::tcmalloc_internal::Parameters::
                                background_release_rate()) *
        absl::ToDoubleSeconds(now - prev_time);
    if (bytes_to_release > 0) {  // may be negative if time goes backwards
      tcmalloc::MallocExtension::ReleaseMemoryToSystem(bytes_to_release);
    }

    if (tcmalloc::MallocExtension::PerCpuCachesActive()) {
      // Accelerate fences as part of this operation by registering this thread
      // with rseq.  While this is not strictly required to succeed, we do not
      // expect an inconsistent state for rseq (some threads registered and some
      // threads unable to).
      CHECK_CONDITION(tcmalloc::tcmalloc_internal::subtle::percpu::IsFast());

      // Try to reclaim per-cpu caches once every kCpuCacheReclaimPeriod
      // when enabled.
      if (now - last_reclaim >= kCpuCacheReclaimPeriod) {
        tcmalloc::tcmalloc_internal::Static::cpu_cache().TryReclaimingCaches();
        last_reclaim = now;
      }

      const bool shuffle_per_cpu_caches =
          tcmalloc::tcmalloc_internal::Parameters::shuffle_per_cpu_caches();

      if (shuffle_per_cpu_caches) {
        if (now - last_shuffle >= kCpuCacheShufflePeriod) {
          tcmalloc::tcmalloc_internal::Static::cpu_cache().ShuffleCpuCaches();
          last_shuffle = now;
        }
      }
    }

    tcmalloc::tcmalloc_internal::Static().sharded_transfer_cache().Plunder();
    prev_time = now;
    absl::SleepFor(kSleepTime);
  }
}
