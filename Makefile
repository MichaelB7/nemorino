# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.14

# Default target executed when no arguments are given to make.
default_target: all

.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:


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

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/opt/local/bin/cmake -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache

.PHONY : rebuild_cache/fast

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake cache editor..."
	/opt/local/bin/ccmake -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache

.PHONY : edit_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /Users/michaelbyrne/Github/nemorino/CMakeFiles /Users/michaelbyrne/Github/nemorino/CMakeFiles/progress.marks
	$(MAKE) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /Users/michaelbyrne/Github/nemorino/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean

.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named nemorino

# Build rule for target.
nemorino: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 nemorino
.PHONY : nemorino

# fast build rule for target.
nemorino/fast:
	$(MAKE) -f CMakeFiles/nemorino.dir/build.make CMakeFiles/nemorino.dir/build
.PHONY : nemorino/fast

Material.o: Material.cpp.o

.PHONY : Material.o

# target to build an object file
Material.cpp.o:
	$(MAKE) -f CMakeFiles/nemorino.dir/build.make CMakeFiles/nemorino.dir/Material.cpp.o
.PHONY : Material.cpp.o

Material.i: Material.cpp.i

.PHONY : Material.i

# target to preprocess a source file
Material.cpp.i:
	$(MAKE) -f CMakeFiles/nemorino.dir/build.make CMakeFiles/nemorino.dir/Material.cpp.i
.PHONY : Material.cpp.i

Material.s: Material.cpp.s

.PHONY : Material.s

# target to generate assembly for a file
Material.cpp.s:
	$(MAKE) -f CMakeFiles/nemorino.dir/build.make CMakeFiles/nemorino.dir/Material.cpp.s
.PHONY : Material.cpp.s

Nemorino.o: Nemorino.cpp.o

.PHONY : Nemorino.o

# target to build an object file
Nemorino.cpp.o:
	$(MAKE) -f CMakeFiles/nemorino.dir/build.make CMakeFiles/nemorino.dir/Nemorino.cpp.o
.PHONY : Nemorino.cpp.o

Nemorino.i: Nemorino.cpp.i

.PHONY : Nemorino.i

# target to preprocess a source file
Nemorino.cpp.i:
	$(MAKE) -f CMakeFiles/nemorino.dir/build.make CMakeFiles/nemorino.dir/Nemorino.cpp.i
.PHONY : Nemorino.cpp.i

Nemorino.s: Nemorino.cpp.s

.PHONY : Nemorino.s

# target to generate assembly for a file
Nemorino.cpp.s:
	$(MAKE) -f CMakeFiles/nemorino.dir/build.make CMakeFiles/nemorino.dir/Nemorino.cpp.s
.PHONY : Nemorino.cpp.s

bbEndings.o: bbEndings.cpp.o

.PHONY : bbEndings.o

# target to build an object file
bbEndings.cpp.o:
	$(MAKE) -f CMakeFiles/nemorino.dir/build.make CMakeFiles/nemorino.dir/bbEndings.cpp.o
.PHONY : bbEndings.cpp.o

bbEndings.i: bbEndings.cpp.i

.PHONY : bbEndings.i

# target to preprocess a source file
bbEndings.cpp.i:
	$(MAKE) -f CMakeFiles/nemorino.dir/build.make CMakeFiles/nemorino.dir/bbEndings.cpp.i
.PHONY : bbEndings.cpp.i

bbEndings.s: bbEndings.cpp.s

.PHONY : bbEndings.s

# target to generate assembly for a file
bbEndings.cpp.s:
	$(MAKE) -f CMakeFiles/nemorino.dir/build.make CMakeFiles/nemorino.dir/bbEndings.cpp.s
.PHONY : bbEndings.cpp.s

board.o: board.cpp.o

.PHONY : board.o

# target to build an object file
board.cpp.o:
	$(MAKE) -f CMakeFiles/nemorino.dir/build.make CMakeFiles/nemorino.dir/board.cpp.o
