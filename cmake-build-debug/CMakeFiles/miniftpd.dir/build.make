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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /tmp/tmp.qQGCGXiSq4

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /tmp/tmp.qQGCGXiSq4/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/miniftpd.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/miniftpd.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/miniftpd.dir/flags.make

CMakeFiles/miniftpd.dir/main.c.o: CMakeFiles/miniftpd.dir/flags.make
CMakeFiles/miniftpd.dir/main.c.o: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/tmp.qQGCGXiSq4/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/miniftpd.dir/main.c.o"
	/usr/lib64/ccache/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/miniftpd.dir/main.c.o   -c /tmp/tmp.qQGCGXiSq4/main.c

CMakeFiles/miniftpd.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/miniftpd.dir/main.c.i"
	/usr/lib64/ccache/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /tmp/tmp.qQGCGXiSq4/main.c > CMakeFiles/miniftpd.dir/main.c.i

CMakeFiles/miniftpd.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/miniftpd.dir/main.c.s"
	/usr/lib64/ccache/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /tmp/tmp.qQGCGXiSq4/main.c -o CMakeFiles/miniftpd.dir/main.c.s

CMakeFiles/miniftpd.dir/sysutil.c.o: CMakeFiles/miniftpd.dir/flags.make
CMakeFiles/miniftpd.dir/sysutil.c.o: ../sysutil.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/tmp.qQGCGXiSq4/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/miniftpd.dir/sysutil.c.o"
	/usr/lib64/ccache/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/miniftpd.dir/sysutil.c.o   -c /tmp/tmp.qQGCGXiSq4/sysutil.c

CMakeFiles/miniftpd.dir/sysutil.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/miniftpd.dir/sysutil.c.i"
	/usr/lib64/ccache/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /tmp/tmp.qQGCGXiSq4/sysutil.c > CMakeFiles/miniftpd.dir/sysutil.c.i

CMakeFiles/miniftpd.dir/sysutil.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/miniftpd.dir/sysutil.c.s"
	/usr/lib64/ccache/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /tmp/tmp.qQGCGXiSq4/sysutil.c -o CMakeFiles/miniftpd.dir/sysutil.c.s

CMakeFiles/miniftpd.dir/common.c.o: CMakeFiles/miniftpd.dir/flags.make
CMakeFiles/miniftpd.dir/common.c.o: ../common.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/tmp.qQGCGXiSq4/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/miniftpd.dir/common.c.o"
	/usr/lib64/ccache/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/miniftpd.dir/common.c.o   -c /tmp/tmp.qQGCGXiSq4/common.c

CMakeFiles/miniftpd.dir/common.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/miniftpd.dir/common.c.i"
	/usr/lib64/ccache/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /tmp/tmp.qQGCGXiSq4/common.c > CMakeFiles/miniftpd.dir/common.c.i

CMakeFiles/miniftpd.dir/common.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/miniftpd.dir/common.c.s"
	/usr/lib64/ccache/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /tmp/tmp.qQGCGXiSq4/common.c -o CMakeFiles/miniftpd.dir/common.c.s

CMakeFiles/miniftpd.dir/session.c.o: CMakeFiles/miniftpd.dir/flags.make
CMakeFiles/miniftpd.dir/session.c.o: ../session.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/tmp.qQGCGXiSq4/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object CMakeFiles/miniftpd.dir/session.c.o"
	/usr/lib64/ccache/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/miniftpd.dir/session.c.o   -c /tmp/tmp.qQGCGXiSq4/session.c

CMakeFiles/miniftpd.dir/session.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/miniftpd.dir/session.c.i"
	/usr/lib64/ccache/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /tmp/tmp.qQGCGXiSq4/session.c > CMakeFiles/miniftpd.dir/session.c.i

CMakeFiles/miniftpd.dir/session.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/miniftpd.dir/session.c.s"
	/usr/lib64/ccache/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /tmp/tmp.qQGCGXiSq4/session.c -o CMakeFiles/miniftpd.dir/session.c.s

CMakeFiles/miniftpd.dir/ftpproto.c.o: CMakeFiles/miniftpd.dir/flags.make
CMakeFiles/miniftpd.dir/ftpproto.c.o: ../ftpproto.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/tmp.qQGCGXiSq4/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object CMakeFiles/miniftpd.dir/ftpproto.c.o"
	/usr/lib64/ccache/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/miniftpd.dir/ftpproto.c.o   -c /tmp/tmp.qQGCGXiSq4/ftpproto.c

CMakeFiles/miniftpd.dir/ftpproto.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/miniftpd.dir/ftpproto.c.i"
	/usr/lib64/ccache/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /tmp/tmp.qQGCGXiSq4/ftpproto.c > CMakeFiles/miniftpd.dir/ftpproto.c.i

CMakeFiles/miniftpd.dir/ftpproto.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/miniftpd.dir/ftpproto.c.s"
	/usr/lib64/ccache/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /tmp/tmp.qQGCGXiSq4/ftpproto.c -o CMakeFiles/miniftpd.dir/ftpproto.c.s

CMakeFiles/miniftpd.dir/privparent.c.o: CMakeFiles/miniftpd.dir/flags.make
CMakeFiles/miniftpd.dir/privparent.c.o: ../privparent.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/tmp.qQGCGXiSq4/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building C object CMakeFiles/miniftpd.dir/privparent.c.o"
	/usr/lib64/ccache/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/miniftpd.dir/privparent.c.o   -c /tmp/tmp.qQGCGXiSq4/privparent.c

CMakeFiles/miniftpd.dir/privparent.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/miniftpd.dir/privparent.c.i"
	/usr/lib64/ccache/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /tmp/tmp.qQGCGXiSq4/privparent.c > CMakeFiles/miniftpd.dir/privparent.c.i

