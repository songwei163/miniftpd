# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.15

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\JetBrains\CLion 2019.2.5\bin\cmake\win\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\JetBrains\CLion 2019.2.5\bin\cmake\win\bin\cmake.exe" -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\Users\S\CLionProjects\miniftpd

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Users\S\CLionProjects\miniftpd\cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/miniftpd.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/miniftpd.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/miniftpd.dir/flags.make

CMakeFiles/miniftpd.dir/src/main.c.obj: CMakeFiles/miniftpd.dir/flags.make
CMakeFiles/miniftpd.dir/src/main.c.obj: ../src/main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\S\CLionProjects\miniftpd\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/miniftpd.dir/src/main.c.obj"
	C:\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\miniftpd.dir\src\main.c.obj   -c C:\Users\S\CLionProjects\miniftpd\src\main.c

CMakeFiles/miniftpd.dir/src/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/miniftpd.dir/src/main.c.i"
	C:\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E C:\Users\S\CLionProjects\miniftpd\src\main.c > CMakeFiles\miniftpd.dir\src\main.c.i

CMakeFiles/miniftpd.dir/src/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/miniftpd.dir/src/main.c.s"
	C:\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S C:\Users\S\CLionProjects\miniftpd\src\main.c -o CMakeFiles\miniftpd.dir\src\main.c.s

CMakeFiles/miniftpd.dir/src/sysutil.c.obj: CMakeFiles/miniftpd.dir/flags.make
CMakeFiles/miniftpd.dir/src/sysutil.c.obj: ../src/sysutil.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\S\CLionProjects\miniftpd\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/miniftpd.dir/src/sysutil.c.obj"
	C:\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\miniftpd.dir\src\sysutil.c.obj   -c C:\Users\S\CLionProjects\miniftpd\src\sysutil.c

CMakeFiles/miniftpd.dir/src/sysutil.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/miniftpd.dir/src/sysutil.c.i"
	C:\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E C:\Users\S\CLionProjects\miniftpd\src\sysutil.c > CMakeFiles\miniftpd.dir\src\sysutil.c.i

CMakeFiles/miniftpd.dir/src/sysutil.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/miniftpd.dir/src/sysutil.c.s"
	C:\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S C:\Users\S\CLionProjects\miniftpd\src\sysutil.c -o CMakeFiles\miniftpd.dir\src\sysutil.c.s

CMakeFiles/miniftpd.dir/src/session.c.obj: CMakeFiles/miniftpd.dir/flags.make
CMakeFiles/miniftpd.dir/src/session.c.obj: ../src/session.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\S\CLionProjects\miniftpd\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/miniftpd.dir/src/session.c.obj"
	C:\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\miniftpd.dir\src\session.c.obj   -c C:\Users\S\CLionProjects\miniftpd\src\session.c

CMakeFiles/miniftpd.dir/src/session.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/miniftpd.dir/src/session.c.i"
	C:\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E C:\Users\S\CLionProjects\miniftpd\src\session.c > CMakeFiles\miniftpd.dir\src\session.c.i

CMakeFiles/miniftpd.dir/src/session.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/miniftpd.dir/src/session.c.s"
	C:\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S C:\Users\S\CLionProjects\miniftpd\src\session.c -o CMakeFiles\miniftpd.dir\src\session.c.s

CMakeFiles/miniftpd.dir/src/ftpproto.c.obj: CMakeFiles/miniftpd.dir/flags.make
CMakeFiles/miniftpd.dir/src/ftpproto.c.obj: ../src/ftpproto.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\S\CLionProjects\miniftpd\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object CMakeFiles/miniftpd.dir/src/ftpproto.c.obj"
	C:\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\miniftpd.dir\src\ftpproto.c.obj   -c C:\Users\S\CLionProjects\miniftpd\src\ftpproto.c

CMakeFiles/miniftpd.dir/src/ftpproto.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/miniftpd.dir/src/ftpproto.c.i"
	C:\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E C:\Users\S\CLionProjects\miniftpd\src\ftpproto.c > CMakeFiles\miniftpd.dir\src\ftpproto.c.i

CMakeFiles/miniftpd.dir/src/ftpproto.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/miniftpd.dir/src/ftpproto.c.s"
	C:\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S C:\Users\S\CLionProjects\miniftpd\src\ftpproto.c -o CMakeFiles\miniftpd.dir\src\ftpproto.c.s

