/*
 *
 * Copyright 2018 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <iostream>
#include <memory>
#include <string>
#include <vector>

// Wenqi: the grpc library
#include <grpcpp/grpcpp.h>

// Wenqi: generated during compilation
#include "keyvaluestore.grpc.pb.h"

// Wenqi: in grpcpp.h, use them directly in this file without the need to add namespace
using grpc::Server; // Server class in include/grcpp/server.h
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;
// Wenqi: in the generated keyvaluestore.grpc.pb.h, use them directly in this file without the need to add namespace 
using keyvaluestore::KeyValueStore; // Wenqi: the k-v class
using keyvaluestore::Request;       // Wenqi: the Request type based on protobuf
using keyvaluestore::Response;      // Wenqi: the Response type based on protobuf

struct kv_pair {
  const char* key;
  const char* value;
};

static const kv_pair kvs_map[] = {
    {"key1", "value1"}, {"key2", "value2"}, {"key3", "value3"},
    {"key4", "value4"}, {"key5", "value5"},
};

const char* get_value_from_map(const char* key) {
  for (size_t i = 0; i < sizeof(kvs_map) / sizeof(kv_pair); ++i) {
    if (strcmp(key, kvs_map[i].key) == 0) {
      return kvs_map[i].value;
    }
  }
  return "";
}

// Logic and data behind the server's behavior.
// Wenqi: inherited from KeyValueStore::Service, final = cannot be further derived from
class KeyValueStoreServiceImpl final : public KeyValueStore::Service {
  // Wenqi: the GetValues function defined in the .proto file
  //    override the virtual function defined in keyvaluestore::KeyValueStore::Service
  //    make sure the interface is the same as keyvaluestore.grpc.pb.h
  // Wenqi: the ServerReaderWriter is inherited from ServerReaderWriterInterface in grpcpp/impl/codegen/sync_stream.h
  //    ServerReaderWriterInterface is inherited from ServerStreamingInterface, WriterInterface, ReaderInterface
  //    it has both Read and Write functions
  //    bool Read(R* msg) return true if successful, false when there will be no more incoming messages, R is the read message type
  //    depends on whether you need write optinos, 
  //       bool Write(const W& msg), W is the write message type, true on success, \a false when the stream has been closed
  //       bool Write(const W& msg, grpc::WriteOptions options) 
  Status GetValues(ServerContext* context,
                   ServerReaderWriter<Response, Request>* stream) override {
    Request request;
    while (stream->Read(&request)) { // Wenqi: wait read, and if read successful, continue the loop
      Response response;
      // Wenqi: key() -> element defined in .proto, method generated in keyvaluestore.pb.h, essentially return the string
      //   c_str -> returns the pointer to the array
      //   get_value_from_map the function we defined, returns a pointer to a string
      //   void Response::set_value(ArgT0&& arg0, ArgT... args)
      //   && -> r-value reference, it doesn't have a memory address. E.g. the number 6, and character 'v' are both r-values
      //       https://stackoverflow.com/questions/4549151/c-double-address-operator
      response.set_value(get_value_from_map(request.key().c_str()));
      stream->Write(response);
    }
    return Status::OK;
  }
};

void RunServer() {
  std::string server_address("0.0.0.0:50051");
  KeyValueStoreServiceImpl service;

  // Wenqi: from grpc include
  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case, it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  // Wenqi: ServerBuilder in include/grpcpp/server_builder.h
  //  virtual std::unique_ptr<grpc::Server> BuildAndStart();
  std::unique_ptr<Server> server = builder.BuildAndStart(); 
  std::cout << "Server listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}

int main(int argc, char** argv) {
  RunServer();

  return 0;
}
