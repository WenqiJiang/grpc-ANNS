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

project(ANN C CXX)

include(../cmake/common.cmake)

# Proto file
get_filename_component(ANN_proto "../protos/ANN.proto" ABSOLUTE)
get_filename_component(ANN_proto_path "${ANN_proto}" PATH)

# Generated sources
set(ANN_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/ANN.pb.cc")
set(ANN_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/ANN.pb.h")
set(ANN_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/ANN.grpc.pb.cc")
set(ANN_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/ANN.grpc.pb.h")
add_custom_command(
      OUTPUT "${ANN_proto_srcs}" "${ANN_proto_hdrs}" "${ANN_grpc_srcs}" "${ANN_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${ANN_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${ANN_proto}"
      DEPENDS "${ANN_proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# ANN_grpc_proto
add_library(ANN_grpc_proto
  ${ANN_grpc_srcs}
  ${ANN_grpc_hdrs}
  ${ANN_proto_srcs}
  ${ANN_proto_hdrs})
target_link_libraries(ANN_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})

# client
add_executable(async_master_servers_multiple_connections "async_master_servers_multiple_connections.cc")
target_link_libraries(async_master_servers_multiple_connections
  ANN_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})

add_executable(async_master_server_many_requests "async_master_server_many_requests.cc")
target_link_libraries(async_master_server_many_requests
  ANN_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})

# server
add_executable(async_storage_server "async_storage_server.cc")
target_link_libraries(async_storage_server
  ANN_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})
