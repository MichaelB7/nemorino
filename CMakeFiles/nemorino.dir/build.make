# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.14

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
CMAKE_COMMAND = /opt/local/bin/cmake

# The command to remove a file.
RM = /opt/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/michaelbyrne/Github/nemorino/src

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/michaelbyrne/Github/nemorino

# Include any dependencies generated for this target.
include CMakeFiles/nemorino.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/nemorino.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/nemorino.dir/flags.make

CMakeFiles/nemorino.dir/Nemorino.cpp.o: CMakeFiles/nemorino.dir/flags.make
CMakeFiles/nemorino.dir/Nemorino.cpp.o: src/Nemorino.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/michaelbyrne/Github/nemorino/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/nemorino.dir/Nemorino.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/nemorino.dir/Nemorino.cpp.o -c /Users/michaelbyrne/Github/nemorino/src/Nemorino.cpp

CMakeFiles/nemorino.dir/Nemorino.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/nemorino.dir/Nemorino.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/michaelbyrne/Github/nemorino/src/Nemorino.cpp > CMakeFiles/nemorino.dir/Nemorino.cpp.i

CMakeFiles/nemorino.dir/Nemorino.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/nemorino.dir/Nemorino.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/michaelbyrne/Github/nemorino/src/Nemorino.cpp -o CMakeFiles/nemorino.dir/Nemorino.cpp.s

CMakeFiles/nemorino.dir/board.cpp.o: CMakeFiles/nemorino.dir/flags.make
CMakeFiles/nemorino.dir/board.cpp.o: src/board.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/michaelbyrne/Github/nemorino/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/nemorino.dir/board.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/nemorino.dir/board.cpp.o -c /Users/michaelbyrne/Github/nemorino/src/board.cpp

CMakeFiles/nemorino.dir/board.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/nemorino.dir/board.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/michaelbyrne/Github/nemorino/src/board.cpp > CMakeFiles/nemorino.dir/board.cpp.i

CMakeFiles/nemorino.dir/board.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/nemorino.dir/board.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/michaelbyrne/Github/nemorino/src/board.cpp -o CMakeFiles/nemorino.dir/board.cpp.s

CMakeFiles/nemorino.dir/book.cpp.o: CMakeFiles/nemorino.dir/flags.make
CMakeFiles/nemorino.dir/book.cpp.o: src/book.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/michaelbyrne/Github/nemorino/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/nemorino.dir/book.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/nemorino.dir/book.cpp.o -c /Users/michaelbyrne/Github/nemorino/src/book.cpp

CMakeFiles/nemorino.dir/book.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/nemorino.dir/book.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/michaelbyrne/Github/nemorino/src/book.cpp > CMakeFiles/nemorino.dir/book.cpp.i

CMakeFiles/nemorino.dir/book.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/nemorino.dir/book.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/michaelbyrne/Github/nemorino/src/book.cpp -o CMakeFiles/nemorino.dir/book.cpp.s

CMakeFiles/nemorino.dir/evaluation.cpp.o: CMakeFiles/nemorino.dir/flags.make
CMakeFiles/nemorino.dir/evaluation.cpp.o: src/evaluation.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/michaelbyrne/Github/nemorino/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/nemorino.dir/evaluation.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/nemorino.dir/evaluation.cpp.o -c /Users/michaelbyrne/Github/nemorino/src/evaluation.cpp

CMakeFiles/nemorino.dir/evaluation.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/nemorino.dir/evaluation.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/michaelbyrne/Github/nemorino/src/evaluation.cpp > CMakeFiles/nemorino.dir/evaluation.cpp.i

CMakeFiles/nemorino.dir/evaluation.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/nemorino.dir/evaluation.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/michaelbyrne/Github/nemorino/src/evaluation.cpp -o CMakeFiles/nemorino.dir/evaluation.cpp.s

CMakeFiles/nemorino.dir/tbprobe.cpp.o: CMakeFiles/nemorino.dir/flags.make
CMakeFiles/nemorino.dir/tbprobe.cpp.o: src/tbprobe.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/michaelbyrne/Github/nemorino/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/nemorino.dir/tbprobe.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/nemorino.dir/tbprobe.cpp.o -c /Users/michaelbyrne/Github/nemorino/src/tbprobe.cpp

