Environment - Centos 7
======================

yum -y install gcc gcc-c++ glibc-static python python-devel libicu libicu-devel zlib zlib-devel bzip2 bzip2-devel wget which make openssl-devel xz

GCC 11.2 - without building
================

yum install -y centos-release-scl-rh

yum install -y devtoolset-11-gcc*

ln -s /opt/rh/devtoolset-11/root/bin/gcc /usr/bin/gcc

ln -s /opt/rh/devtoolset-11/root/bin/g++ /usr/bin/g++

Ref: https://blog.csdn.net/Higer2008/article/details/123322867

GCC 8.2.0
=========

wget --no-check-certificate https://ftp.gnu.org/gnu/gcc/gcc-8.2.0/gcc-8.2.0.tar.gz

tar xzf gcc-8.2.0.tar.gz

cd gcc-8.2.0

./contrib/download_prerequisites

gmp-6.1.0.tar.bz2: OK

mpfr-3.1.4.tar.bz2: OK

mpc-1.0.3.tar.gz: OK

isl-0.18.tar.bz2: OK


mkdir build;cd build

../configure --prefix=/usr/local/gcc-8.2.0 --enable-bootstrap --enable-checking=release --enable-languages=c,c++ --disable-multilib --enable-threads=posix

make -j $(nproc)

make install

echo 'export PATH=/usr/local/gcc-8.2.0/bin:$PATH' >> ~/.bashrc; source ~/.bashrc

rm -f /usr/lib64/libstdc++.so.6

cp -rp /usr/local/gcc-8.2.0/lib64/libstdc++.so.6* /usr/lib64

Reference:

http://wfeii.com/2021/01/25/centos7-gcc10.html

Boost libraries 1.83.0
=======================

wget https://github.com/boostorg/boost/releases/download/boost-1.83.0/boost-1.83.0.tar.gz

tar xzf boost-1.83.0.tar.gz

cd boost-1.83.0

chmod 777 tools/build/src/engine/build.sh

./bootstrap.sh --with-libraries=all --prefix=./compiled/boost

./b2 cxxflags="-fPIC" variant=release threading=multi link=static runtime-link=static install

CMake 3.26.4
=========

wget --no-check-certificate https://github.com/Kitware/CMake/releases/download/v3.26.4/cmake-3.26.4.tar.gz

tar xzf cmake-3.26.4.tar.gz

cd cmake-3.26.4/

./bootstrap

make -j$(nproc)

make install

cmake --version

ref: https://gist.github.com/1duo/38af1abd68a2c7fe5087532ab968574e

Abseil LTS 20230125.3
=====================

wget https://github.com/abseil/abseil-cpp/archive/refs/tags/20230125.3.tar.gz

tar xzf 20230125.3.tar.gz

cd abseil-cpp-20230125.3

sed -i 's/\\(ABSL_OPTION_USE_STD_[a-zA-Z0-9_]\*\\) 2/\1 1/g' absl/base/options.h

mkdir build;cd build

export CC=gcc;export CXX=g++

cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF -DBUILD_SHARED_LIBS=NO -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DCMAKE_CXX_STANDARD=17 -DABSL_PROPAGATE_CXX_STD=ON ..

make -j $(nproc)

make DESTDIR=./output install

grep -nr define\ ABSL_LTS_RELEASE_VERSION output/usr/local/include/absl/base/config.h



fmtlib 10.0.0
=============

wget https://github.com/fmtlib/fmt/archive/refs/tags/10.0.0.tar.gz

tar xzf 10.0.0.tar.gz

cd fmt-10.0.0

mkdir build;cd build

cmake -DFMT_TEST=OFF -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE ..

make -j$(nproc)

make DESTDIR=./output install

range-v3 0.12.0
=============

wget https://github.com/ericniebler/range-v3/archive/refs/tags/0.12.0.tar.gz

tar xzf 0.12.0.tar.gz

cd range-v3-0.12.0

mkdir build;cd build

cmake ..

make -j $(nproc)

make DESTDIR=./output install

Catch2 3.3.2
============

wget https://github.com/catchorg/Catch2/archive/refs/tags/v3.3.2.tar.gz

tar xzf v3.3.2.tar.gz

cd Catch2-3.3.2/

mkdir build;cd build

cmake -DCATCH_INSTALL_DOCS=OFF -DCATCH_BUILD_TESTING=OFF ..

make -j $(nproc)

make DESTDIR=./output install


Google Benchmark 1.8.0
============

wget https://github.com/google/benchmark/archive/refs/tags/v1.8.0.tar.gz

tar xzf v1.8.0.tar.gz

wget https://github.com/google/googletest/archive/refs/tags/v1.13.0.tar.gz

tar xzf v1.13.0.tar.gz

mv googletest-1.13.0 benchmark-1.8.0/googletest

cd benchmark-1.8.0

cmake -E make_directory "build"

cmake -E chdir "build" cmake -DBENCHMARK_ENABLE_GTEST_TESTS=OFF -DCMAKE_BUILD_TYPE=Release ../

cmake --build "build" --config Release

cd build

make DESTDIR=./output install


Microsoft GSL 4.0.0
============

wget https://github.com/microsoft/GSL/archive/refs/tags/v4.0.0.tar.gz

mkdir build;cd build

cmake -DGSL_TEST=OFF ..

make -j $(nproc)

make DESTDIR=./output install

Flatbuffers v23.5.26
============

- header only most of the time
- cmake to build flatc

wget https://github.com/google/flatbuffers/archive/refs/tags/v23.5.26.tar.gz

mkdir build;cd build

cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=NO -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DCMAKE_CXX_STANDARD=17 -DFLATBUFFERS_BUILD_TESTS=OFF -DFLATBUFFERS_LIBCXX_WITH_CLANG=OFF -DFLATBUFFERS_BUILD_FLATC=ON ..

make -j$(nproc)

make DESTDIR=./output install

gRPC v1.55.3
=============

git clone -b v1.55.3 https://github.com/grpc/grpc

cd grpc

git submodule update --init

mkdir -p cmake/build

pushd cmake/build

cmake -DABSL_PROPAGATE_CXX_STD=ON -DgRPC_INSTALL=ON -DgRPC_BUILD_TESTS=OFF ../..

make -j$(nproc)

make DESTDIR=./output install

popd

Python 3.8.18 (with Boost Libraries)
==========

wget https://www.python.org/ftp/python/3.8.18/Python-3.8.18.tgz

tar xzf Python-3.8.18.tgz

cd Python-3.8.18

./configure --enable-optimizations

make altinstall

cd boost_1_73_0

./bootstrap.sh --with-python=python3.8 --prefix=/usr/local

./b2 install


Quill v3.3.1
=====

wget https://github.com/odygrd/quill/archive/refs/tags/v3.3.1.tar.gz

 