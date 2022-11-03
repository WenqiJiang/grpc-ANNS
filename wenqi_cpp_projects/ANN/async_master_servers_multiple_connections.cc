/*
Usage:

./async_master_servers_multiple_connections <optional server_IP 0:server_port 0> <optional server_IP 1:server_port 1> ...

e.g.,: 

./async_master_servers_multiple_connections 127.0.0.1:8888 127.0.0.1:8889 

 */

#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include <grpc/support/log.h>
#include <grpcpp/grpcpp.h>

#ifdef BAZEL_BUILD
#include "examples/protos/ANN.grpc.pb.h"
#else
#include "ANN.grpc.pb.h"
#endif

using grpc::Channel;
using grpc::ClientAsyncResponseReader;
using grpc::ClientContext;
using grpc::CompletionQueue;
using grpc::Status;
using ANN::ANNService;
using ANN::SearchRequest;
using ANN::SearchResponse;

class SearchClient {
 public:

  explicit SearchClient(
      int num_servers, std::string IP_port[]) :
      // int num_servers, std::shared_ptr<Channel> channels[]) :
        num_servers(num_servers) {

        // build channels 
        std::shared_ptr<Channel> channels[num_servers];
        for (int i = 0; i < num_servers; i++) {
          channels[i] = grpc::CreateChannel(
            IP_port[i], grpc::InsecureChannelCredentials());
        }

        // build stubs
        stubs_ = std::unique_ptr<std::unique_ptr<ANNService::Stub>[]>(
          new std::unique_ptr<ANNService::Stub>[num_servers]);
        for (int i = 0; i < num_servers; i++) {
          stubs_[i] = ANNService::NewStub(channels[i]);
        }
      }

  void StartSearch() {
    start_ = true; 
  }

  // Assembles the client's payload, sends it and presents the response back
  // from the server.
  void Search(const std::string& user, int n_iter=1000) {

    while (!start_) {

    }

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
        
        int nprobe = 2;
        int ivf_list_len = 2;
        long ivf_list_ids[ivf_list_len] = {100, 101};
        int D = 2;
        float xq[D] = {1.2, 3.4};
        calls[i]->request.set_nq(int(1));
        calls[i]->request.set_topk(int(100));
        calls[i]->request.set_nprobe(nprobe);
        for (long id : ivf_list_ids) {
          calls[i]->request.add_ivf_list_ids(id);
        }
        for (float element : xq) {
          calls[i]->request.add_xq(element);
        }

        calls[i]->call_id = i;

        // stub_->PrepareAsyncSearch() creates an RPC object, returning
        // an instance to store in "call" but does not actually start the RPC
        // Because we are using the asynchronous API, we need to hold on to
        // the "call" instance in order to get updates on the ongoing RPC.
        calls[i]->response_reader = stubs_[i]->PrepareAsyncSearch(
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
          std::cout << "Received results" << std::endl;
        } else {
          std::cout << "RPC failed" << std::endl;
        }
        // std::cout << call->reply.message() << std::endl;
      }
      t1 = std::chrono::high_resolution_clock::now();
      profile[4] += std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
    }
    
    auto finish = std::chrono::high_resolution_clock::now();
    std::cout << "Search on " << num_servers << "servers for " << n_iter << " iterations took "
              << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count()
              << " microseconds" << std::endl;

    std::cout << "Time (microseconds) on part 0: " << profile[0] << std::endl <<
      "Time (microseconds) on part 1: " << profile[1] << std::endl <<
      "Time (microseconds) on part 2: " << profile[2] << std::endl <<
      "Time (microseconds) on part 3: " << profile[3] << std::endl << 
      "Time (microseconds) on part 4: " << profile[4] << std::endl;

  }

 private:
  // Out of the passed in Channel comes the stub, stored here, our view of the
  // server's exposed services.
  int num_servers;
  // Wenqi: cannot use stubs_[num_servers] as sizeof(classname) must be a constant: 
  // https://stackoverflow.com/questions/26198052/how-do-i-initialize-a-variable-size-array-in-a-c-class
  std::unique_ptr<std::unique_ptr<ANNService::Stub>[]> stubs_;
  bool start_;

  // struct for keeping state and data information for a single RPC call
  struct AsyncClientCall {
    // Data we are sending to the server.
    SearchRequest request;

    // Container for the data we expect from the server.
    SearchResponse reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // Storage for the status of the RPC upon completion.
    Status status;

    // record the ID for the RPC call, which can be used to track, e.g.,
    //   which server the call is asking for
    int call_id;

    // stub_->PrepareAsyncSearch() creates an RPC object, returning
    // an instance to store in "call" but does not actually start the RPC
    // Because we are using the asynchronous API, we need to hold on to
    // the "call" instance in order to get updates on the ongoing RPC.
    std::unique_ptr<ClientAsyncResponseReader<SearchResponse>> response_reader;
  };
};

int main(int argc, char** argv) {
  // Instantiate the client. It requires a channel, out of which the actual RPCs
  // are created. This channel models a connection to an endpoint (in this case,
  // localhost at port 50051). We indicate that the channel isn't authenticated
  // (use of InsecureChannelCredentials()).

  int num_servers = argc > 1? argc - 1 : 1;
  std::cout << "Number of servers: " << num_servers << std::endl;

  std::cout << "Type in number of clients: " << std::endl;
  std::string input;
  getline(std::cin, input);
  int num_clients = std::stol(input);


  std::string IP_port[num_servers];
  IP_port[0] = "0.0.0.0:50051";
  for (int i = 0 ; i < argc - 1; i++) {
    IP_port[i] = argv[i + 1];
    std::cout << "arg server IP and port: " << IP_port[i] << std::endl;
  }

  // SearchClient search_clients[num_clients];
  std::vector<SearchClient> search_clients;
  // SearchClient* search_clients = new SearchClient[num_clients];
  for (int i = 0; i < num_clients; i++) {
    search_clients.push_back(SearchClient(num_servers, IP_port));
  }
  // SearchClient greeter(num_servers, channels);
  std::string user("world");
  int n_iter = 1000;
  std::thread threads_[num_clients];
  for (int i = 0; i < num_clients; i++) {
    threads_[i] = std::thread(&SearchClient::Search, &search_clients[i], user, n_iter); 
  }

  auto start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < num_clients; i++) {
    search_clients[i].StartSearch();
  }
  for (int i = 0; i < num_clients; i++) {
    threads_[i].join();
  }
  auto finish = std::chrono::high_resolution_clock::now();
  double time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();
  std::cout << "Finish " << num_clients << " client requests, " << num_servers << "servers, for " 
            << n_iter << " iterations took " << time_ms << " milliseconds" << std::endl;
  std::cout << "QPS: " << float(num_clients) * n_iter / (time_ms / 1000) << std::endl;

  return 0;
}
