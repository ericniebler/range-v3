add_executable(alg.adjacent_find adjacent_find.cpp)
target_link_libraries(alg.adjacent_find range-v3)
add_test(test.alg.adjacent_find, alg.adjacent_find)

add_executable(alg.all_of all_of.cpp)
target_link_libraries(alg.all_of range-v3)
add_test(test.alg.all_of, alg.all_of)

add_executable(alg.any_of any_of.cpp)
target_link_libraries(alg.any_of range-v3)
add_test(test.alg.any_of, alg.any_of)

add_executable(alg.none_of none_of.cpp)
target_link_libraries(alg.none_of range-v3)
add_test(test.alg.none_of, alg.none_of)

add_executable(alg.binary_search binary_search.cpp)
target_link_libraries(alg.binary_search range-v3)
add_test(test.alg.binary_search, alg.binary_search)

add_executable(alg.copy copy.cpp)
target_link_libraries(alg.copy range-v3)
add_test(test.alg.copy, alg.copy)

add_executable(alg.copy_backward copy_backward.cpp)
target_link_libraries(alg.copy_backward range-v3)
add_test(test.alg.copy_backward, alg.copy_backward)

add_executable(alg.count count.cpp)
target_link_libraries(alg.count range-v3)
add_test(test.alg.count, alg.count)

add_executable(alg.count_if count_if.cpp)
target_link_libraries(alg.count_if range-v3)
add_test(test.alg.count_if, alg.count_if)

add_executable(alg.equal equal.cpp)
target_link_libraries(alg.equal range-v3)
add_test(test.alg.equal, alg.equal)

add_executable(alg.equal_range equal_range.cpp)
target_link_libraries(alg.equal_range range-v3)
add_test(test.alg.equal_range, alg.equal_range)

add_executable(alg.fill fill.cpp)
target_link_libraries(alg.fill range-v3)
add_test(test.alg.fill, alg.fill)

add_executable(alg.find find.cpp)
target_link_libraries(alg.find range-v3)
add_test(test.alg.find, alg.find)

add_executable(alg.find_end find_end.cpp)
target_link_libraries(alg.find_end range-v3)
add_test(test.alg.find_end, alg.find_end)

add_executable(alg.find_if find_if.cpp)
target_link_libraries(alg.find_if range-v3)
add_test(test.alg.find_if, alg.find_if)

add_executable(alg.find_first_of find_first_of.cpp)
target_link_libraries(alg.find_first_of range-v3)
add_test(test.alg.find_first_of, alg.find_first_of)

add_executable(alg.for_each for_each.cpp)
target_link_libraries(alg.for_each range-v3)
add_test(test.alg.for_each, alg.for_each)

add_executable(alg.for_each_n for_each_n.cpp)
target_link_libraries(alg.for_each_n range-v3)
add_test(test.alg.for_each_n, alg.for_each_n)

add_executable(alg.generate generate.cpp)
target_link_libraries(alg.generate range-v3)
add_test(test.alg.generate, alg.generate)

add_executable(alg.generate_n generate_n.cpp)
target_link_libraries(alg.generate_n range-v3)
add_test(test.alg.generate_n, alg.generate_n)

add_executable(alg.includes includes.cpp)
target_link_libraries(alg.includes range-v3)
add_test(test.alg.includes, alg.includes)

add_executable(alg.inplace_merge inplace_merge.cpp)
target_link_libraries(alg.inplace_merge range-v3)
add_test(test.alg.inplace_merge, alg.inplace_merge)

add_executable(alg.is_heap1 is_heap1.cpp)
target_link_libraries(alg.is_heap1 range-v3)
add_test(test.alg.is_heap1, alg.is_heap1)

add_executable(alg.is_heap2 is_heap2.cpp)
target_link_libraries(alg.is_heap2 range-v3)
add_test(test.alg.is_heap2, alg.is_heap2)

add_executable(alg.is_heap3 is_heap3.cpp)
target_link_libraries(alg.is_heap3 range-v3)
add_test(test.alg.is_heap3, alg.is_heap3)

add_executable(alg.is_heap4 is_heap4.cpp)
target_link_libraries(alg.is_heap4 range-v3)
add_test(test.alg.is_heap4, alg.is_heap4)

add_executable(alg.is_heap_until1 is_heap_until1.cpp)
target_link_libraries(alg.is_heap_until1 range-v3)
add_test(test.alg.is_heap_until1, alg.is_heap_until1)

add_executable(alg.is_heap_until2 is_heap_until2.cpp)
target_link_libraries(alg.is_heap_until2 range-v3)
add_test(test.alg.is_heap_until2, alg.is_heap_until2)

