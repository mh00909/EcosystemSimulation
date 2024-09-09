# CMake generated Testfile for 
# Source directory: /home/monika/cpp/ecosystem
# Build directory: /home/monika/cpp/ecosystem/cmake-build-debug
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[OrganismTest]=] "/home/monika/cpp/ecosystem/cmake-build-debug/ecosystem_tests")
set_tests_properties([=[OrganismTest]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/monika/cpp/ecosystem/CMakeLists.txt;75;add_test;/home/monika/cpp/ecosystem/CMakeLists.txt;0;")
subdirs("_deps/googletest-build")
