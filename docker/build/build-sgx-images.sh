#!/bin/bash

set -e

# Navigate to project directory and prepare
cd confidential-computing.sgx && make preparation

# Set common build arguments
BUILD_ARGS="--build-arg https_proxy=$https_proxy --build-arg http_proxy=$http_proxy"
DOCKERFILE_PATH="../../"

# Build AESM container
echo "Building SGX AESM container..."
docker build $BUILD_ARGS --target aesm -t sgx_aesm -f ./Dockerfile $DOCKERFILE_PATH

# Build sample container  
echo "Building SGX Sample container..."
docker build $BUILD_ARGS --target sample -t sgx_sample -f ./Dockerfile $DOCKERFILE_PATH

echo "Build completed successfully!"