add_executable(alg.is_heap_until3 is_heap_until3.cpp)
target_link_libraries(alg.is_heap_until3 range-v3)
add_test(test.alg.is_heap_until3, alg.is_heap_until3)

add_executable(alg.is_heap_until4 is_heap_until4.cpp)
target_link_libraries(alg.is_heap_until4 range-v3)
add_test(test.alg.is_heap_until4, alg.is_heap_until4)

add_executable(alg.is_partitioned is_partitioned.cpp)
target_link_libraries(alg.is_partitioned range-v3)
add_test(test.alg.is_partitioned, alg.is_partitioned)

add_executable(alg.is_permutation is_permutation.cpp)
target_link_libraries(alg.is_permutation range-v3)
add_test(test.alg.is_permutation, alg.is_permutation)

add_executable(alg.is_sorted_until is_sorted_until.cpp)
target_link_libraries(alg.is_sorted_until range-v3)
add_test(test.alg.is_sorted_until, alg.is_sorted_until)

add_executable(alg.is_sorted is_sorted.cpp)
target_link_libraries(alg.is_sorted range-v3)
add_test(test.alg.is_sorted, alg.is_sorted)

add_executable(alg.lexicographical_compare lexicographical_compare.cpp)
target_link_libraries(alg.lexicographical_compare range-v3)
add_test(test.alg.lexicographical_compare, alg.lexicographical_compare)

add_executable(alg.lower_bound lower_bound.cpp)
target_link_libraries(alg.lower_bound range-v3)
add_test(test.alg.lower_bound, alg.lower_bound)

add_executable(alg.make_heap make_heap.cpp)
target_link_libraries(alg.make_heap range-v3)
add_test(test.alg.make_heap, alg.make_heap)

add_executable(alg.max max.cpp)
target_link_libraries(alg.max range-v3)
add_test(test.alg.max, alg.max)

add_executable(alg.max_element max_element.cpp)
target_link_libraries(alg.max_element range-v3)
add_test(test.alg.max_element, alg.max_element)

add_executable(alg.merge merge.cpp)
target_link_libraries(alg.merge range-v3)
add_test(test.alg.merge, alg.merge)

add_executable(alg.min min.cpp)
target_link_libraries(alg.min range-v3)
add_test(test.alg.min, alg.min)

add_executable(alg.min_element min_element.cpp)
target_link_libraries(alg.min_element range-v3)
add_test(test.alg.min_element, alg.min_element)

add_executable(alg.minmax minmax.cpp)
target_link_libraries(alg.minmax range-v3)
add_test(test.alg.minmax, alg.minmax)

add_executable(alg.minmax_element minmax_element.cpp)
target_link_libraries(alg.minmax_element range-v3)
add_test(test.alg.minmax_element, alg.minmax_element)

add_executable(alg.mismatch mismatch.cpp)
target_link_libraries(alg.mismatch range-v3)
add_test(test.alg.mismatch, alg.mismatch)

add_executable(alg.move move.cpp)
target_link_libraries(alg.move range-v3)
add_test(test.alg.move alg.move)

add_executable(alg.move_backward move_backward.cpp)
target_link_libraries(alg.move_backward range-v3)
add_test(test.alg.move_backward alg.move_backward)

add_executable(alg.next_permutation next_permutation.cpp)
target_link_libraries(alg.next_permutation range-v3)
add_test(test.alg.next_permutation alg.next_permutation)

add_executable(alg.nth_element nth_element.cpp)
target_link_libraries(alg.nth_element range-v3)
add_test(test.alg.nth_element alg.nth_element)

add_executable(alg.partial_sort partial_sort.cpp)
target_link_libraries(alg.partial_sort range-v3)
add_test(test.alg.partial_sort, alg.partial_sort)

add_executable(alg.partial_sort_copy partial_sort_copy.cpp)
target_link_libraries(alg.partial_sort_copy range-v3)
add_test(test.alg.partial_sort_copy, alg.partial_sort_copy)

add_executable(alg.partition partition.cpp)
target_link_libraries(alg.partition range-v3)
add_test(test.alg.partition, alg.partition)

add_executable(alg.partition_copy partition_copy.cpp)
target_link_libraries(alg.partition_copy range-v3)
add_test(test.alg.partition_copy, alg.partition_copy)

add_executable(alg.partition_point partition_point.cpp)
target_link_libraries(alg.partition_point range-v3)
add_test(test.alg.partition_point, alg.partition_point)

add_executable(alg.pop_heap pop_heap.cpp)
target_link_libraries(alg.pop_heap range-v3)
add_test(test.alg.pop_heap alg.pop_heap)

add_executable(alg.prev_permutation prev_permutation.cpp)
target_link_libraries(alg.prev_permutation range-v3)
add_test(test.alg.prev_permutation alg.prev_permutation)

