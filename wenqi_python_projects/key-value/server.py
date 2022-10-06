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

from concurrent import futures
import logging

import grpc
import keyvaluestore_pb2
import keyvaluestore_pb2_grpc


class KVS(keyvaluestore_pb2_grpc.KeyValueStoreServicer): # Service name  + Servicer

    KV_pairs = None

    def __init__(self):
        self.KV_pairs = dict()

     # method same should be consistent with .proto file
    def InsertValues(self, pair, context):
        self.KV_pairs[pair.key] = pair.value
        return keyvaluestore_pb2.ResponseCode(code = 0)

    def GetValues(self,request, context):
        if request.key in self.KV_pairs:
            return keyvaluestore_pb2.LookupResponse(value=self.KV_pairs[request.key])
        else:
            return keyvaluestore_pb2.LookupResponse(value=-1)


def serve():
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    keyvaluestore_pb2_grpc.add_KeyValueStoreServicer_to_server(KVS(), server)
    server.add_insecure_port('[::]:50051')
    server.start()
    server.wait_for_termination()


if __name__ == '__main__':
    logging.basicConfig()
    serve()
