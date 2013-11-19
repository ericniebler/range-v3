range-v3
========

Experimental range library for C++11. 

**Dislaimer:** This code is not fit for *any use whatsoever*. 

Open Design Questions:
----------------------

* Are the range adaptors better thought of as "views"? E.g. `my_range | range::filter_view(some_fun)`? Or
  will this get people worrying about lifetime issues needlessly?
* Talk to Arno and Fabio about [N3752](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3752.pdf)
* "Algorithms vs adaptors" is "strict vs lazy execution". Does it make sense to provide both? With what syntax?
  - The eager "algorithm" version takes an output iterator. The lazy "adaptor" version doesn't, and returns a (lazy) range.
  - Observation: `copy_if` algorithm and `filter` adaptor are really the same.
  - Observation: `reverse_copy` algorithm and `reverse` adaptor are really the same. (Oops, there
    already is a `reverse` algorithm and it does an in-place reverse. Hrm. Maybe call it `inplace_reverse`?)
* `rng | count(val)` vs `rng | count(_1, val)`. Be consistent about allowing both.
