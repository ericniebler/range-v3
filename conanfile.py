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
