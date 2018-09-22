# fusor
Obfuscator based on logic-bombs

**Tested on llvm-3.8, Debian 9.x**

# Using Docker (Recommended)
How to install [Docker](https://docs.docker.com/install/).
```
wget https://raw.githubusercontent.com/zzrcxb/fusor/master/Dockerfile
docker build -t fusor .
docker run -itd --name {container's name} fusor /bin/bash
```
Use `Ctrl+P Ctrl+Q` to escape, use `docker attach {container's name}` to attach, use `docker ps -a` to view all containers' status.

# Build fusor
```git clone https://github.com/zzrcxb/fusor.git```
```cd fusor && mkdir build```
```cmake ..```
```make && cd ..```

# How to run

Append `-Xclang -load -Xclang /path/to/libArrayPass.so` to your compiler's flag.

To use `-array_size` option:
Append `-mllvm -array_size -mllvm (your array size)`
