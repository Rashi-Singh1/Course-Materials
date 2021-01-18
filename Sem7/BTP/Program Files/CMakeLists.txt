cmake_minimum_required(VERSION 3.15)
project(Program_Files)

set(CMAKE_CXX_STANDARD 17)
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Set your project compile flags.
# E.g. if using the C++ header files
# you will need to enable C++11 support
# for your compiler.

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

#add_executable(Program_Files IR-FSMD2/include/parser.h  IR-FSMD2/include/FsmdaHeader.h IR-FSMD2/include/valPropHeader.h IR-FSMD2/src/fsmdaCode.c IR-FSMD2/src/fsmda.tab.c IR-FSMD2/include/fsmda.tab.h IR-FSMD2/src/normalization.c IR-FSMD2/src/lex.yy.c IR-FSMD2/src/lva.c IR-FSMD2/src/main.c)
#add_executable(Program_Files IR-FSMD/include/parser.h  IR-FSMD/include/FsmdaHeader.h IR-FSMD/include/valPropHeader.h IR-FSMD/src/fsmdaCode.c IR-FSMD/src/fsmda.tab.c IR-FSMD/include/fsmda.tab.h IR-FSMD/src/normalization.c IR-FSMD/src/lex.yy.c IR-FSMD/src/lva.c IR-FSMD/src/main3.c)
#add_executable(Program_Files IR-FSMD/include/parser.h  IR-FSMD/include/FsmdaHeader.h IR-FSMD/include/valPropHeader.h IR-FSMD/src/fsmdaCode.c IR-FSMD/src/fsmda.tab.c IR-FSMD/include/fsmda.tab.h IR-FSMD/src/normalization.c IR-FSMD/src/valPropCode.c IR-FSMD/src/lex.yy.c IR-FSMD/src/lva.c)
add_executable(Program_Files parser.cpp)
# Find the libraries that correspond to the LLVM components
# that we wish to use
llvm_map_components_to_libnames(llvm_libs support core irreader)

# Link against LLVM libraries
target_link_libraries(Program_Files ${llvm_libs})