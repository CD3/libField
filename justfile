install-deps:
  conan install . -s build_type=Debug --build missing
  conan install . -s build_type=Release --build missing

configure: install-deps
  cmake --preset conan-default

build: configure
  cmake --build build --config Debug
  cmake --build build --config Release

test: build
  cd build && testing/Debug/libField_CatchTests
  cd build && testing/Release/libField_CatchTests

format:
  fd . src --type f --exec clang-format -i
