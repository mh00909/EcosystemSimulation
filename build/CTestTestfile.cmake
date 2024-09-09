# CMake generated Testfile for 
# Source directory: /home/monika/cpp/ecosystem
# Build directory: /home/monika/cpp/ecosystem/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[OrganismTest]=] "/home/monika/cpp/ecosystem/build/ecosystem_tests")
set_tests_properties([=[OrganismTest]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/monika/cpp/ecosystem/CMakeLists.txt;75;add_test;/home/monika/cpp/ecosystem/CMakeLists.txt;0;")
subdirs("_deps/googletest-build")
