from conans import ConanFile, CMake, tools
import os

class ConanBuild(ConanFile):
    generators = "cmake", "virtualenv"
    requires = 'boost/1.69.0@conan/stable', 'hdf5/1.10.5@cd3/devel'


    def build(self):
      cmake = CMake(self)
      cmake.configure()
      cmake.build()