.PHONY : board.cpp.o

board.i: board.cpp.i

.PHONY : board.i

# target to preprocess a source file
board.cpp.i:
	$(MAKE) -f CMakeFiles/nemorino.dir/build.make CMakeFiles/nemorino.dir/board.cpp.i
.PHONY : board.cpp.i

board.s: board.cpp.s

.PHONY : board.s

# target to generate assembly for a file
board.cpp.s:
	$(MAKE) -f CMakeFiles/nemorino.dir/build.make CMakeFiles/nemorino.dir/board.cpp.s
.PHONY : board.cpp.s

book.o: book.cpp.o

.PHONY : book.o

# target to build an object file
book.cpp.o:
	$(MAKE) -f CMakeFiles/nemorino.dir/build.make CMakeFiles/nemorino.dir/book.cpp.o
.PHONY : book.cpp.o

book.i: book.cpp.i

.PHONY : book.i

# target to preprocess a source file
book.cpp.i:
	$(MAKE) -f CMakeFiles/nemorino.dir/build.make CMakeFiles/nemorino.dir/book.cpp.i
.PHONY : book.cpp.i

book.s: book.cpp.s

.PHONY : book.s

# target to generate assembly for a file
book.cpp.s:
	$(MAKE) -f CMakeFiles/nemorino.dir/build.make CMakeFiles/nemorino.dir/book.cpp.s
.PHONY : book.cpp.s

evaluation.o: evaluation.cpp.o

.PHONY : evaluation.o

# target to build an object file
evaluation.cpp.o:
	$(MAKE) -f CMakeFiles/nemorino.dir/build.make CMakeFiles/nemorino.dir/evaluation.cpp.o
.PHONY : evaluation.cpp.o

evaluation.i: evaluation.cpp.i

.PHONY : evaluation.i

# target to preprocess a source file
evaluation.cpp.i:
	$(MAKE) -f CMakeFiles/nemorino.dir/build.make CMakeFiles/nemorino.dir/evaluation.cpp.i
.PHONY : evaluation.cpp.i

evaluation.s: evaluation.cpp.s

.PHONY : evaluation.s

# target to generate assembly for a file
evaluation.cpp.s:
	$(MAKE) -f CMakeFiles/nemorino.dir/build.make CMakeFiles/nemorino.dir/evaluation.cpp.s
.PHONY : evaluation.cpp.s

hashtables.o: hashtables.cpp.o

.PHONY : hashtables.o

# target to build an object file
hashtables.cpp.o:
	$(MAKE) -f CMakeFiles/nemorino.dir/build.make CMakeFiles/nemorino.dir/hashtables.cpp.o
.PHONY : hashtables.cpp.o

hashtables.i: hashtables.cpp.i

.PHONY : hashtables.i

# target to preprocess a source file
hashtables.cpp.i:
	$(MAKE) -f CMakeFiles/nemorino.dir/build.make CMakeFiles/nemorino.dir/hashtables.cpp.i
.PHONY : hashtables.cpp.i

hashtables.s: hashtables.cpp.s

.PHONY : hashtables.s

# target to generate assembly for a file
hashtables.cpp.s:
	$(MAKE) -f CMakeFiles/nemorino.dir/build.make CMakeFiles/nemorino.dir/hashtables.cpp.s
.PHONY : hashtables.cpp.s

position.o: position.cpp.o

.PHONY : position.o

# target to build an object file
position.cpp.o:
	$(MAKE) -f CMakeFiles/nemorino.dir/build.make CMakeFiles/nemorino.dir/position.cpp.o
.PHONY : position.cpp.o

position.i: position.cpp.i

.PHONY : position.i

# target to preprocess a source file
position.cpp.i:
	$(MAKE) -f CMakeFiles/nemorino.dir/build.make CMakeFiles/nemorino.dir/position.cpp.i
.PHONY : position.cpp.i

position.s: position.cpp.s

.PHONY : position.s

