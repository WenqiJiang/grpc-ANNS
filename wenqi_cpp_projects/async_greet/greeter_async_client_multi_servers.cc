/*
Usage:

./greeter_async_client_multi_servers <optional server_IP 0:server_port 0> <optional server_IP 1:server_port 1> ...

e.g.,: 

./greeter_async_client_multi_servers 127.0.0.1:8888 127.0.0.1:8889 

 */

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <grpc/support/log.h>
#include <grpcpp/grpcpp.h>

#ifdef BAZEL_BUILD
#include "examples/protos/helloworld.grpc.pb.h"
#else
#include "helloworld.grpc.pb.h"
#endif

using grpc::Channel;
using grpc::ClientAsyncResponseReader;
using grpc::ClientContext;
using grpc::CompletionQueue;
using grpc::Status;
using helloworld::Greeter;
using helloworld::HelloReply;
using helloworld::HelloRequest;

class GreeterClient {
 public:
  explicit GreeterClient(
      int num_servers, std::shared_ptr<Channel> channels[]) :
        num_servers(num_servers) {
        stubs_ = std::unique_ptr<std::unique_ptr<Greeter::Stub>[]>(
          new std::unique_ptr<Greeter::Stub>[num_servers]);
        // stubs_ = std::unique_ptr<Greeter::Stub[]>(new Greeter::Stub[num_servers]);
        for (int i = 0; i < num_servers; i++) {
          stubs_[i] = Greeter::NewStub(channels[i]);
        }
        // stub_ = Greeter::NewStub(channels[0]);
        // array = std::unique_ptr<int[]>(new int[num_servers]);
        // for (int i = 0; i < num_servers; i++) {
        //   array[i] = i;
        // }
      }

  // Assembles the client's payload, sends it and presents the response back
  // from the server.
  std::unique_ptr<std::string[]>  SayHello(const std::string& user) {
    // Data we are sending to the server.
    HelloRequest request[num_servers];

    // Container for the data we expect from the server.
    HelloReply reply[num_servers];

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context[num_servers];

    // The producer-consumer queue we use to communicate asynchronously with the
    // gRPC runtime.
    CompletionQueue cq[num_servers];

    // Storage for the status of the RPC upon completion.
    Status status[num_servers];


    // stub_->PrepareAsyncSayHello() creates an RPC object, returning
    // an instance to store in "call" but does not actually start the RPC
    // Because we are using the asynchronous API, we need to hold on to
    // the "call" instance in order to get updates on the ongoing RPC.
    std::unique_ptr<ClientAsyncResponseReader<HelloReply> > rpc[num_servers];

    for (int i = 0; i < num_servers; i++) {
      request[i].set_name(user);

      // stub_->PrepareAsyncSayHello() creates an RPC object, returning
      // an instance to store in "call" but does not actually start the RPC
      // Because we are using the asynchronous API, we need to hold on to
      // the "call" instance in order to get updates on the ongoing RPC.
      rpc[i] = stubs_[i]->PrepareAsyncSayHello(&context[i], request[i], &cq[i]);
    }

    // StartCall initiates the RPC call
    for (int i = 0; i < num_servers; i++) {
      rpc[i]->StartCall();
    }

    // Request that, upon completion of the RPC, "reply" be updated with the
    // server's response; "status" with the indication of whether the operation
    // was successful. Tag the request with the integer 1.
    for (int i = 0; i < num_servers; i++) {
      rpc[i]->Finish(&reply[i], &status[i], (void*)1);
    }
    void* got_tag[num_servers];
    bool ok[num_servers] = {false};
   
    for (int i = 0; i < num_servers; i++) { 
      // Block until the next result is available in the completion queue "cq".
      // The return value of Next should always be checked. This return value
      // tells us whether there is any kind of event or the cq_ is shutting down.
      GPR_ASSERT(cq[i].Next(&got_tag[i], &ok[i]));

      // Verify that the result from "cq" corresponds, by its tag, our previous
      // request.
      GPR_ASSERT(got_tag[i] == (void*)1);
      // ... and that the request was completed successfully. Note that "ok"
      // corresponds solely to the request for updates introduced by Finish().
      GPR_ASSERT(ok[i]);
    }

    // Act upon the status of the actual RPC.
    std::unique_ptr<std::string[]> return_msgs(new std::string[num_servers]);
    for (int i = 0; i < num_servers; i++) { 
      if (status[i].ok()) {
        return_msgs[i] = reply[i].message();
      } else {
        return_msgs[i] = "RPC failed";
      }
    }
    return return_msgs;
  }

 private:
  // Out of the passed in Channel comes the stub, stored here, our view of the
  // server's exposed services.
  int num_servers;
  // Wenqi: cannot use stubs_[num_servers] as sizeof(classname) must be a constant: 
  // https://stackoverflow.com/questions/26198052/how-do-i-initialize-a-variable-size-array-in-a-c-class
  // std::unique_ptr<Greeter::Stub[]> stubs_;
  std::unique_ptr<std::unique_ptr<Greeter::Stub>[]> stubs_;
  // std::unique_ptr<Greeter::Stub> stub_;
  // std::unique_ptr<int[]> array;
  // std::unique_ptr<int[]> array(new int[5]);
};

int main(int argc, char** argv) {
  // Instantiate the client. It requires a channel, out of which the actual RPCs
  // are created. This channel models a connection to an endpoint (in this case,
  // localhost at port 50051). We indicate that the channel isn't authenticated
  // (use of InsecureChannelCredentials()).

  int num_servers = argc > 1? argc - 1 : 1;
  std::cout << "Number of servers: " << num_servers << std::endl;
  std::string IP_port[num_servers];
  IP_port[0] = "0.0.0.0:50051";
  for (int i = 0 ; i < argc - 1; i++) {
    IP_port[i] = argv[i + 1];
    std::cout << "arg server IP and port: " << IP_port[i] << std::endl;
  }

  std::shared_ptr<Channel> channels[num_servers];
  for (int i = 0; i < num_servers; i++) {
    channels[i] = grpc::CreateChannel(
      IP_port[i], grpc::InsecureChannelCredentials());
  }
  GreeterClient greeter(num_servers, channels);
  std::string user("world");
  std::unique_ptr<std::string[]> return_msgs = greeter.SayHello(user);  // The actual RPC call!
  for (int i = 0; i < num_servers; i++) {
    std::cout << "Greeter received: " << return_msgs[i] << std::endl;
  }

  return 0;
}
