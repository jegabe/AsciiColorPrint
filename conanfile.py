from conans import ConanFile, CMake, tools
import os

class AsciiColorPrint(ConanFile):
    name = "asciicp"
    version_major  = 1
    version_minor  = 0
    version_bugfix = 0
    version = str(version_major) + "." + str(version_minor) + "." + str(version_bugfix)
    url = "TODO"
    license = "MIT"
    description = "TODO"
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"
    exports_sources = "CMakeLists.txt", "include*", "src*", "tests*", "LICENSE.txt"
    requires = [
        "colmc/1.0.0@ganteje/release"
    ]
    no_copy_source=True

    def configure_cmake(self):
        cmake = CMake(self)
        cmake.configure()
        return cmake

    def build(self):
        cmake = self.configure_cmake()
        cmake.configure()
        cmake.build()
        
    def package(self):
        self.copy(self.name + ".exe", dst="bin", src="bin")
        self.copy(self.name, dst="bin", src="bin")
        self.copy("LICENSE.txt", dst="license", src="")

    def package_info(self):
        self.cpp_info.libs = [];
