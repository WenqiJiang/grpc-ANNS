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

#include "caching_interceptor.h"

#include <grpcpp/grpcpp.h>

#include "keyvaluestore.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using keyvaluestore::KeyValueStore;
using keyvaluestore::Request;
using keyvaluestore::Response;

class KeyValueStoreClient {
 public:
  // Wenqi: stub_ is a unique_ptr defined in the private region
  //   initialize stub_ by KeyValueStore::NewStub(channel), which is defined in keyvaluestore.grpc.pb.h:
  //   static std::unique_ptr<Stub> NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options = ::grpc::StubOptions());
  KeyValueStoreClient(std::shared_ptr<Channel> channel)
      : stub_(KeyValueStore::NewStub(channel)) {}

  // Requests each key in the vector and displays the key and its corresponding
  // value as a pair
  void GetValues(const std::vector<std::string>& keys) {
    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context; // from grpc include
    auto stream = stub_->GetValues(&context); // GetValues is defined in .proto
    // Wenqi: use the reference, avoid making copying elements from keys, key is reference type 
    //   https://stackoverflow.com/questions/15176104/range-based-loop-get-item-by-value-or-reference-to-const
    for (const auto& key : keys) {  
      // Key we are sending to the server.
      Request request;
      request.set_key(key); // Wenqi: in keyvaluestore.pb.h, void Request::set_key(ArgT0&& arg0, ArgT... args)
      stream->Write(request);

      // Get the value for the sent key
      Response response;
      stream->Read(&response);
      std::cout << key << " : " << response.value() << "\n";
    }
    stream->WritesDone();
    Status status = stream->Finish();
    if (!status.ok()) {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      std::cout << "RPC failed";
    }
  }

 private:
  // Wenqi: defined in keyvaluestore.grpc.pb.h: 
  //   class Stub final : public StubInterface
  std::unique_ptr<KeyValueStore::Stub> stub_;
};

int main(int argc, char** argv) {
  // Instantiate the client. It requires a channel, out of which the actual RPCs
  // are created. This channel models a connection to an endpoint (in this case,
  // localhost at port 50051). We indicate that the channel isn't authenticated
  // (use of InsecureChannelCredentials()).
  // In this example, we are using a cache which has been added in as an
  // interceptor.
  grpc::ChannelArguments args;
  std::vector<
      std::unique_ptr<grpc::experimental::ClientInterceptorFactoryInterface>>
      interceptor_creators;
  interceptor_creators.push_back(std::unique_ptr<CachingInterceptorFactory>(
      new CachingInterceptorFactory()));
  auto channel = grpc::experimental::CreateCustomChannelWithInterceptors(
      "localhost:50051", grpc::InsecureChannelCredentials(), args,
      std::move(interceptor_creators));
  KeyValueStoreClient client(channel);
  std::vector<std::string> keys = {"key1", "key2", "key3", "key4",
                                   "key5", "key1", "key2", "key4"};
  client.GetValues(keys);

  return 0;
}
