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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /mnt/scratch/wenqi/grpc-ANNS/wenqi_cpp_projects/keyvaluestore_grpc_example

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /mnt/scratch/wenqi/grpc-ANNS/wenqi_cpp_projects/keyvaluestore_grpc_example/build

# Include any dependencies generated for this target.
include CMakeFiles/server.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/server.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/server.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/server.dir/flags.make

CMakeFiles/server.dir/server.cc.o: CMakeFiles/server.dir/flags.make
CMakeFiles/server.dir/server.cc.o: ../server.cc
CMakeFiles/server.dir/server.cc.o: CMakeFiles/server.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/scratch/wenqi/grpc-ANNS/wenqi_cpp_projects/keyvaluestore_grpc_example/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/server.dir/server.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/server.dir/server.cc.o -MF CMakeFiles/server.dir/server.cc.o.d -o CMakeFiles/server.dir/server.cc.o -c /mnt/scratch/wenqi/grpc-ANNS/wenqi_cpp_projects/keyvaluestore_grpc_example/server.cc

CMakeFiles/server.dir/server.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/server.dir/server.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/scratch/wenqi/grpc-ANNS/wenqi_cpp_projects/keyvaluestore_grpc_example/server.cc > CMakeFiles/server.dir/server.cc.i

CMakeFiles/server.dir/server.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/server.dir/server.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/scratch/wenqi/grpc-ANNS/wenqi_cpp_projects/keyvaluestore_grpc_example/server.cc -o CMakeFiles/server.dir/server.cc.s

# Object files for target server
server_OBJECTS = \
"CMakeFiles/server.dir/server.cc.o"

# External object files for target server
server_EXTERNAL_OBJECTS =

server: CMakeFiles/server.dir/server.cc.o
server: CMakeFiles/server.dir/build.make
server: libkvs_grpc_proto.a
server: /home/wejiang/.local/lib/libgrpc++_reflection.a
server: /home/wejiang/.local/lib/libgrpc++.a
server: /home/wejiang/.local/lib/libprotobuf.a
server: /home/wejiang/.local/lib/libgrpc.a
server: /home/wejiang/.local/lib/libz.a
server: /home/wejiang/.local/lib/libcares.a
server: /home/wejiang/.local/lib/libaddress_sorting.a
server: /home/wejiang/.local/lib/libre2.a
server: /home/wejiang/.local/lib/libupb.a
server: /home/wejiang/.local/lib/libabsl_raw_hash_set.a
server: /home/wejiang/.local/lib/libabsl_hashtablez_sampler.a
server: /home/wejiang/.local/lib/libabsl_hash.a
server: /home/wejiang/.local/lib/libabsl_city.a
server: /home/wejiang/.local/lib/libabsl_low_level_hash.a
server: /home/wejiang/.local/lib/libabsl_statusor.a
server: /home/wejiang/.local/lib/libgpr.a
server: /home/wejiang/.local/lib/libabsl_bad_variant_access.a
server: /home/wejiang/.local/lib/libabsl_status.a
server: /home/wejiang/.local/lib/libabsl_strerror.a
server: /home/wejiang/.local/lib/libabsl_random_distributions.a
server: /home/wejiang/.local/lib/libabsl_random_seed_sequences.a
server: /home/wejiang/.local/lib/libabsl_random_internal_pool_urbg.a
server: /home/wejiang/.local/lib/libabsl_random_internal_randen.a
server: /home/wejiang/.local/lib/libabsl_random_internal_randen_hwaes.a
server: /home/wejiang/.local/lib/libabsl_random_internal_randen_hwaes_impl.a
server: /home/wejiang/.local/lib/libabsl_random_internal_randen_slow.a
server: /home/wejiang/.local/lib/libabsl_random_internal_platform.a
server: /home/wejiang/.local/lib/libabsl_random_internal_seed_material.a
server: /home/wejiang/.local/lib/libabsl_random_seed_gen_exception.a
server: /home/wejiang/.local/lib/libabsl_cord.a
server: /home/wejiang/.local/lib/libabsl_bad_optional_access.a
server: /home/wejiang/.local/lib/libabsl_cordz_info.a
server: /home/wejiang/.local/lib/libabsl_cord_internal.a
server: /home/wejiang/.local/lib/libabsl_cordz_functions.a
server: /home/wejiang/.local/lib/libabsl_exponential_biased.a
server: /home/wejiang/.local/lib/libabsl_cordz_handle.a
server: /home/wejiang/.local/lib/libabsl_str_format_internal.a
server: /home/wejiang/.local/lib/libabsl_synchronization.a
server: /home/wejiang/.local/lib/libabsl_stacktrace.a
server: /home/wejiang/.local/lib/libabsl_symbolize.a
server: /home/wejiang/.local/lib/libabsl_debugging_internal.a
server: /home/wejiang/.local/lib/libabsl_demangle_internal.a
server: /home/wejiang/.local/lib/libabsl_graphcycles_internal.a
server: /home/wejiang/.local/lib/libabsl_malloc_internal.a
server: /home/wejiang/.local/lib/libabsl_time.a
server: /home/wejiang/.local/lib/libabsl_strings.a
server: /home/wejiang/.local/lib/libabsl_throw_delegate.a
server: /home/wejiang/.local/lib/libabsl_int128.a
server: /home/wejiang/.local/lib/libabsl_strings_internal.a
server: /home/wejiang/.local/lib/libabsl_base.a
server: /home/wejiang/.local/lib/libabsl_spinlock_wait.a
server: /home/wejiang/.local/lib/libabsl_raw_logging_internal.a
server: /home/wejiang/.local/lib/libabsl_log_severity.a
server: /home/wejiang/.local/lib/libabsl_civil_time.a
server: /home/wejiang/.local/lib/libabsl_time_zone.a
server: /home/wejiang/.local/lib/libssl.a
server: /home/wejiang/.local/lib/libcrypto.a
server: CMakeFiles/server.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/mnt/scratch/wenqi/grpc-ANNS/wenqi_cpp_projects/keyvaluestore_grpc_example/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable server"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/server.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/server.dir/build: server
.PHONY : CMakeFiles/server.dir/build

CMakeFiles/server.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/server.dir/cmake_clean.cmake
.PHONY : CMakeFiles/server.dir/clean

CMakeFiles/server.dir/depend:
	cd /mnt/scratch/wenqi/grpc-ANNS/wenqi_cpp_projects/keyvaluestore_grpc_example/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /mnt/scratch/wenqi/grpc-ANNS/wenqi_cpp_projects/keyvaluestore_grpc_example /mnt/scratch/wenqi/grpc-ANNS/wenqi_cpp_projects/keyvaluestore_grpc_example /mnt/scratch/wenqi/grpc-ANNS/wenqi_cpp_projects/keyvaluestore_grpc_example/build /mnt/scratch/wenqi/grpc-ANNS/wenqi_cpp_projects/keyvaluestore_grpc_example/build /mnt/scratch/wenqi/grpc-ANNS/wenqi_cpp_projects/keyvaluestore_grpc_example/build/CMakeFiles/server.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/server.dir/depend

