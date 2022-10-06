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
