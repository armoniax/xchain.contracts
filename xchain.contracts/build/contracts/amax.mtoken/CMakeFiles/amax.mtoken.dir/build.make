# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.22.3/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.22.3/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/richardchen/work/amax/amax.xchain/xchain.contracts/contracts

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/richardchen/work/amax/amax.xchain/xchain.contracts/build/contracts

# Include any dependencies generated for this target.
include amax.mtoken/CMakeFiles/amax.mtoken.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include amax.mtoken/CMakeFiles/amax.mtoken.dir/compiler_depend.make

# Include the progress variables for this target.
include amax.mtoken/CMakeFiles/amax.mtoken.dir/progress.make

# Include the compile flags for this target's objects.
include amax.mtoken/CMakeFiles/amax.mtoken.dir/flags.make

amax.mtoken/CMakeFiles/amax.mtoken.dir/src/amax.mtoken.cpp.obj: amax.mtoken/CMakeFiles/amax.mtoken.dir/flags.make
amax.mtoken/CMakeFiles/amax.mtoken.dir/src/amax.mtoken.cpp.obj: /Users/richardchen/work/amax/amax.xchain/xchain.contracts/contracts/amax.mtoken/src/amax.mtoken.cpp
amax.mtoken/CMakeFiles/amax.mtoken.dir/src/amax.mtoken.cpp.obj: amax.mtoken/CMakeFiles/amax.mtoken.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/richardchen/work/amax/amax.xchain/xchain.contracts/build/contracts/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object amax.mtoken/CMakeFiles/amax.mtoken.dir/src/amax.mtoken.cpp.obj"
	cd /Users/richardchen/work/amax/amax.xchain/xchain.contracts/build/contracts/amax.mtoken && /usr/local/amax.cdt/bin/amax-cpp $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT amax.mtoken/CMakeFiles/amax.mtoken.dir/src/amax.mtoken.cpp.obj -MF CMakeFiles/amax.mtoken.dir/src/amax.mtoken.cpp.obj.d -o CMakeFiles/amax.mtoken.dir/src/amax.mtoken.cpp.obj -c /Users/richardchen/work/amax/amax.xchain/xchain.contracts/contracts/amax.mtoken/src/amax.mtoken.cpp

amax.mtoken/CMakeFiles/amax.mtoken.dir/src/amax.mtoken.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/amax.mtoken.dir/src/amax.mtoken.cpp.i"
	cd /Users/richardchen/work/amax/amax.xchain/xchain.contracts/build/contracts/amax.mtoken && /usr/local/amax.cdt/bin/amax-cpp $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/richardchen/work/amax/amax.xchain/xchain.contracts/contracts/amax.mtoken/src/amax.mtoken.cpp > CMakeFiles/amax.mtoken.dir/src/amax.mtoken.cpp.i

amax.mtoken/CMakeFiles/amax.mtoken.dir/src/amax.mtoken.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/amax.mtoken.dir/src/amax.mtoken.cpp.s"
	cd /Users/richardchen/work/amax/amax.xchain/xchain.contracts/build/contracts/amax.mtoken && /usr/local/amax.cdt/bin/amax-cpp $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/richardchen/work/amax/amax.xchain/xchain.contracts/contracts/amax.mtoken/src/amax.mtoken.cpp -o CMakeFiles/amax.mtoken.dir/src/amax.mtoken.cpp.s

# Object files for target amax.mtoken
amax_mtoken_OBJECTS = \
"CMakeFiles/amax.mtoken.dir/src/amax.mtoken.cpp.obj"

# External object files for target amax.mtoken
amax_mtoken_EXTERNAL_OBJECTS =

amax.mtoken/amax.mtoken.wasm: amax.mtoken/CMakeFiles/amax.mtoken.dir/src/amax.mtoken.cpp.obj
amax.mtoken/amax.mtoken.wasm: amax.mtoken/CMakeFiles/amax.mtoken.dir/build.make
amax.mtoken/amax.mtoken.wasm: amax.mtoken/CMakeFiles/amax.mtoken.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/richardchen/work/amax/amax.xchain/xchain.contracts/build/contracts/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable amax.mtoken.wasm"
	cd /Users/richardchen/work/amax/amax.xchain/xchain.contracts/build/contracts/amax.mtoken && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/amax.mtoken.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
amax.mtoken/CMakeFiles/amax.mtoken.dir/build: amax.mtoken/amax.mtoken.wasm
.PHONY : amax.mtoken/CMakeFiles/amax.mtoken.dir/build

amax.mtoken/CMakeFiles/amax.mtoken.dir/clean:
	cd /Users/richardchen/work/amax/amax.xchain/xchain.contracts/build/contracts/amax.mtoken && $(CMAKE_COMMAND) -P CMakeFiles/amax.mtoken.dir/cmake_clean.cmake
.PHONY : amax.mtoken/CMakeFiles/amax.mtoken.dir/clean

amax.mtoken/CMakeFiles/amax.mtoken.dir/depend:
	cd /Users/richardchen/work/amax/amax.xchain/xchain.contracts/build/contracts && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/richardchen/work/amax/amax.xchain/xchain.contracts/contracts /Users/richardchen/work/amax/amax.xchain/xchain.contracts/contracts/amax.mtoken /Users/richardchen/work/amax/amax.xchain/xchain.contracts/build/contracts /Users/richardchen/work/amax/amax.xchain/xchain.contracts/build/contracts/amax.mtoken /Users/richardchen/work/amax/amax.xchain/xchain.contracts/build/contracts/amax.mtoken/CMakeFiles/amax.mtoken.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : amax.mtoken/CMakeFiles/amax.mtoken.dir/depend
