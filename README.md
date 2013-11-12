range-v3
========

Experimental range library for C++11. 

**Dislaimer:** This code is not fit for *any use whatsoever*. 

Open Design Questions:
----------------------

* Are the range adaptors better thought of as "views"? E.g. `my_range | range::filter_view(some_fun)`? Or
  will this get people worrying about lifetime issues needlessly?
* Talk to Arno and Fabio about [N3752](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3752.pdf)
