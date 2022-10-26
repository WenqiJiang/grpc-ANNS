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
"""The Python implementation of the GRPC helloworld.Greeter server."""

from cgitb import reset
from concurrent import futures
import logging

import grpc
import ANN_pb2
import ANN_pb2_grpc

import numpy as np
import faiss

import argparse 
parser = argparse.ArgumentParser()
parser.add_argument('--server_port', type=str, default='8888')

args = parser.parse_args()
server_port = args.server_port

class ANN(ANN_pb2_grpc.ANNServicer): # Service name  + Servicer

    print("Initiating index...")
    # Faiss demo: https://github.com/facebookresearch/faiss/blob/main/tutorial/python/2-IVFFlat.py
    d = 64                           # dimension
    nb = 10000                       # database size
    np.random.seed(1234)             # make reproducible
    xb = np.random.random((nb, d)).astype('float32')
    xb[:, 0] += np.arange(nb) / 1000.

    nlist = 100
    quantizer = faiss.IndexFlatL2(d)  # the other index
    index = faiss.IndexIVFFlat(quantizer, d, nlist, faiss.METRIC_L2)

    assert not index.is_trained
    index.train(xb)
    assert index.is_trained
    index.add(xb)                  # add may be a bit slower as well
    print("Finished initiating index.")

    def __init__(self):
        pass

    # method same should be consistent with .proto file
    def Search(self, request):

        # interpret request
        nq = request.nq
        topK = request.topK
        nprobe = request.nprobe
        xq = request.xq
        assert nq * self.d == len(xq)
        xq = np.array(xq, dtype=np.float32).reshape((nq, self.d))

        self.index.nprobe = nprobe
        D, I = self.index.search(xq, topK)
        
        # serialize
        D = D.reshape(-1,).tolist()
        I = I.reshape(-1,).tolist()
        
        response = ANN_pb2.SearchResponse(results_ID=I, results_dist=D)
        
        return response 


def serve():
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    ANN_pb2_grpc.add_ANNServicer_to_server(ANN(), server)
    server.add_insecure_port('[::]:{}'.format(server_port))
    server.start()
    server.wait_for_termination()


if __name__ == '__main__':
    logging.basicConfig()
    serve()
