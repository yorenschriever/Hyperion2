FROM alpine:3.17
LABEL Description="Hyperion"

RUN apk add cmake=3.24.4-r0 make=4.3-r1 gcc=12.2.1_git20220924-r4 g++=12.2.1_git20220924-r4 openssl=3.0.8-r4 openssl-dev=3.0.8-r4 git linux-headers bash

WORKDIR /home
RUN wget https://boostorg.jfrog.io/artifactory/main/release/1.82.0/source/boost_1_82_0.tar.gz
RUN tar xzvf boost_1_82_0.tar.gz 

ENV HYPERION_LIB_DIR=/hyperion_lib
ENV BOOST_DIR=/home/boost_1_82_0
ENV CMAKE_C_COMPILER=/usr/bin/gcc
ENV CMAKE_CXX_COMPILER=/usr/bin/g++

EXPOSE 443
