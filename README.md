# gRPC-ANNS

A distributed approximate nearest neighbor system based on gRPC.

## C++ installation guide

https://grpc.io/docs/languages/cpp/quickstart/

In ~/.bashrc, add some environment variable (gRPC bin directory):

```
MY_INSTALL_DIR=$HOME/.local
PATH="$MY_INSTALL_DIR/bin:$PATH"
```

Install gRPC:

```
mkdir -p $MY_INSTALL_DIR
git clone --recurse-submodules -b v1.49.1 --depth 1 --shallow-submodules https://github.com/grpc/grpc
cd grpc
mkdir -p cmake/build
pushd cmake/build
cmake -DgRPC_INSTALL=ON \
      -DgRPC_BUILD_TESTS=OFF \
      -DCMAKE_INSTALL_PREFIX=$MY_INSTALL_DIR \
      ../..
make -j4
make install
popd
```

Build examples:

```
cd examples/cpp/helloworld
mkdir -p cmake/build
pushd cmake/build
cmake -DCMAKE_PREFIX_PATH=$MY_INSTALL_DIR ../..
make -j
```

Run examples, in one terminal run server, in another run client:

```
./greeter_server # terminal 1
./greeter_client # terminal 2
```

## Python installation

https://grpc.io/docs/languages/python/quickstart/

```
conda create -n grpc python=3.7
conda activate grpc
conda install grpcio
conda install grpcio-tools
```

To generate the needed gRPC py files, run:

```
# make sure the proto's include directory (-I) is correct
python -m grpc_tools.protoc -I../protos --python_out=. --grpc_python_out=. helloworld.proto
```

This will generate 'helloworld_pb2.py' (for protocol buffer) and 'helloworld_pb2_grpc.py' (for grpc), but these can be adjusted in the proto file ('package helloworld;'). In the client and server code:

```
import grpc
import helloworld_pb2
import helloworld_pb2_grpc
```
