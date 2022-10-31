# Copyright 2021 the gRPC authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# cmake build file for C++ helloworld example.
# Assumes protobuf and gRPC have been installed using cmake.
# See cmake_externalproject/CMakeLists.txt for all-in-one cmake build
# that automatically builds all the dependencies before building helloworld.

cmake_minimum_required(VERSION 3.5.1)

project(AsyncGreet C CXX)

include(../cmake/common.cmake)

# Proto file
get_filename_component(helloworld_proto "../protos/helloworld.proto" ABSOLUTE)
get_filename_component(helloworld_proto_path "${helloworld_proto}" PATH)

# Generated sources
set(helloworld_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/helloworld.pb.cc")
set(helloworld_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/helloworld.pb.h")
set(helloworld_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/helloworld.grpc.pb.cc")
set(helloworld_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/helloworld.grpc.pb.h")
add_custom_command(
      OUTPUT "${helloworld_proto_srcs}" "${helloworld_proto_hdrs}" "${helloworld_grpc_srcs}" "${helloworld_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${helloworld_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${helloworld_proto}"
      DEPENDS "${helloworld_proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# helloworld_grpc_proto
add_library(helloworld_grpc_proto
  ${helloworld_grpc_srcs}
  ${helloworld_grpc_hdrs}
  ${helloworld_proto_srcs}
  ${helloworld_proto_hdrs})
target_link_libraries(helloworld_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})

# client
add_executable(greeter_async_client "greeter_async_client.cc")
target_link_libraries(greeter_async_client
  helloworld_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})

add_executable(greeter_async_client_many_requests "greeter_async_client_many_requests.cc")
target_link_libraries(greeter_async_client_many_requests
  helloworld_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})

add_executable(greeter_async_client_multi_servers "greeter_async_client_multi_servers.cc")
target_link_libraries(greeter_async_client_multi_servers
  helloworld_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})

# server
add_executable(greeter_async_server "greeter_async_server.cc")
target_link_libraries(greeter_async_server
  helloworld_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})
