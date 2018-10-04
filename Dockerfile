FROM debian:9.4
MAINTAINER Neil Zhao, i@zzrcxb.me

WORKDIR /root/

RUN apt update && \
    apt install -y build-essential cmake llvm vim git clang python python-pip && \
    git clone https://github.com/zzrcxb/fusor.git && \
    pip install angr termcolor

RUN cd fusor && mkdir build && cd build && cmake .. && make


