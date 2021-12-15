cmake_minimum_required(VERSION 3.5)
project(tcmalloc
    LANGUAGES CXX ASM
    VERSION 0.0.0
)

set(CMAKE_CXX_STANDARD 17)

#set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-std=c++17  ${CMAKE_CXX_FLAGS}")

#option(ABSL_PROPAGATE_CXX_STD ON )

## dependencies
add_subdirectory(  abseil-cpp )

## common parts of tcmalloc
add_library(tcmalloc_common STATIC
tcmalloc/arena.cc
#tcmalloc/arena_test.cc
tcmalloc/background.cc
#tcmalloc/central_freelist_benchmark.cc
tcmalloc/central_freelist.cc
#tcmalloc/central_freelist_test.cc
tcmalloc/common.cc
#tcmalloc/cpu_cache_activate_test.cc
tcmalloc/cpu_cache.cc
#tcmalloc/cpu_cache_test.cc
tcmalloc/experimental_pow2_below64_size_class.cc
tcmalloc/experimental_pow2_size_class.cc
tcmalloc/experiment.cc
#tcmalloc/experiment_config_test.cc
tcmalloc/experiment_fuzz.cc
#tcmalloc/guarded_page_allocator_benchmark.cc
tcmalloc/guarded_page_allocator.cc
#tcmalloc/guarded_page_allocator_test.cc
#tcmalloc/heap_profiling_test.cc
tcmalloc/huge_address_map.cc
#tcmalloc/huge_address_map_test.cc
tcmalloc/huge_allocator.cc
#tcmalloc/huge_allocator_test.cc
tcmalloc/huge_cache.cc
#tcmalloc/huge_cache_test.cc
tcmalloc/huge_page_aware_allocator.cc
#tcmalloc/huge_page_aware_allocator_test.cc
#tcmalloc/huge_page_filler_test.cc
#tcmalloc/huge_region_test.cc
tcmalloc/lifetime_based_allocator.cc
#tcmalloc/lifetime_based_allocator_test.cc
tcmalloc/malloc_extension.cc
tcmalloc/malloc_extension_fuzz.cc
#tcmalloc/malloc_extension_system_malloc_test.cc
#tcmalloc/malloc_extension_test.cc
#tcmalloc/mock_central_freelist.cc
#tcmalloc/mock_transfer_cache.cc
tcmalloc/new_extension.cc
#tcmalloc/new_extension_test.cc
tcmalloc/noruntime_size_classes.cc
tcmalloc/page_allocator.cc
tcmalloc/page_allocator_interface.cc
#tcmalloc/page_allocator_test.cc
tcmalloc/page_heap.cc
#tcmalloc/page_heap_test.cc
tcmalloc/pagemap.cc
#tcmalloc/pagemap_test.cc
tcmalloc/parameters.cc
tcmalloc/peak_heap_tracker.cc
#tcmalloc/profile_marshaler.cc
#tcmalloc/profile_marshaler_test.cc
#tcmalloc/profile_test.cc
#tcmalloc/realloc_test.cc
tcmalloc/runtime_size_classes.cc
tcmalloc/runtime_size_classes_fuzz.cc
#tcmalloc/runtime_size_classes_test.cc
tcmalloc/sampled_allocation.cc
#tcmalloc/sampled_allocation_recorder_test.cc
tcmalloc/sampler.cc
tcmalloc/size_classes.cc
#tcmalloc/size_classes_test.cc
#tcmalloc/size_classes_with_runtime_size_classes_test.cc
#tcmalloc/span_benchmark.cc
tcmalloc/span.cc
#tcmalloc/span_test.cc
tcmalloc/stack_trace_table.cc
#tcmalloc/stack_trace_table_test.cc
tcmalloc/static_vars.cc
tcmalloc/stats.cc
#tcmalloc/stats_test.cc
tcmalloc/system-alloc.cc
#tcmalloc/system-alloc_test.cc
#tcmalloc/tcmalloc.cc
#tcmalloc/tcmalloc_large_test.cc
tcmalloc/thread_cache.cc
#tcmalloc/thread_cache_test.cc
#tcmalloc/transfer_cache_benchmark.cc
tcmalloc/transfer_cache.cc
#tcmalloc/transfer_cache_fuzz.cc
#tcmalloc/transfer_cache_test.cc
tcmalloc/want_hpaa.cc
tcmalloc/want_hpaa_subrelease.cc
tcmalloc/want_no_hpaa.cc
tcmalloc/want_numa_aware.cc

tcmalloc/internal/cache_topology.cc
#tcmalloc/internal/cache_topology_test.cc
tcmalloc/internal/environment.cc
#tcmalloc/internal/environment_test.cc
#tcmalloc/internal/lifetime_predictions_test.cc
#tcmalloc/internal/lifetime_tracker_test.cc
#tcmalloc/internal/linked_list_benchmark.cc
#tcmalloc/internal/linked_list_test.cc
tcmalloc/internal/logging.cc
#tcmalloc/internal/logging_test.cc
#tcmalloc/internal/logging_test_helper.cc
tcmalloc/internal/memory_stats.cc
#tcmalloc/internal/memory_stats_test.cc
#tcmalloc/internal/mincore_benchmark.cc
tcmalloc/internal/mincore.cc
#tcmalloc/internal/mincore_test.cc
tcmalloc/internal/numa.cc
#tcmalloc/internal/numa_test.cc
tcmalloc/internal/percpu.cc
tcmalloc/internal/percpu_rseq_x86_64.S

tcmalloc/internal/percpu_rseq_unsupported.cc
#tcmalloc/internal/percpu_tcmalloc_test.cc
tcmalloc/internal/proc_maps.cc

#tcmalloc/internal/profile_builder.cc
#tcmalloc/internal/profile_builder_test.cc
#tcmalloc/internal/range_tracker_benchmark.cc
#tcmalloc/internal/range_tracker_test.cc
#tcmalloc/internal/timeseries_tracker_test.cc
tcmalloc/internal/util.cc

)

target_include_directories(tcmalloc_common PUBLIC . )
target_compile_features(tcmalloc_common PUBLIC cxx_std_11)
target_link_libraries(tcmalloc_common PUBLIC
    # These libraries are in the PUBLIC interface to allow other
    # applications/libs including `tcmalloc` to use all functions
    # defined in headers. Those libs are potentially part of
    # the interface and not just used internally.
    #
    # TODO: Check how much of `absl` is in the `tcmalloc` interface.

    absl::base
    absl::debugging
    absl::flags
)
add_library(tcmalloc::common ALIAS tcmalloc_common)

## overrides
# always
add_library(tcmalloc_always STATIC
    tcmalloc/tcmalloc.cc
)
target_include_directories(tcmalloc_always PUBLIC . )
target_compile_features(tcmalloc_always PUBLIC cxx_std_11)
target_link_libraries(tcmalloc_always PUBLIC
    # See above comment about PUBLIC link interface.
    tcmalloc_common
)
add_library(tcmalloc::always ALIAS tcmalloc_always)