# target to generate assembly for a file
position.cpp.s:
	$(MAKE) -f CMakeFiles/nemorino.dir/build.make CMakeFiles/nemorino.dir/position.cpp.s
.PHONY : position.cpp.s

search.o: search.cpp.o

.PHONY : search.o

# target to build an object file
search.cpp.o:
	$(MAKE) -f CMakeFiles/nemorino.dir/build.make CMakeFiles/nemorino.dir/search.cpp.o
.PHONY : search.cpp.o

search.i: search.cpp.i

.PHONY : search.i

# target to preprocess a source file
search.cpp.i:
	$(MAKE) -f CMakeFiles/nemorino.dir/build.make CMakeFiles/nemorino.dir/search.cpp.i
.PHONY : search.cpp.i

search.s: search.cpp.s

.PHONY : search.s

# target to generate assembly for a file
search.cpp.s:
	$(MAKE) -f CMakeFiles/nemorino.dir/build.make CMakeFiles/nemorino.dir/search.cpp.s
.PHONY : search.cpp.s

settings.o: settings.cpp.o

.PHONY : settings.o

# target to build an object file
settings.cpp.o:
	$(MAKE) -f CMakeFiles/nemorino.dir/build.make CMakeFiles/nemorino.dir/settings.cpp.o
.PHONY : settings.cpp.o

settings.i: settings.cpp.i

.PHONY : settings.i

# target to preprocess a source file
settings.cpp.i:
	$(MAKE) -f CMakeFiles/nemorino.dir/build.make CMakeFiles/nemorino.dir/settings.cpp.i
.PHONY : settings.cpp.i

settings.s: settings.cpp.s

.PHONY : settings.s

# target to generate assembly for a file
settings.cpp.s:
	$(MAKE) -f CMakeFiles/nemorino.dir/build.make CMakeFiles/nemorino.dir/settings.cpp.s
.PHONY : settings.cpp.s

tbprobe.o: tbprobe.cpp.o

.PHONY : tbprobe.o

# target to build an object file
tbprobe.cpp.o:
	$(MAKE) -f CMakeFiles/nemorino.dir/build.make CMakeFiles/nemorino.dir/tbprobe.cpp.o
.PHONY : tbprobe.cpp.o

tbprobe.i: tbprobe.cpp.i

.PHONY : tbprobe.i

# target to preprocess a source file
tbprobe.cpp.i:
	$(MAKE) -f CMakeFiles/nemorino.dir/build.make CMakeFiles/nemorino.dir/tbprobe.cpp.i
.PHONY : tbprobe.cpp.i

tbprobe.s: tbprobe.cpp.s

.PHONY : tbprobe.s

# target to generate assembly for a file
tbprobe.cpp.s:
	$(MAKE) -f CMakeFiles/nemorino.dir/build.make CMakeFiles/nemorino.dir/tbprobe.cpp.s
.PHONY : tbprobe.cpp.s

test.o: test.cpp.o

.PHONY : test.o

# target to build an object file
test.cpp.o:
	$(MAKE) -f CMakeFiles/nemorino.dir/build.make CMakeFiles/nemorino.dir/test.cpp.o
.PHONY : test.cpp.o

test.i: test.cpp.i

.PHONY : test.i

# target to preprocess a source file
test.cpp.i:
	$(MAKE) -f CMakeFiles/nemorino.dir/build.make CMakeFiles/nemorino.dir/test.cpp.i
.PHONY : test.cpp.i

test.s: test.cpp.s

.PHONY : test.s

# target to generate assembly for a file
test.cpp.s:
	$(MAKE) -f CMakeFiles/nemorino.dir/build.make CMakeFiles/nemorino.dir/test.cpp.s
.PHONY : test.cpp.s

timemanager.o: timemanager.cpp.o

.PHONY : timemanager.o

# target to build an object file
timemanager.cpp.o:
	$(MAKE) -f CMakeFiles/nemorino.dir/build.make CMakeFiles/nemorino.dir/timemanager.cpp.o