CMakeFiles/miniftpd.dir/privparent.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/miniftpd.dir/privparent.c.s"
	/usr/lib64/ccache/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /tmp/tmp.qQGCGXiSq4/privparent.c -o CMakeFiles/miniftpd.dir/privparent.c.s

CMakeFiles/miniftpd.dir/str.c.o: CMakeFiles/miniftpd.dir/flags.make
CMakeFiles/miniftpd.dir/str.c.o: ../str.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/tmp.qQGCGXiSq4/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building C object CMakeFiles/miniftpd.dir/str.c.o"
	/usr/lib64/ccache/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/miniftpd.dir/str.c.o   -c /tmp/tmp.qQGCGXiSq4/str.c

CMakeFiles/miniftpd.dir/str.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/miniftpd.dir/str.c.i"
	/usr/lib64/ccache/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /tmp/tmp.qQGCGXiSq4/str.c > CMakeFiles/miniftpd.dir/str.c.i

CMakeFiles/miniftpd.dir/str.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/miniftpd.dir/str.c.s"
	/usr/lib64/ccache/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /tmp/tmp.qQGCGXiSq4/str.c -o CMakeFiles/miniftpd.dir/str.c.s

CMakeFiles/miniftpd.dir/tunable.c.o: CMakeFiles/miniftpd.dir/flags.make
CMakeFiles/miniftpd.dir/tunable.c.o: ../tunable.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/tmp.qQGCGXiSq4/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building C object CMakeFiles/miniftpd.dir/tunable.c.o"
	/usr/lib64/ccache/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/miniftpd.dir/tunable.c.o   -c /tmp/tmp.qQGCGXiSq4/tunable.c

CMakeFiles/miniftpd.dir/tunable.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/miniftpd.dir/tunable.c.i"
	/usr/lib64/ccache/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /tmp/tmp.qQGCGXiSq4/tunable.c > CMakeFiles/miniftpd.dir/tunable.c.i

CMakeFiles/miniftpd.dir/tunable.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/miniftpd.dir/tunable.c.s"
	/usr/lib64/ccache/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /tmp/tmp.qQGCGXiSq4/tunable.c -o CMakeFiles/miniftpd.dir/tunable.c.s

CMakeFiles/miniftpd.dir/parseconf.c.o: CMakeFiles/miniftpd.dir/flags.make
CMakeFiles/miniftpd.dir/parseconf.c.o: ../parseconf.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/tmp.qQGCGXiSq4/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building C object CMakeFiles/miniftpd.dir/parseconf.c.o"
	/usr/lib64/ccache/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/miniftpd.dir/parseconf.c.o   -c /tmp/tmp.qQGCGXiSq4/parseconf.c

CMakeFiles/miniftpd.dir/parseconf.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/miniftpd.dir/parseconf.c.i"
	/usr/lib64/ccache/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /tmp/tmp.qQGCGXiSq4/parseconf.c > CMakeFiles/miniftpd.dir/parseconf.c.i

CMakeFiles/miniftpd.dir/parseconf.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/miniftpd.dir/parseconf.c.s"
	/usr/lib64/ccache/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /tmp/tmp.qQGCGXiSq4/parseconf.c -o CMakeFiles/miniftpd.dir/parseconf.c.s

# Object files for target miniftpd
miniftpd_OBJECTS = \
"CMakeFiles/miniftpd.dir/main.c.o" \
"CMakeFiles/miniftpd.dir/sysutil.c.o" \
"CMakeFiles/miniftpd.dir/common.c.o" \
"CMakeFiles/miniftpd.dir/session.c.o" \
"CMakeFiles/miniftpd.dir/ftpproto.c.o" \
"CMakeFiles/miniftpd.dir/privparent.c.o" \
"CMakeFiles/miniftpd.dir/str.c.o" \
"CMakeFiles/miniftpd.dir/tunable.c.o" \
"CMakeFiles/miniftpd.dir/parseconf.c.o"

# External object files for target miniftpd
miniftpd_EXTERNAL_OBJECTS =

miniftpd: CMakeFiles/miniftpd.dir/main.c.o
miniftpd: CMakeFiles/miniftpd.dir/sysutil.c.o
miniftpd: CMakeFiles/miniftpd.dir/common.c.o
miniftpd: CMakeFiles/miniftpd.dir/session.c.o
miniftpd: CMakeFiles/miniftpd.dir/ftpproto.c.o
miniftpd: CMakeFiles/miniftpd.dir/privparent.c.o
miniftpd: CMakeFiles/miniftpd.dir/str.c.o
miniftpd: CMakeFiles/miniftpd.dir/tunable.c.o
miniftpd: CMakeFiles/miniftpd.dir/parseconf.c.o
miniftpd: CMakeFiles/miniftpd.dir/build.make
miniftpd: CMakeFiles/miniftpd.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/tmp/tmp.qQGCGXiSq4/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Linking C executable miniftpd"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/miniftpd.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/miniftpd.dir/build: miniftpd

.PHONY : CMakeFiles/miniftpd.dir/build

CMakeFiles/miniftpd.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/miniftpd.dir/cmake_clean.cmake
.PHONY : CMakeFiles/miniftpd.dir/clean

CMakeFiles/miniftpd.dir/depend:
	cd /tmp/tmp.qQGCGXiSq4/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /tmp/tmp.qQGCGXiSq4 /tmp/tmp.qQGCGXiSq4 /tmp/tmp.qQGCGXiSq4/cmake-build-debug /tmp/tmp.qQGCGXiSq4/cmake-build-debug /tmp/tmp.qQGCGXiSq4/cmake-build-debug/CMakeFiles/miniftpd.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/miniftpd.dir/depend

