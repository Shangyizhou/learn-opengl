# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.30

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\CMake\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\CMake\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = D:\shangyizhou\code\opengl\learn_opengl

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = D:\shangyizhou\code\opengl\learn_opengl\build

# Include any dependencies generated for this target.
include src/01/CMakeFiles/01.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include src/01/CMakeFiles/01.dir/compiler_depend.make

# Include the progress variables for this target.
include src/01/CMakeFiles/01.dir/progress.make

# Include the compile flags for this target's objects.
include src/01/CMakeFiles/01.dir/flags.make

src/01/CMakeFiles/01.dir/main.cc.obj: src/01/CMakeFiles/01.dir/flags.make
src/01/CMakeFiles/01.dir/main.cc.obj: src/01/CMakeFiles/01.dir/includes_CXX.rsp
src/01/CMakeFiles/01.dir/main.cc.obj: D:/shangyizhou/code/opengl/learn_opengl/src/01/main.cc
src/01/CMakeFiles/01.dir/main.cc.obj: src/01/CMakeFiles/01.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=D:\shangyizhou\code\opengl\learn_opengl\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/01/CMakeFiles/01.dir/main.cc.obj"
	cd /d D:\shangyizhou\code\opengl\learn_opengl\build\src\01 && C:\MinGW\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT src/01/CMakeFiles/01.dir/main.cc.obj -MF CMakeFiles\01.dir\main.cc.obj.d -o CMakeFiles\01.dir\main.cc.obj -c D:\shangyizhou\code\opengl\learn_opengl\src\01\main.cc

src/01/CMakeFiles/01.dir/main.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/01.dir/main.cc.i"
	cd /d D:\shangyizhou\code\opengl\learn_opengl\build\src\01 && C:\MinGW\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\shangyizhou\code\opengl\learn_opengl\src\01\main.cc > CMakeFiles\01.dir\main.cc.i

src/01/CMakeFiles/01.dir/main.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/01.dir/main.cc.s"
	cd /d D:\shangyizhou\code\opengl\learn_opengl\build\src\01 && C:\MinGW\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S D:\shangyizhou\code\opengl\learn_opengl\src\01\main.cc -o CMakeFiles\01.dir\main.cc.s

# Object files for target 01
01_OBJECTS = \
"CMakeFiles/01.dir/main.cc.obj"

# External object files for target 01
01_EXTERNAL_OBJECTS =

D:/shangyizhou/code/opengl/learn_opengl/bin/01.exe: src/01/CMakeFiles/01.dir/main.cc.obj
D:/shangyizhou/code/opengl/learn_opengl/bin/01.exe: src/01/CMakeFiles/01.dir/build.make
D:/shangyizhou/code/opengl/learn_opengl/bin/01.exe: D:/shangyizhou/code/opengl/learn_opengl/lib/libglfw3.a
D:/shangyizhou/code/opengl/learn_opengl/bin/01.exe: D:/shangyizhou/code/opengl/learn_opengl/lib/libglad.a
D:/shangyizhou/code/opengl/learn_opengl/bin/01.exe: src/01/CMakeFiles/01.dir/linkLibs.rsp
D:/shangyizhou/code/opengl/learn_opengl/bin/01.exe: src/01/CMakeFiles/01.dir/objects1.rsp
D:/shangyizhou/code/opengl/learn_opengl/bin/01.exe: src/01/CMakeFiles/01.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=D:\shangyizhou\code\opengl\learn_opengl\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable D:\shangyizhou\code\opengl\learn_opengl\bin\01.exe"
	cd /d D:\shangyizhou\code\opengl\learn_opengl\build\src\01 && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\01.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/01/CMakeFiles/01.dir/build: D:/shangyizhou/code/opengl/learn_opengl/bin/01.exe
.PHONY : src/01/CMakeFiles/01.dir/build

src/01/CMakeFiles/01.dir/clean:
	cd /d D:\shangyizhou\code\opengl\learn_opengl\build\src\01 && $(CMAKE_COMMAND) -P CMakeFiles\01.dir\cmake_clean.cmake
.PHONY : src/01/CMakeFiles/01.dir/clean

src/01/CMakeFiles/01.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" D:\shangyizhou\code\opengl\learn_opengl D:\shangyizhou\code\opengl\learn_opengl\src\01 D:\shangyizhou\code\opengl\learn_opengl\build D:\shangyizhou\code\opengl\learn_opengl\build\src\01 D:\shangyizhou\code\opengl\learn_opengl\build\src\01\CMakeFiles\01.dir\DependInfo.cmake "--color=$(COLOR)"
.PHONY : src/01/CMakeFiles/01.dir/depend

