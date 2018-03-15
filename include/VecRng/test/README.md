## VecRng: Installation and Tests

### Checkout vecrng branch of VecCore

    cd ${SRC_DIR}
    git clone https://github.com/root-project/veccore VecCore
    cd VecCore
    git checkout -b vecrng origin/vecrng

### How to install VecCore with VecRng, add -DVECRNG=ON

    cd ${BUILD_DIR}
    cmake ${SRC_DIR}/VecCore -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}/VecCore \
          -DVECRNG=ON
    make install

### How to build the VecRng test (RngBenchmark), add -DBUILD_RNGTEST=ON 
 - select a backend: example for Vc with VCROOT to the "Vc" install dir 
   and add -DBACKEND=Vc -DCMAKE_PREFIX_PATH="$VCROOT" to cmake
 - to test the CUDA backend, add -DRNGTEST_CUDA=ON

    cd ${BUILD_DIR}
    cmake $SRC_DIR/VecCore -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}/VecCore \
          -DCMAKE_PREFIX_PATH="$VCROOT" -DBACKEND=Vc \
          -DCMAKE_BUILD_TYPE=Release -DBUILD_RNGTEST=ON -DRNGTEST_CUDA=ON
    make -j4

### How to run RngBenchmark

    cd ${BUILD_DIR}/include/VecRng/test
    ./RngBenchmark [NSample=1000000] [NRepetitions=100]