add_executable(alg.push_heap push_heap.cpp)
target_link_libraries(alg.push_heap range-v3)
add_test(test.alg.push_heap alg.push_heap)

add_executable(alg.remove remove.cpp)
target_link_libraries(alg.remove range-v3)
add_test(test.alg.remove, alg.remove)

add_executable(alg.remove_copy remove_copy.cpp)
target_link_libraries(alg.remove_copy range-v3)
add_test(test.alg.remove_copy, alg.remove_copy)

add_executable(alg.remove_copy_if remove_copy_if.cpp)
target_link_libraries(alg.remove_copy_if range-v3)
add_test(test.alg.remove_copy_if, alg.remove_copy_if)

add_executable(alg.remove_if remove_if.cpp)
target_link_libraries(alg.remove_if range-v3)
add_test(test.alg.remove_if, alg.remove_if)

add_executable(alg.replace replace.cpp)
target_link_libraries(alg.replace range-v3)
add_test(test.alg.replace, alg.replace)

add_executable(alg.replace_copy replace_copy.cpp)
target_link_libraries(alg.replace_copy range-v3)
add_test(test.alg.replace_copy, alg.replace_copy)

add_executable(alg.replace_copy_if replace_copy_if.cpp)
target_link_libraries(alg.replace_copy_if range-v3)
add_test(test.alg.replace_copy_if, alg.replace_copy_if)

add_executable(alg.replace_if replace_if.cpp)
target_link_libraries(alg.replace_if range-v3)
add_test(test.alg.replace_if, alg.replace_if)

add_executable(alg.reverse reverse.cpp)
target_link_libraries(alg.reverse range-v3)
add_test(test.alg.reverse, alg.reverse)

add_executable(alg.reverse_copy reverse_copy.cpp)
target_link_libraries(alg.reverse_copy range-v3)
add_test(test.alg.reverse_copy, alg.reverse_copy)

add_executable(alg.rotate rotate.cpp)
target_link_libraries(alg.rotate range-v3)
add_test(test.alg.rotate, alg.rotate)

add_executable(alg.rotate_copy rotate_copy.cpp)
target_link_libraries(alg.rotate_copy range-v3)
add_test(test.alg.rotate_copy, alg.rotate_copy)

add_executable(alg.sample sample.cpp)
target_link_libraries(alg.sample range-v3)
add_test(test.alg.sample, alg.sample)

add_executable(alg.search search.cpp)
target_link_libraries(alg.search range-v3)
add_test(test.alg.search, alg.search)

add_executable(alg.search_n search_n.cpp)
target_link_libraries(alg.search_n range-v3)
add_test(test.alg.search_n, alg.search_n)

add_executable(alg.set_difference1 set_difference1.cpp)
target_link_libraries(alg.set_difference1 range-v3)
add_test(test.alg.set_difference1, alg.set_difference1)

add_executable(alg.set_difference2 set_difference2.cpp)
target_link_libraries(alg.set_difference2 range-v3)
add_test(test.alg.set_difference2, alg.set_difference2)

add_executable(alg.set_difference3 set_difference3.cpp)
target_link_libraries(alg.set_difference3 range-v3)
add_test(test.alg.set_difference3, alg.set_difference3)

add_executable(alg.set_difference4 set_difference4.cpp)
target_link_libraries(alg.set_difference4 range-v3)
add_test(test.alg.set_difference4, alg.set_difference4)

add_executable(alg.set_difference5 set_difference5.cpp)
target_link_libraries(alg.set_difference5 range-v3)
add_test(test.alg.set_difference5, alg.set_difference5)

add_executable(alg.set_difference6 set_difference6.cpp)
target_link_libraries(alg.set_difference6 range-v3)
add_test(test.alg.set_difference6, alg.set_difference6)

add_executable(alg.set_intersection1 set_intersection1.cpp)
target_link_libraries(alg.set_intersection1 range-v3)
add_test(test.alg.set_intersection1, alg.set_intersection1)

add_executable(alg.set_intersection2 set_intersection2.cpp)
target_link_libraries(alg.set_intersection2 range-v3)
add_test(test.alg.set_intersection2, alg.set_intersection2)

add_executable(alg.set_intersection3 set_intersection3.cpp)
target_link_libraries(alg.set_intersection3 range-v3)
add_test(test.alg.set_intersection3, alg.set_intersection3)

add_executable(alg.set_intersection4 set_intersection4.cpp)
target_link_libraries(alg.set_intersection4 range-v3)
add_test(test.alg.set_intersection4, alg.set_intersection4)

add_executable(alg.set_intersection5 set_intersection5.cpp)
target_link_libraries(alg.set_intersection5 range-v3)
add_test(test.alg.set_intersection5, alg.set_intersection5)