CMakeFiles/miniftpd.dir/src/privparent.c.obj: CMakeFiles/miniftpd.dir/flags.make
CMakeFiles/miniftpd.dir/src/privparent.c.obj: ../src/privparent.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\S\CLionProjects\miniftpd\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object CMakeFiles/miniftpd.dir/src/privparent.c.obj"
	C:\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\miniftpd.dir\src\privparent.c.obj   -c C:\Users\S\CLionProjects\miniftpd\src\privparent.c

CMakeFiles/miniftpd.dir/src/privparent.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/miniftpd.dir/src/privparent.c.i"
	C:\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E C:\Users\S\CLionProjects\miniftpd\src\privparent.c > CMakeFiles\miniftpd.dir\src\privparent.c.i

CMakeFiles/miniftpd.dir/src/privparent.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/miniftpd.dir/src/privparent.c.s"
	C:\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S C:\Users\S\CLionProjects\miniftpd\src\privparent.c -o CMakeFiles\miniftpd.dir\src\privparent.c.s

CMakeFiles/miniftpd.dir/src/str.c.obj: CMakeFiles/miniftpd.dir/flags.make
CMakeFiles/miniftpd.dir/src/str.c.obj: ../src/str.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\S\CLionProjects\miniftpd\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building C object CMakeFiles/miniftpd.dir/src/str.c.obj"
	C:\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\miniftpd.dir\src\str.c.obj   -c C:\Users\S\CLionProjects\miniftpd\src\str.c

CMakeFiles/miniftpd.dir/src/str.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/miniftpd.dir/src/str.c.i"
	C:\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E C:\Users\S\CLionProjects\miniftpd\src\str.c > CMakeFiles\miniftpd.dir\src\str.c.i

CMakeFiles/miniftpd.dir/src/str.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/miniftpd.dir/src/str.c.s"
	C:\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S C:\Users\S\CLionProjects\miniftpd\src\str.c -o CMakeFiles\miniftpd.dir\src\str.c.s

CMakeFiles/miniftpd.dir/src/tunable.c.obj: CMakeFiles/miniftpd.dir/flags.make
CMakeFiles/miniftpd.dir/src/tunable.c.obj: ../src/tunable.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\S\CLionProjects\miniftpd\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building C object CMakeFiles/miniftpd.dir/src/tunable.c.obj"
	C:\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\miniftpd.dir\src\tunable.c.obj   -c C:\Users\S\CLionProjects\miniftpd\src\tunable.c

CMakeFiles/miniftpd.dir/src/tunable.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/miniftpd.dir/src/tunable.c.i"
	C:\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E C:\Users\S\CLionProjects\miniftpd\src\tunable.c > CMakeFiles\miniftpd.dir\src\tunable.c.i

CMakeFiles/miniftpd.dir/src/tunable.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/miniftpd.dir/src/tunable.c.s"
	C:\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S C:\Users\S\CLionProjects\miniftpd\src\tunable.c -o CMakeFiles\miniftpd.dir\src\tunable.c.s

CMakeFiles/miniftpd.dir/src/parseconf.c.obj: CMakeFiles/miniftpd.dir/flags.make
CMakeFiles/miniftpd.dir/src/parseconf.c.obj: ../src/parseconf.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\S\CLionProjects\miniftpd\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building C object CMakeFiles/miniftpd.dir/src/parseconf.c.obj"
	C:\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\miniftpd.dir\src\parseconf.c.obj   -c C:\Users\S\CLionProjects\miniftpd\src\parseconf.c

CMakeFiles/miniftpd.dir/src/parseconf.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/miniftpd.dir/src/parseconf.c.i"
	C:\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E C:\Users\S\CLionProjects\miniftpd\src\parseconf.c > CMakeFiles\miniftpd.dir\src\parseconf.c.i

CMakeFiles/miniftpd.dir/src/parseconf.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/miniftpd.dir/src/parseconf.c.s"
	C:\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S C:\Users\S\CLionProjects\miniftpd\src\parseconf.c -o CMakeFiles\miniftpd.dir\src\parseconf.c.s

CMakeFiles/miniftpd.dir/src/privsock.c.obj: CMakeFiles/miniftpd.dir/flags.make
CMakeFiles/miniftpd.dir/src/privsock.c.obj: ../src/privsock.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\S\CLionProjects\miniftpd\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building C object CMakeFiles/miniftpd.dir/src/privsock.c.obj"
	C:\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\miniftpd.dir\src\privsock.c.obj   -c C:\Users\S\CLionProjects\miniftpd\src\privsock.c