CMakeFiles/nemorino.dir/tbprobe.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/nemorino.dir/tbprobe.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/michaelbyrne/Github/nemorino/src/tbprobe.cpp > CMakeFiles/nemorino.dir/tbprobe.cpp.i

CMakeFiles/nemorino.dir/tbprobe.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/nemorino.dir/tbprobe.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/michaelbyrne/Github/nemorino/src/tbprobe.cpp -o CMakeFiles/nemorino.dir/tbprobe.cpp.s

CMakeFiles/nemorino.dir/utils.cpp.o: CMakeFiles/nemorino.dir/flags.make
CMakeFiles/nemorino.dir/utils.cpp.o: src/utils.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/michaelbyrne/Github/nemorino/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/nemorino.dir/utils.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/nemorino.dir/utils.cpp.o -c /Users/michaelbyrne/Github/nemorino/src/utils.cpp

CMakeFiles/nemorino.dir/utils.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/nemorino.dir/utils.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/michaelbyrne/Github/nemorino/src/utils.cpp > CMakeFiles/nemorino.dir/utils.cpp.i

CMakeFiles/nemorino.dir/utils.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/nemorino.dir/utils.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/michaelbyrne/Github/nemorino/src/utils.cpp -o CMakeFiles/nemorino.dir/utils.cpp.s

CMakeFiles/nemorino.dir/hashtables.cpp.o: CMakeFiles/nemorino.dir/flags.make
CMakeFiles/nemorino.dir/hashtables.cpp.o: src/hashtables.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/michaelbyrne/Github/nemorino/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/nemorino.dir/hashtables.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/nemorino.dir/hashtables.cpp.o -c /Users/michaelbyrne/Github/nemorino/src/hashtables.cpp

CMakeFiles/nemorino.dir/hashtables.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/nemorino.dir/hashtables.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/michaelbyrne/Github/nemorino/src/hashtables.cpp > CMakeFiles/nemorino.dir/hashtables.cpp.i

CMakeFiles/nemorino.dir/hashtables.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/nemorino.dir/hashtables.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/michaelbyrne/Github/nemorino/src/hashtables.cpp -o CMakeFiles/nemorino.dir/hashtables.cpp.s

CMakeFiles/nemorino.dir/bbEndings.cpp.o: CMakeFiles/nemorino.dir/flags.make
CMakeFiles/nemorino.dir/bbEndings.cpp.o: src/bbEndings.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/michaelbyrne/Github/nemorino/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object CMakeFiles/nemorino.dir/bbEndings.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/nemorino.dir/bbEndings.cpp.o -c /Users/michaelbyrne/Github/nemorino/src/bbEndings.cpp

CMakeFiles/nemorino.dir/bbEndings.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/nemorino.dir/bbEndings.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/michaelbyrne/Github/nemorino/src/bbEndings.cpp > CMakeFiles/nemorino.dir/bbEndings.cpp.i

CMakeFiles/nemorino.dir/bbEndings.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/nemorino.dir/bbEndings.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/michaelbyrne/Github/nemorino/src/bbEndings.cpp -o CMakeFiles/nemorino.dir/bbEndings.cpp.s

CMakeFiles/nemorino.dir/Material.cpp.o: CMakeFiles/nemorino.dir/flags.make
CMakeFiles/nemorino.dir/Material.cpp.o: src/Material.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/michaelbyrne/Github/nemorino/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object CMakeFiles/nemorino.dir/Material.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/nemorino.dir/Material.cpp.o -c /Users/michaelbyrne/Github/nemorino/src/Material.cpp

CMakeFiles/nemorino.dir/Material.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/nemorino.dir/Material.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/michaelbyrne/Github/nemorino/src/Material.cpp > CMakeFiles/nemorino.dir/Material.cpp.i

CMakeFiles/nemorino.dir/Material.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/nemorino.dir/Material.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/michaelbyrne/Github/nemorino/src/Material.cpp -o CMakeFiles/nemorino.dir/Material.cpp.s

CMakeFiles/nemorino.dir/position.cpp.o: CMakeFiles/nemorino.dir/flags.make
CMakeFiles/nemorino.dir/position.cpp.o: src/position.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/michaelbyrne/Github/nemorino/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building CXX object CMakeFiles/nemorino.dir/position.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/nemorino.dir/position.cpp.o -c /Users/michaelbyrne/Github/nemorino/src/position.cpp

