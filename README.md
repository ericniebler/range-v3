range-v3
========

Experimental range library for C++11. 

**Dislaimer:** This code is not fit for *any use whatsoever*. 

Open Design Questions:
----------------------

* Think about range assignment. Can you assign into a range slice, for instance?
* Think about infinite ranges.
* Think about range adaptors and facades.
* Some ranges have O(1) size even though they are not RandomAccess (e.g., `std::list` or the
  suggested `move_range_view`). How to expose and take advantage of that information?
* Think about a `debug_iterator` adaptor that remembers its range and catches bad comparisons
  and dereferences.
* Talk to Arno and Fabio about [N3752](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3752.pdf)
* "Algorithms vs adaptors" is "strict vs lazy execution". Does it make sense to provide both? With what syntax?
  - The eager "algorithm" version takes an output iterator. The lazy "adaptor" version doesn't, and returns a (lazy) range.
  - Observation: `copy_if` algorithm and `filter` adaptor are really the same.
  - Observation: `reverse_copy` algorithm and `reverse` adaptor are really the same. (Oops, there
    already is a `reverse` algorithm and it does an in-place reverse. Hrm. Maybe call it `inplace_reverse`?)
  - No, probably better to draw a distinction between INVOKE notation (e.g. `reverse(rng)`) and PIPE notation (e.g. `rng | reverse`), the former being eager when an operation supports it, and the latter being lazy. 
  - No, probably even better:
    * For all algorithms that take an output iterator, provide an overload that doesn't, but mutates the input sequence in-place. Return the new range, along with an iterator showing current output position. Lazy adaptors get their own names.
  - No, no, it's too confusing. Just keep the adaptors and the algorithms separate. Bummer.
