# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/zachary/src/lain

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/zachary/src/lain/build

# Include any dependencies generated for this target.
include CMakeFiles/Lexer.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/Lexer.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Lexer.dir/flags.make

CMakeFiles/Lexer.dir/src/lexer/lexer_dump.cpp.o: CMakeFiles/Lexer.dir/flags.make
CMakeFiles/Lexer.dir/src/lexer/lexer_dump.cpp.o: ../src/lexer/lexer_dump.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/zachary/src/lain/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Lexer.dir/src/lexer/lexer_dump.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Lexer.dir/src/lexer/lexer_dump.cpp.o -c /home/zachary/src/lain/src/lexer/lexer_dump.cpp

CMakeFiles/Lexer.dir/src/lexer/lexer_dump.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Lexer.dir/src/lexer/lexer_dump.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/zachary/src/lain/src/lexer/lexer_dump.cpp > CMakeFiles/Lexer.dir/src/lexer/lexer_dump.cpp.i

CMakeFiles/Lexer.dir/src/lexer/lexer_dump.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Lexer.dir/src/lexer/lexer_dump.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/zachary/src/lain/src/lexer/lexer_dump.cpp -o CMakeFiles/Lexer.dir/src/lexer/lexer_dump.cpp.s

CMakeFiles/Lexer.dir/src/lexer/lexer_dump.cpp.o.requires:

.PHONY : CMakeFiles/Lexer.dir/src/lexer/lexer_dump.cpp.o.requires

CMakeFiles/Lexer.dir/src/lexer/lexer_dump.cpp.o.provides: CMakeFiles/Lexer.dir/src/lexer/lexer_dump.cpp.o.requires
	$(MAKE) -f CMakeFiles/Lexer.dir/build.make CMakeFiles/Lexer.dir/src/lexer/lexer_dump.cpp.o.provides.build
.PHONY : CMakeFiles/Lexer.dir/src/lexer/lexer_dump.cpp.o.provides

CMakeFiles/Lexer.dir/src/lexer/lexer_dump.cpp.o.provides.build: CMakeFiles/Lexer.dir/src/lexer/lexer_dump.cpp.o


CMakeFiles/Lexer.dir/src/lexer/lexer.cpp.o: CMakeFiles/Lexer.dir/flags.make
CMakeFiles/Lexer.dir/src/lexer/lexer.cpp.o: ../src/lexer/lexer.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/zachary/src/lain/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/Lexer.dir/src/lexer/lexer.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Lexer.dir/src/lexer/lexer.cpp.o -c /home/zachary/src/lain/src/lexer/lexer.cpp

CMakeFiles/Lexer.dir/src/lexer/lexer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Lexer.dir/src/lexer/lexer.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/zachary/src/lain/src/lexer/lexer.cpp > CMakeFiles/Lexer.dir/src/lexer/lexer.cpp.i

CMakeFiles/Lexer.dir/src/lexer/lexer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Lexer.dir/src/lexer/lexer.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/zachary/src/lain/src/lexer/lexer.cpp -o CMakeFiles/Lexer.dir/src/lexer/lexer.cpp.s

CMakeFiles/Lexer.dir/src/lexer/lexer.cpp.o.requires:

.PHONY : CMakeFiles/Lexer.dir/src/lexer/lexer.cpp.o.requires

CMakeFiles/Lexer.dir/src/lexer/lexer.cpp.o.provides: CMakeFiles/Lexer.dir/src/lexer/lexer.cpp.o.requires
	$(MAKE) -f CMakeFiles/Lexer.dir/build.make CMakeFiles/Lexer.dir/src/lexer/lexer.cpp.o.provides.build
.PHONY : CMakeFiles/Lexer.dir/src/lexer/lexer.cpp.o.provides

CMakeFiles/Lexer.dir/src/lexer/lexer.cpp.o.provides.build: CMakeFiles/Lexer.dir/src/lexer/lexer.cpp.o


# Object files for target Lexer
Lexer_OBJECTS = \
"CMakeFiles/Lexer.dir/src/lexer/lexer_dump.cpp.o" \
"CMakeFiles/Lexer.dir/src/lexer/lexer.cpp.o"

# External object files for target Lexer
Lexer_EXTERNAL_OBJECTS =

Lexer: CMakeFiles/Lexer.dir/src/lexer/lexer_dump.cpp.o
Lexer: CMakeFiles/Lexer.dir/src/lexer/lexer.cpp.o
Lexer: CMakeFiles/Lexer.dir/build.make
Lexer: CMakeFiles/Lexer.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/zachary/src/lain/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable Lexer"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Lexer.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Lexer.dir/build: Lexer

.PHONY : CMakeFiles/Lexer.dir/build

CMakeFiles/Lexer.dir/requires: CMakeFiles/Lexer.dir/src/lexer/lexer_dump.cpp.o.requires
CMakeFiles/Lexer.dir/requires: CMakeFiles/Lexer.dir/src/lexer/lexer.cpp.o.requires

.PHONY : CMakeFiles/Lexer.dir/requires

CMakeFiles/Lexer.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Lexer.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Lexer.dir/clean

CMakeFiles/Lexer.dir/depend:
	cd /home/zachary/src/lain/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/zachary/src/lain /home/zachary/src/lain /home/zachary/src/lain/build /home/zachary/src/lain/build /home/zachary/src/lain/build/CMakeFiles/Lexer.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Lexer.dir/depend