add_executable(alg.set_intersection6 set_intersection6.cpp)
target_link_libraries(alg.set_intersection6 range-v3)
add_test(test.alg.set_intersection6, alg.set_intersection6)

add_executable(alg.set_symmetric_difference1 set_symmetric_difference1.cpp)
target_link_libraries(alg.set_symmetric_difference1 range-v3)
add_test(test.alg.set_symmetric_difference1, alg.set_symmetric_difference1)

add_executable(alg.set_symmetric_difference2 set_symmetric_difference2.cpp)
target_link_libraries(alg.set_symmetric_difference2 range-v3)
add_test(test.alg.set_symmetric_difference2, alg.set_symmetric_difference2)

add_executable(alg.set_symmetric_difference3 set_symmetric_difference3.cpp)
target_link_libraries(alg.set_symmetric_difference3 range-v3)
add_test(test.alg.set_symmetric_difference3, alg.set_symmetric_difference3)

add_executable(alg.set_symmetric_difference4 set_symmetric_difference4.cpp)
target_link_libraries(alg.set_symmetric_difference4 range-v3)
add_test(test.alg.set_symmetric_difference4, alg.set_symmetric_difference4)

add_executable(alg.set_symmetric_difference5 set_symmetric_difference5.cpp)
target_link_libraries(alg.set_symmetric_difference5 range-v3)
add_test(test.alg.set_symmetric_difference5, alg.set_symmetric_difference5)

add_executable(alg.set_symmetric_difference6 set_symmetric_difference6.cpp)
target_link_libraries(alg.set_symmetric_difference6 range-v3)
add_test(test.alg.set_symmetric_difference6, alg.set_symmetric_difference6)

add_executable(alg.set_union1 set_union1.cpp)
target_link_libraries(alg.set_union1 range-v3)
add_test(test.alg.set_union1, alg.set_union1)

add_executable(alg.set_union2 set_union2.cpp)
target_link_libraries(alg.set_union2 range-v3)
add_test(test.alg.set_union2, alg.set_union2)

add_executable(alg.set_union3 set_union3.cpp)
target_link_libraries(alg.set_union3 range-v3)
add_test(test.alg.set_union3, alg.set_union3)

add_executable(alg.set_union4 set_union4.cpp)
target_link_libraries(alg.set_union4 range-v3)
add_test(test.alg.set_union4, alg.set_union4)

add_executable(alg.set_union5 set_union5.cpp)
target_link_libraries(alg.set_union5 range-v3)
add_test(test.alg.set_union5, alg.set_union5)

add_executable(alg.set_union6 set_union6.cpp)
target_link_libraries(alg.set_union6 range-v3)
add_test(test.alg.set_union6, alg.set_union6)

add_executable(alg.shuffle shuffle.cpp)
target_link_libraries(alg.shuffle range-v3)
add_test(test.alg.shuffle, alg.shuffle)

add_executable(alg.sort sort.cpp)
target_link_libraries(alg.sort range-v3)
add_test(test.alg.sort, alg.sort)

add_executable(alg.sort_heap sort_heap.cpp)
target_link_libraries(alg.sort_heap range-v3)
add_test(test.alg.sort_heap, alg.sort_heap)

add_executable(alg.stable_partition stable_partition.cpp)
target_link_libraries(alg.stable_partition range-v3)
add_test(test.alg.stable_partition, alg.stable_partition)

add_executable(alg.stable_sort stable_sort.cpp)
target_link_libraries(alg.stable_sort range-v3)
add_test(test.alg.stable_sort, alg.stable_sort)

add_executable(alg.swap_ranges swap_ranges.cpp)
target_link_libraries(alg.swap_ranges range-v3)
add_test(test.alg.swap_ranges, alg.swap_ranges)

add_executable(alg.transform transform.cpp)
target_link_libraries(alg.transform range-v3)
add_test(test.alg.transform, alg.transform)

add_executable(alg.unique unique.cpp)
target_link_libraries(alg.unique range-v3)
add_test(test.alg.unique, alg.unique)

add_executable(alg.unique_copy unique_copy.cpp)
target_link_libraries(alg.unique_copy range-v3)
add_test(test.alg.unique_copy, alg.unique_copy)

add_executable(alg.upper_bound upper_bound.cpp)
target_link_libraries(alg.upper_bound range-v3)
add_test(test.alg.upper_bound, alg.upper_bound)

add_executable(alg.sort_n_with_buffer sort_n_with_buffer.cpp)
target_link_libraries(alg.sort_n_with_buffer range-v3)
add_test(test.alg.sort_n_with_buffer alg.sort_n_with_buffer)
