/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_ALGORITHM_HPP
#define RANGES_V3_ALGORITHM_HPP

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wshadow"
#pragma clang diagnostic ignored "-Wdocumentation"
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wpadded"
#pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#pragma clang diagnostic ignored "-Wundef"

#include <range/v3/detail/config.hpp>
RANGES_DISABLE_WARNINGS

#include <range/v3/algorithm/adjacent_find.hpp>
#include <range/v3/algorithm/all_of.hpp>
#include <range/v3/algorithm/any_of.hpp>
#include <range/v3/algorithm/binary_search.hpp>
#include <range/v3/algorithm/copy.hpp>
#include <range/v3/algorithm/copy_backward.hpp>
#include <range/v3/algorithm/copy_if.hpp>
#include <range/v3/algorithm/copy_n.hpp>
#include <range/v3/algorithm/count.hpp>
#include <range/v3/algorithm/count_if.hpp>
#include <range/v3/algorithm/equal.hpp>
#include <range/v3/algorithm/equal_range.hpp>
#include <range/v3/algorithm/fill.hpp>
#include <range/v3/algorithm/fill_n.hpp>
#include <range/v3/algorithm/find.hpp>
#include <range/v3/algorithm/find_end.hpp>
#include <range/v3/algorithm/find_first_of.hpp>
#include <range/v3/algorithm/find_if.hpp>
#include <range/v3/algorithm/find_if_not.hpp>
#include <range/v3/algorithm/for_each.hpp>
#include <range/v3/algorithm/generate.hpp>
#include <range/v3/algorithm/generate_n.hpp>
#include <range/v3/algorithm/heap_algorithm.hpp>
#include <range/v3/algorithm/inplace_merge.hpp>
#include <range/v3/algorithm/is_partitioned.hpp>
#include <range/v3/algorithm/is_sorted.hpp>
#include <range/v3/algorithm/is_sorted_until.hpp>
#include <range/v3/algorithm/lexicographical_compare.hpp>
#include <range/v3/algorithm/lower_bound.hpp>
#include <range/v3/algorithm/max.hpp>
#include <range/v3/algorithm/max_element.hpp>
#include <range/v3/algorithm/merge.hpp>
#include <range/v3/algorithm/min.hpp>
#include <range/v3/algorithm/min_element.hpp>
#include <range/v3/algorithm/minmax.hpp>
#include <range/v3/algorithm/minmax_element.hpp>
#include <range/v3/algorithm/mismatch.hpp>
#include <range/v3/algorithm/move.hpp>
#include <range/v3/algorithm/move_backward.hpp>
#include <range/v3/algorithm/none_of.hpp>
#include <range/v3/algorithm/nth_element.hpp>
#include <range/v3/algorithm/partial_sort.hpp>
#include <range/v3/algorithm/partial_sort_copy.hpp>
#include <range/v3/algorithm/partition.hpp>
#include <range/v3/algorithm/partition_copy.hpp>
#include <range/v3/algorithm/partition_point.hpp>
#include <range/v3/algorithm/permutation.hpp>
#include <range/v3/algorithm/remove.hpp>
#include <range/v3/algorithm/remove_copy.hpp>
#include <range/v3/algorithm/remove_copy_if.hpp>
#include <range/v3/algorithm/remove_if.hpp>
#include <range/v3/algorithm/replace.hpp>
#include <range/v3/algorithm/replace_copy.hpp>
#include <range/v3/algorithm/replace_copy_if.hpp>
#include <range/v3/algorithm/replace_if.hpp>
#include <range/v3/algorithm/reverse.hpp>
#include <range/v3/algorithm/reverse_copy.hpp>
#include <range/v3/algorithm/rotate.hpp>
#include <range/v3/algorithm/rotate_copy.hpp>
#include <range/v3/algorithm/sample.hpp>
#include <range/v3/algorithm/search.hpp>
#include <range/v3/algorithm/search_n.hpp>
#include <range/v3/algorithm/set_algorithm.hpp>
#include <range/v3/algorithm/shuffle.hpp>
#include <range/v3/algorithm/sort.hpp>
#include <range/v3/algorithm/stable_partition.hpp>
#include <range/v3/algorithm/stable_sort.hpp>
#include <range/v3/algorithm/swap_ranges.hpp>
#include <range/v3/algorithm/transform.hpp>
#include <range/v3/algorithm/unique.hpp>
#include <range/v3/algorithm/unique_copy.hpp>
#include <range/v3/algorithm/upper_bound.hpp>

// BUGBUG
#include <range/v3/algorithm/aux_/equal_range_n.hpp>
#include <range/v3/algorithm/aux_/lower_bound_n.hpp>
#include <range/v3/algorithm/aux_/merge_n.hpp>
#include <range/v3/algorithm/aux_/merge_n_with_buffer.hpp>
#include <range/v3/algorithm/aux_/sort_n_with_buffer.hpp>
#include <range/v3/algorithm/aux_/upper_bound_n.hpp>

RANGES_RE_ENABLE_WARNINGS

#pragma clang diagnostic pop

#endif