CMakeFiles/nemorino.dir/position.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/nemorino.dir/position.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/michaelbyrne/Github/nemorino/src/position.cpp > CMakeFiles/nemorino.dir/position.cpp.i

CMakeFiles/nemorino.dir/position.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/nemorino.dir/position.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/michaelbyrne/Github/nemorino/src/position.cpp -o CMakeFiles/nemorino.dir/position.cpp.s

CMakeFiles/nemorino.dir/search.cpp.o: CMakeFiles/nemorino.dir/flags.make
CMakeFiles/nemorino.dir/search.cpp.o: src/search.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/michaelbyrne/Github/nemorino/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Building CXX object CMakeFiles/nemorino.dir/search.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/nemorino.dir/search.cpp.o -c /Users/michaelbyrne/Github/nemorino/src/search.cpp

CMakeFiles/nemorino.dir/search.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/nemorino.dir/search.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/michaelbyrne/Github/nemorino/src/search.cpp > CMakeFiles/nemorino.dir/search.cpp.i

CMakeFiles/nemorino.dir/search.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/nemorino.dir/search.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/michaelbyrne/Github/nemorino/src/search.cpp -o CMakeFiles/nemorino.dir/search.cpp.s

CMakeFiles/nemorino.dir/settings.cpp.o: CMakeFiles/nemorino.dir/flags.make
CMakeFiles/nemorino.dir/settings.cpp.o: src/settings.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/michaelbyrne/Github/nemorino/CMakeFiles --progress-num=$(CMAKE_PROGRESS_12) "Building CXX object CMakeFiles/nemorino.dir/settings.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/nemorino.dir/settings.cpp.o -c /Users/michaelbyrne/Github/nemorino/src/settings.cpp

CMakeFiles/nemorino.dir/settings.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/nemorino.dir/settings.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/michaelbyrne/Github/nemorino/src/settings.cpp > CMakeFiles/nemorino.dir/settings.cpp.i

CMakeFiles/nemorino.dir/settings.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/nemorino.dir/settings.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/michaelbyrne/Github/nemorino/src/settings.cpp -o CMakeFiles/nemorino.dir/settings.cpp.s

CMakeFiles/nemorino.dir/test.cpp.o: CMakeFiles/nemorino.dir/flags.make
CMakeFiles/nemorino.dir/test.cpp.o: src/test.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/michaelbyrne/Github/nemorino/CMakeFiles --progress-num=$(CMAKE_PROGRESS_13) "Building CXX object CMakeFiles/nemorino.dir/test.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/nemorino.dir/test.cpp.o -c /Users/michaelbyrne/Github/nemorino/src/test.cpp

CMakeFiles/nemorino.dir/test.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/nemorino.dir/test.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/michaelbyrne/Github/nemorino/src/test.cpp > CMakeFiles/nemorino.dir/test.cpp.i

CMakeFiles/nemorino.dir/test.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/nemorino.dir/test.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/michaelbyrne/Github/nemorino/src/test.cpp -o CMakeFiles/nemorino.dir/test.cpp.s

CMakeFiles/nemorino.dir/timemanager.cpp.o: CMakeFiles/nemorino.dir/flags.make
CMakeFiles/nemorino.dir/timemanager.cpp.o: src/timemanager.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/michaelbyrne/Github/nemorino/CMakeFiles --progress-num=$(CMAKE_PROGRESS_14) "Building CXX object CMakeFiles/nemorino.dir/timemanager.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/nemorino.dir/timemanager.cpp.o -c /Users/michaelbyrne/Github/nemorino/src/timemanager.cpp

CMakeFiles/nemorino.dir/timemanager.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/nemorino.dir/timemanager.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/michaelbyrne/Github/nemorino/src/timemanager.cpp > CMakeFiles/nemorino.dir/timemanager.cpp.i

CMakeFiles/nemorino.dir/timemanager.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/nemorino.dir/timemanager.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/michaelbyrne/Github/nemorino/src/timemanager.cpp -o CMakeFiles/nemorino.dir/timemanager.cpp.s

