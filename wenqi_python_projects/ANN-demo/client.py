# Copyright 2015 gRPC authors.
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
"""The Python implementation of the GRPC helloworld.Greeter client."""

from __future__ import print_function

import logging

import grpc
import ANN_pb2
import ANN_pb2_grpc

import numpy as np
import faiss

import argparse 
parser = argparse.ArgumentParser()
parser.add_argument('--server_IP', type=str, default='127.0.0.1', help="server") # alveo-build-01 10.253.74.5
parser.add_argument('--server_port', type=str, default='8888')

args = parser.parse_args()
server_IP = args.server_IP
server_port = args.server_port

def run():
    # NOTE(gRPC Python Team): .close() is possible on a channel and should be
    # used in circumstances in which the with statement does not fit the needs
    # of the code.


    d = 64                           # dimension
    nq = 2
    xq = np.random.random((nq, d)).astype('float32')
    xq[:, 0] += np.arange(nq) / 1000.


    topK = 100
    nprobe = 1

    with grpc.insecure_channel('{}:{}'.format(server_IP, server_port)) as channel:

        stub = ANN_pb2_grpc.ANNStub(channel) # proto service name + Stub

        # serialize
        xq = xq.reshape(-1,).tolist()
        request = ANN_pb2.SearchRequest(nq = nq, topK = topK, nprobe=nprobe, xq=xq)

        response = stub.Search(request)
        print("Result dist: ", response.results_dist)
        print("Result ID: ", response.results_ID)


if __name__ == '__main__':
    logging.basicConfig()
    run()
