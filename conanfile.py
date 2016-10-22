# Range v3 library
#
#  Copyright Luis Martinez de Bartolome Izquierdo 2016
#
#  Use, modification and distribution is subject to the
#  Boost Software License, Version 1.0. (See accompanying
#  file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt)
#
# Project home: https://github.com/ericniebler/range-v3
#

# This example shows how to define a range that is implemented
# in terms of itself. The example is generating the Fibonacci
# sequence using self-reference and zip_with.
#
# Note: don't use recursive_range_fn in performance sensitive
# code. Self-reference comes with indirection and dynamic
# allocation overhead.

from conans import ConanFile
import platform


class Rangev3Conan(ConanFile):
    name = "range-v3"
    version = "latest"
    license = "Boost Software License - Version 1.0 - August 17th, 2003"
    url = "https://github.com/ericniebler/range-v3"
    exports = "*"

    def package(self):
        self.copy("*.hpp", src="include", dst="include", keep_path=True)