CMakeFiles/nemorino.dir/uci.cpp.o: CMakeFiles/nemorino.dir/flags.make
CMakeFiles/nemorino.dir/uci.cpp.o: src/uci.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/michaelbyrne/Github/nemorino/CMakeFiles --progress-num=$(CMAKE_PROGRESS_15) "Building CXX object CMakeFiles/nemorino.dir/uci.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/nemorino.dir/uci.cpp.o -c /Users/michaelbyrne/Github/nemorino/src/uci.cpp

CMakeFiles/nemorino.dir/uci.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/nemorino.dir/uci.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/michaelbyrne/Github/nemorino/src/uci.cpp > CMakeFiles/nemorino.dir/uci.cpp.i

CMakeFiles/nemorino.dir/uci.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/nemorino.dir/uci.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/michaelbyrne/Github/nemorino/src/uci.cpp -o CMakeFiles/nemorino.dir/uci.cpp.s

# Object files for target nemorino
nemorino_OBJECTS = \
"CMakeFiles/nemorino.dir/Nemorino.cpp.o" \
"CMakeFiles/nemorino.dir/board.cpp.o" \
"CMakeFiles/nemorino.dir/book.cpp.o" \
"CMakeFiles/nemorino.dir/evaluation.cpp.o" \
"CMakeFiles/nemorino.dir/tbprobe.cpp.o" \
"CMakeFiles/nemorino.dir/utils.cpp.o" \
"CMakeFiles/nemorino.dir/hashtables.cpp.o" \
"CMakeFiles/nemorino.dir/bbEndings.cpp.o" \
"CMakeFiles/nemorino.dir/Material.cpp.o" \
"CMakeFiles/nemorino.dir/position.cpp.o" \
"CMakeFiles/nemorino.dir/search.cpp.o" \
"CMakeFiles/nemorino.dir/settings.cpp.o" \
"CMakeFiles/nemorino.dir/test.cpp.o" \
"CMakeFiles/nemorino.dir/timemanager.cpp.o" \
"CMakeFiles/nemorino.dir/uci.cpp.o"

# External object files for target nemorino
nemorino_EXTERNAL_OBJECTS =

nemorino: CMakeFiles/nemorino.dir/Nemorino.cpp.o
nemorino: CMakeFiles/nemorino.dir/board.cpp.o
nemorino: CMakeFiles/nemorino.dir/book.cpp.o
nemorino: CMakeFiles/nemorino.dir/evaluation.cpp.o
nemorino: CMakeFiles/nemorino.dir/tbprobe.cpp.o
nemorino: CMakeFiles/nemorino.dir/utils.cpp.o
nemorino: CMakeFiles/nemorino.dir/hashtables.cpp.o
nemorino: CMakeFiles/nemorino.dir/bbEndings.cpp.o
nemorino: CMakeFiles/nemorino.dir/Material.cpp.o
nemorino: CMakeFiles/nemorino.dir/position.cpp.o
nemorino: CMakeFiles/nemorino.dir/search.cpp.o
nemorino: CMakeFiles/nemorino.dir/settings.cpp.o
nemorino: CMakeFiles/nemorino.dir/test.cpp.o
nemorino: CMakeFiles/nemorino.dir/timemanager.cpp.o
nemorino: CMakeFiles/nemorino.dir/uci.cpp.o
nemorino: CMakeFiles/nemorino.dir/build.make
nemorino: CMakeFiles/nemorino.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/michaelbyrne/Github/nemorino/CMakeFiles --progress-num=$(CMAKE_PROGRESS_16) "Linking CXX executable nemorino"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/nemorino.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/nemorino.dir/build: nemorino

.PHONY : CMakeFiles/nemorino.dir/build

CMakeFiles/nemorino.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/nemorino.dir/cmake_clean.cmake
.PHONY : CMakeFiles/nemorino.dir/clean

CMakeFiles/nemorino.dir/depend:
	cd /Users/michaelbyrne/Github/nemorino && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/michaelbyrne/Github/nemorino/src /Users/michaelbyrne/Github/nemorino/src /Users/michaelbyrne/Github/nemorino /Users/michaelbyrne/Github/nemorino /Users/michaelbyrne/Github/nemorino/CMakeFiles/nemorino.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/nemorino.dir/depend

