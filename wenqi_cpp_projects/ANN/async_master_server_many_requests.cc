/*
Usage:

./async_master_server_many_requests <optional server_IP:server_port>

e.g.,: 

./async_master_server_many_requests 127.0.0.1:8888

 */

#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

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

// using grpc::grpc_channel_args;

class SearchClient {
 public:
  // explicit SearchClient() {}


  // explicit SearchClient(std::string IP_port) : finished_request_num(0) {
  explicit SearchClient(std::shared_ptr<Channel> channel) : finished_request_num(0) {
        // std::shared_ptr<Channel> channel = grpc::CreateChannel(
        //     IP_port, grpc::InsecureChannelCredentials());
        std::cout << "Use count for the channel shared_ptr:" << channel.use_count() << std::endl;
        // channel->SetChannelArgs();
        stub_ = ANNService::NewStub(channel);
      }

  void SetConcurrency(int max_concurrency_in) {
    // set the max number of requests on the fly
    max_concurrency = max_concurrency_in;
  }

  // Assembles the client's payload and sends it to the server.
  void Search(int n_iter) {

    for (int i = 0; i < n_iter; i++) {
      // Data we are sending to the server.
      SearchRequest request;
      // message SearchRequest {
      //   int32 nq = 1; // num query
      //   int32 topK = 2;   // topK results
      //   int32 nprobe = 3; // number of cells to scan
      //   repeated int64 ivf_list_ids = 4; // the IVF list IDs 
      //   repeated float xq = 5; // query vectors, can contain multiple vectors
      // }
      int nprobe = 2;
      int ivf_list_len = 2;
      long ivf_list_ids[ivf_list_len] = {100, 101};
      int D = 2;
      float xq[D] = {1.2, 3.4};
      request.set_nq(int(1));
      request.set_topk(int(100));
      request.set_nprobe(nprobe);
      for (long id : ivf_list_ids) {
        request.add_ivf_list_ids(id);
      }
      for (float element : xq) {
        request.add_xq(element);
      }

      // Call object to store rpc data
      AsyncClientCall* call = new AsyncClientCall;

      // stub_->PrepareAsyncSearch() creates an RPC object, returning
      // an instance to store in "call" but does not actually start the RPC
      // Because we are using the asynchronous API, we need to hold on to
      // the "call" instance in order to get updates on the ongoing RPC.
      call->response_reader =
          stub_->PrepareAsyncSearch(&call->context, request, &cq_);

      // StartCall initiates the RPC call
      call->response_reader->StartCall();

      // Request that, upon completion of the RPC, "reply" be updated with the
      // server's response; "status" with the indication of whether the operation
      // was successful. Tag the request with the memory address of the call
      // object.
      call->response_reader->Finish(&call->reply, &call->status, (void*)call);
    
      // wait if the max concurrency is reached
      while ( (i - finished_request_num + 2) > max_concurrency) {}
    }
  }

  // Loop while listening for completed responses.
  // Prints out the response from the server.
  void AsyncCompleteRpc(long expect_response_num) {
    void* got_tag;
    bool ok = false;

    // Block until the next result is available in the completion queue "cq".
    for (int i = 0; i < expect_response_num; i++) {
      GPR_ASSERT(cq_.Next(&got_tag, &ok));
      // The tag in this example is the memory location of the call object
      AsyncClientCall* call = static_cast<AsyncClientCall*>(got_tag);

      // Verify that the request was completed successfully. Note that "ok"
      // corresponds solely to the request for updates introduced by Finish().
      GPR_ASSERT(ok);

      if (call->status.ok()) {
        std::cout << "Received results" << std::endl;
      } else {
        std::cout << "RPC failed" << std::endl;
      }
      finished_request_num++;

      // Once we're complete, deallocate the call object.
      delete call;
    }
  }

 private:
  // struct for keeping state and data information
  struct AsyncClientCall {
    // Container for the data we expect from the server.
    SearchResponse reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // Storage for the status of the RPC upon completion.
    Status status;

    std::unique_ptr<ClientAsyncResponseReader<SearchResponse>> response_reader;
  };

  // Out of the passed in Channel comes the stub, stored here, our view of the
  // server's exposed services.
  std::unique_ptr<ANNService::Stub> stub_;

  // The producer-consumer queue we use to communicate asynchronously with the
  // gRPC runtime.
  CompletionQueue cq_;
  // max concurrenct request number
  long max_concurrency;
  // keep track of where we are, for concurrency number control
  long finished_request_num;
};

int main(int argc, char** argv) {
  // Instantiate the client. It requires a channel, out of which the actual RPCs
  // are created. This channel models a connection to an endpoint (in this case,
  // localhost at port 50051). We indicate that the channel isn't authenticated
  // (use of InsecureChannelCredentials()).

  std::string IP_port = "0.0.0.0:50051";
  if (argc >= 2) {
    IP_port = argv[1];
  }
  std::cout << "arg server IP and port: " << IP_port << std::endl;

  std::cout << "Type in number of clients: " << std::endl;
  std::string input;
  getline(std::cin, input);
  int num_clients = std::stol(input);
  // int num_clients = 10;

  std::cout << "Type the maxmimum concurrency request per client: " << std::endl;
  getline(std::cin, input);
  int max_concurrency = std::stol(input);

  int n_iter = 10000;

  // std::shared_ptr<Channel> channel = grpc::CreateChannel(
  //     IP_port, grpc::InsecureChannelCredentials());

  grpc::ChannelArguments cargs[num_clients];
  std::shared_ptr<Channel> channel[num_clients];
  for (int i = 0; i < num_clients; i++) {
    cargs[i].SetInt(std::to_string(i), i);
    channel[i] = grpc::CreateCustomChannel( // CreateCustomChannel can set args
        IP_port, grpc::InsecureChannelCredentials(), cargs[i]);

  }
      
  // std::vector<SearchClient> search_clients(num_clients);
  // double pointer
  SearchClient** search_clients = new SearchClient* [num_clients];
  for (int i = 0; i < num_clients; i++) {
    search_clients[i] = new SearchClient(channel[i]);
    search_clients[i]->SetConcurrency(max_concurrency);
  }

  // for (int i = 0; i < num_clients; i++) {
  //   threads_collect[i] std::thread(&SearchClient::AsyncCompleteRpc, &search_clients[i], n_iter);
  // }

  std::thread threads_collect[num_clients];
  for (int i = 0; i < num_clients; i++) {
    threads_collect[i] = std::thread(&SearchClient::AsyncCompleteRpc, search_clients[i], n_iter);
  }

  auto start = std::chrono::high_resolution_clock::now();

  std::thread threads_request[num_clients];
  for (int i = 0; i < num_clients; i++) {
    threads_request[i] = std::thread(&SearchClient::Search, search_clients[i], n_iter); 
  }

  for (int i = 0; i < num_clients; i++) {
    threads_request[i].join();
    threads_collect[i].join();  // blocks forever
  }

  auto finish = std::chrono::high_resolution_clock::now();
  double time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();
  std::cout << "Search for " << n_iter << " iterations took "
            << time_ms << " milliseconds" << std::endl;
  std::cout << "QPS: " << float(n_iter) * num_clients / (time_ms / 1000) << std::endl;

  return 0;
}