CMakeFiles/miniftpd.dir/src/privsock.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/miniftpd.dir/src/privsock.c.i"
	C:\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E C:\Users\S\CLionProjects\miniftpd\src\privsock.c > CMakeFiles\miniftpd.dir\src\privsock.c.i

CMakeFiles/miniftpd.dir/src/privsock.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/miniftpd.dir/src/privsock.c.s"
	C:\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S C:\Users\S\CLionProjects\miniftpd\src\privsock.c -o CMakeFiles\miniftpd.dir\src\privsock.c.s

CMakeFiles/miniftpd.dir/src/hash.c.obj: CMakeFiles/miniftpd.dir/flags.make
CMakeFiles/miniftpd.dir/src/hash.c.obj: ../src/hash.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\S\CLionProjects\miniftpd\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building C object CMakeFiles/miniftpd.dir/src/hash.c.obj"
	C:\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\miniftpd.dir\src\hash.c.obj   -c C:\Users\S\CLionProjects\miniftpd\src\hash.c

CMakeFiles/miniftpd.dir/src/hash.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/miniftpd.dir/src/hash.c.i"
	C:\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E C:\Users\S\CLionProjects\miniftpd\src\hash.c > CMakeFiles\miniftpd.dir\src\hash.c.i

CMakeFiles/miniftpd.dir/src/hash.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/miniftpd.dir/src/hash.c.s"
	C:\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S C:\Users\S\CLionProjects\miniftpd\src\hash.c -o CMakeFiles\miniftpd.dir\src\hash.c.s

# Object files for target miniftpd
miniftpd_OBJECTS = \
"CMakeFiles/miniftpd.dir/src/main.c.obj" \
"CMakeFiles/miniftpd.dir/src/sysutil.c.obj" \
"CMakeFiles/miniftpd.dir/src/session.c.obj" \
"CMakeFiles/miniftpd.dir/src/ftpproto.c.obj" \
"CMakeFiles/miniftpd.dir/src/privparent.c.obj" \
"CMakeFiles/miniftpd.dir/src/str.c.obj" \
"CMakeFiles/miniftpd.dir/src/tunable.c.obj" \
"CMakeFiles/miniftpd.dir/src/parseconf.c.obj" \
"CMakeFiles/miniftpd.dir/src/privsock.c.obj" \
"CMakeFiles/miniftpd.dir/src/hash.c.obj"

# External object files for target miniftpd
miniftpd_EXTERNAL_OBJECTS =

miniftpd.exe: CMakeFiles/miniftpd.dir/src/main.c.obj
miniftpd.exe: CMakeFiles/miniftpd.dir/src/sysutil.c.obj
miniftpd.exe: CMakeFiles/miniftpd.dir/src/session.c.obj
miniftpd.exe: CMakeFiles/miniftpd.dir/src/ftpproto.c.obj
miniftpd.exe: CMakeFiles/miniftpd.dir/src/privparent.c.obj
miniftpd.exe: CMakeFiles/miniftpd.dir/src/str.c.obj
miniftpd.exe: CMakeFiles/miniftpd.dir/src/tunable.c.obj
miniftpd.exe: CMakeFiles/miniftpd.dir/src/parseconf.c.obj
miniftpd.exe: CMakeFiles/miniftpd.dir/src/privsock.c.obj
miniftpd.exe: CMakeFiles/miniftpd.dir/src/hash.c.obj
miniftpd.exe: CMakeFiles/miniftpd.dir/build.make
miniftpd.exe: CMakeFiles/miniftpd.dir/linklibs.rsp
miniftpd.exe: CMakeFiles/miniftpd.dir/objects1.rsp
miniftpd.exe: CMakeFiles/miniftpd.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\Users\S\CLionProjects\miniftpd\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Linking C executable miniftpd.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\miniftpd.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/miniftpd.dir/build: miniftpd.exe

.PHONY : CMakeFiles/miniftpd.dir/build

CMakeFiles/miniftpd.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\miniftpd.dir\cmake_clean.cmake
.PHONY : CMakeFiles/miniftpd.dir/clean

CMakeFiles/miniftpd.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Users\S\CLionProjects\miniftpd C:\Users\S\CLionProjects\miniftpd C:\Users\S\CLionProjects\miniftpd\cmake-build-debug C:\Users\S\CLionProjects\miniftpd\cmake-build-debug C:\Users\S\CLionProjects\miniftpd\cmake-build-debug\CMakeFiles\miniftpd.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/miniftpd.dir/depend

