FROM centos:7

RUN yum -y install gcc gcc-c++ glibc-static python python-devel libicu libicu-devel zlib zlib-devel bzip2 bzip2-devel wget which make openssl-devel curl curl-devel readline-devel
RUN wget --no-check-certificate https://ftp.gnu.org/gnu/gcc/gcc-8.2.0/gcc-8.2.0.tar.gz
RUN tar xzf gcc-8.2.0.tar.gz

WORKDIR /gcc-8.2.0

RUN wget --no-check-certificate https://ftp.gnu.org/gnu/gmp/gmp-6.1.0.tar.bz2
RUN wget --no-check-certificate https://ftp.gnu.org/gnu/mpfr/mpfr-3.1.4.tar.bz2
RUN wget --no-check-certificate https://ftp.gnu.org/gnu/mpc/mpc-1.0.3.tar.gz
RUN wget --no-check-certificate https://gcc.gnu.org/pub/gcc/infrastructure/isl-0.18.tar.bz2

RUN ./contrib/download_prerequisites

RUN ./configure --prefix=/usr/local/gcc-8.2.0 --enable-bootstrap --enable-checking=release --enable-languages=c,c++ --disable-multilib --enable-threads=posix
RUN make -j$(nproc)
RUN make install

ENV PATH="/usr/local/gcc-8.2.0/bin:${PATH}"
ENV LD_LIBRARY_PATH="/usr/local/gcc-8.2.0/lib64:${LD_LIBRARY_PATH}"

WORKDIR /usr/lib64
RUN rm -f libstdc++.so.6
RUN ln -s /usr/local/gcc-8.2.0/lib64/libstdc++.so.6.0.25 libstdc++.so.6

RUN yum remove -y gcc

WORKDIR /

RUN wget --no-check-certificate https://github.com/Kitware/CMake/releases/download/v3.26.4/cmake-3.26.4.tar.gz
RUN tar xzf cmake-3.26.4.tar.gz

WORKDIR /cmake-3.26.4

RUN ./bootstrap
RUN make -j$(nproc)
RUN make install

RUN wget --no-check-certificate https://boostorg.jfrog.io/artifactory/main/release/1.73.0/source/boost_1_73_0.tar.gz
RUN tar xzf boost_1_73_0.tar.gz

WORKDIR /boost_1_73_0

RUN ./bootstrap.sh --prefix=/usr/local

RUN ./b2 install

WORKDIR /

RUN rm -rf /gcc-8.2.0 /gcc-8.2.0.tar.gz
RUN rm -rf /cmake-3.26.4 /cmake-3.26.4.tar.gz
RUN rm -rf /boost_1_73_0 /boost_1_73_0.tar.gz
