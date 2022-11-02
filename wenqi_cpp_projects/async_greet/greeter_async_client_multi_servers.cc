/*
Usage:

./greeter_async_client_multi_servers <optional server_IP 0:server_port 0> <optional server_IP 1:server_port 1> ...

e.g.,: 

./greeter_async_client_multi_servers 127.0.0.1:8888 127.0.0.1:8889 

 */

#include <chrono>
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
        for (int i = 0; i < num_servers; i++) {
          stubs_[i] = Greeter::NewStub(channels[i]);
        }
      }

  // Assembles the client's payload, sends it and presents the response back
  // from the server.
  std::unique_ptr<std::string[]>  SayHello(const std::string& user, int n_iter=1000) {

    // The producer-consumer queue we use to communicate asynchronously with the
    // gRPC runtime.
    CompletionQueue cq;

    std::unique_ptr<std::string[]> return_msgs(new std::string[num_servers]);

    double profile[5];
    auto start = std::chrono::high_resolution_clock::now();

    for (int iter = 0; iter < n_iter; iter++) {

      auto t0 = std::chrono::high_resolution_clock::now();
      std::shared_ptr<AsyncClientCall> calls[num_servers]; 

      for (int i = 0; i < num_servers; i++) {
        calls[i] = std::shared_ptr<AsyncClientCall>(new AsyncClientCall);
      }
      auto t1 = std::chrono::high_resolution_clock::now();
      profile[0] += std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();

      t0 = std::chrono::high_resolution_clock::now();
      for (int i = 0; i < num_servers; i++) {
        // *calls[i] = AsyncClientCall() {};

        // calls[i] = {};
        // memset((void*) calls[i].get(), 0, sizeof(AsyncClientCall));
        // memset(&calls[i]->context, 0, sizeof(ClientContext));
        // memset(&calls[i]->status, 0, sizeof(Status));
    // ClientContext context;

    // // Storage for the status of the RPC upon completion.
    // Status status;
    // calls[i]->context = ClientContext();
    // calls[i]->status = Status();
        

        calls[i]->request.set_name(std::string(user) + std::to_string(i));
        calls[i]->call_id = i;

        // stub_->PrepareAsyncSayHello() creates an RPC object, returning
        // an instance to store in "call" but does not actually start the RPC
        // Because we are using the asynchronous API, we need to hold on to
        // the "call" instance in order to get updates on the ongoing RPC.
        calls[i]->response_reader = stubs_[i]->PrepareAsyncSayHello(
          &calls[i]->context, calls[i]->request, &cq);
      }
      t1 = std::chrono::high_resolution_clock::now();
      profile[1] += std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();

      t0 = std::chrono::high_resolution_clock::now();
      // StartCall initiates the RPC call
      for (int i = 0; i < num_servers; i++) {
        calls[i]->response_reader->StartCall();
      }
      t1 = std::chrono::high_resolution_clock::now();
      profile[2] += std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();

      t0 = std::chrono::high_resolution_clock::now();
      // Request that, upon completion of the RPC, "reply" be updated with the
      // server's response; "status" with the indication of whether the operation
      // was successful. Tag the request with server ID
      for (int i = 0; i < num_servers; i++) {
        calls[i]->response_reader->Finish(&calls[i]->reply, &calls[i]->status, (void*) (calls[i].get()));
      }
      t1 = std::chrono::high_resolution_clock::now();
      profile[3] += std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();

      void* got_tag;
      bool ok = false;

      t0 = std::chrono::high_resolution_clock::now();
      for (int i = 0; i < num_servers; i++) { 
        // Block until the next result is available in the completion queue "cq".
        // The return value of Next should always be checked. This return value
        // tells us whether there is any kind of event or the cq_ is shutting down.
        // the got_tag is the ID of the RPC call
        GPR_ASSERT(cq.Next(&got_tag, &ok));

        // assert that the request was completed successfully. Note that "ok"
        // corresponds solely to the request for updates introduced by Finish().
        GPR_ASSERT(ok);
        AsyncClientCall* call = static_cast<AsyncClientCall*>(got_tag);
        int call_id = calls[i]->call_id;

        if (call->status.ok()) {
          return_msgs[call_id] = call->reply.message();
        } else {
          return_msgs[call_id] = "RPC failed";
        }
        // std::cout << call->reply.message() << std::endl;
      }
      t1 = std::chrono::high_resolution_clock::now();
      profile[4] += std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
    }
    
    auto finish = std::chrono::high_resolution_clock::now();
    std::cout << "SayHello on " << num_servers << "servers for " << n_iter << " iterations took "
              << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count()
              << " microseconds" << std::endl;

    std::cout << "Time (microseconds) on part 0: " << profile[0] << std::endl <<
      "Time (microseconds) on part 1: " << profile[1] << std::endl <<
      "Time (microseconds) on part 2: " << profile[2] << std::endl <<
      "Time (microseconds) on part 3: " << profile[3] << std::endl << 
      "Time (microseconds) on part 4: " << profile[4] << std::endl;

    return return_msgs;
  }

 private:
  // Out of the passed in Channel comes the stub, stored here, our view of the
  // server's exposed services.
  int num_servers;
  // Wenqi: cannot use stubs_[num_servers] as sizeof(classname) must be a constant: 
  // https://stackoverflow.com/questions/26198052/how-do-i-initialize-a-variable-size-array-in-a-c-class
  std::unique_ptr<std::unique_ptr<Greeter::Stub>[]> stubs_;

  // struct for keeping state and data information for a single RPC call
  struct AsyncClientCall {
    // Data we are sending to the server.
    HelloRequest request;

    // Container for the data we expect from the server.
    HelloReply reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // Storage for the status of the RPC upon completion.
    Status status;

    // record the ID for the RPC call, which can be used to track, e.g.,
    //   which server the call is asking for
    int call_id;

    // stub_->PrepareAsyncSayHello() creates an RPC object, returning
    // an instance to store in "call" but does not actually start the RPC
    // Because we are using the asynchronous API, we need to hold on to
    // the "call" instance in order to get updates on the ongoing RPC.
    std::unique_ptr<ClientAsyncResponseReader<HelloReply>> response_reader;
  };
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
