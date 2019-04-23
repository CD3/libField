from conans import ConanFile, CMake, tools
import os

class libFieldConan(ConanFile):
    name = "libField"
    version = "master"
    license = "MIT"
    author = "CD Clark III clifton.clark@gmail.com"
    url = "https://github.com/CD3/conan-libField"
    homepage = "https://github.com/CD3/libField"
    description = "A C++ library for storing and working with field data."
    topics = ("C++", "Physics")

    generators = "cmake", "virtualenv"
    requires = 'boost/1.69.0@conan/stable','eigen/3.3.7@conan/stable'
    build_requires = 'cmake_installer/3.13.0@conan/stable'

    exports_sources = 'CMakeLists.txt','cmake/*','src/*', 'LICENSE.md', 'README.md'


    def build(self):
      tools.replace_in_file('CMakeLists.txt',
                            'project(libField)',
                            'project(libField)\nset(STANDALONE OFF)')

      cmake = CMake(self)
      cmake.configure()

      cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()
        
    def package_info(self):
        self.env_info.libField_DIR = os.path.join(self.package_folder, "cmake")