.PHONY : timemanager.cpp.o

timemanager.i: timemanager.cpp.i

.PHONY : timemanager.i

# target to preprocess a source file
timemanager.cpp.i:
	$(MAKE) -f CMakeFiles/nemorino.dir/build.make CMakeFiles/nemorino.dir/timemanager.cpp.i
.PHONY : timemanager.cpp.i

timemanager.s: timemanager.cpp.s

.PHONY : timemanager.s

# target to generate assembly for a file
timemanager.cpp.s:
	$(MAKE) -f CMakeFiles/nemorino.dir/build.make CMakeFiles/nemorino.dir/timemanager.cpp.s
.PHONY : timemanager.cpp.s

uci.o: uci.cpp.o

.PHONY : uci.o

# target to build an object file
uci.cpp.o:
	$(MAKE) -f CMakeFiles/nemorino.dir/build.make CMakeFiles/nemorino.dir/uci.cpp.o
.PHONY : uci.cpp.o

uci.i: uci.cpp.i

.PHONY : uci.i

# target to preprocess a source file
uci.cpp.i:
	$(MAKE) -f CMakeFiles/nemorino.dir/build.make CMakeFiles/nemorino.dir/uci.cpp.i
.PHONY : uci.cpp.i

uci.s: uci.cpp.s

.PHONY : uci.s

# target to generate assembly for a file
uci.cpp.s:
	$(MAKE) -f CMakeFiles/nemorino.dir/build.make CMakeFiles/nemorino.dir/uci.cpp.s
.PHONY : uci.cpp.s

utils.o: utils.cpp.o

.PHONY : utils.o

# target to build an object file
utils.cpp.o:
	$(MAKE) -f CMakeFiles/nemorino.dir/build.make CMakeFiles/nemorino.dir/utils.cpp.o
.PHONY : utils.cpp.o

utils.i: utils.cpp.i

.PHONY : utils.i

# target to preprocess a source file
utils.cpp.i:
	$(MAKE) -f CMakeFiles/nemorino.dir/build.make CMakeFiles/nemorino.dir/utils.cpp.i
.PHONY : utils.cpp.i

utils.s: utils.cpp.s

.PHONY : utils.s

# target to generate assembly for a file
utils.cpp.s:
	$(MAKE) -f CMakeFiles/nemorino.dir/build.make CMakeFiles/nemorino.dir/utils.cpp.s
.PHONY : utils.cpp.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... rebuild_cache"
	@echo "... edit_cache"
	@echo "... nemorino"
	@echo "... Material.o"
	@echo "... Material.i"
	@echo "... Material.s"
	@echo "... Nemorino.o"
	@echo "... Nemorino.i"
	@echo "... Nemorino.s"
	@echo "... bbEndings.o"
	@echo "... bbEndings.i"
	@echo "... bbEndings.s"
	@echo "... board.o"
	@echo "... board.i"
	@echo "... board.s"
	@echo "... book.o"
	@echo "... book.i"
	@echo "... book.s"
	@echo "... evaluation.o"
	@echo "... evaluation.i"
	@echo "... evaluation.s"
	@echo "... hashtables.o"
	@echo "... hashtables.i"
	@echo "... hashtables.s"
	@echo "... position.o"
	@echo "... position.i"
	@echo "... position.s"
	@echo "... search.o"
	@echo "... search.i"
	@echo "... search.s"
	@echo "... settings.o"
	@echo "... settings.i"
	@echo "... settings.s"
	@echo "... tbprobe.o"
	@echo "... tbprobe.i"
	@echo "... tbprobe.s"
	@echo "... test.o"
	@echo "... test.i"
	@echo "... test.s"
	@echo "... timemanager.o"
	@echo "... timemanager.i"
	@echo "... timemanager.s"
	@echo "... uci.o"
	@echo "... uci.i"
	@echo "... uci.s"
	@echo "... utils.o"
	@echo "... utils.i"
	@echo "... utils.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

