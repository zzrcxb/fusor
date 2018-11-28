# fusor
Obfuscator based on logic-bombs

**Tested on llvm-3.8, Debian 9.x**

# Using Docker (Recommended)
How to install [Docker](https://docs.docker.com/install/).
## Pull from Docker Hub
```
docker pull zzrcxb/fusor
docker run -itd --name {container's name} fusor /bin/bash
```
## or Build by Yourself
```
wget https://raw.githubusercontent.com/zzrcxb/fusor/master/Dockerfile
docker build -t fusor .
docker run -itd --name {container's name} fusor /bin/bash
```
Use `Ctrl+P Ctrl+Q` to escape, use `docker attach {container's name}` to attach, use `docker ps -a` to view all containers' status.

# Build fusor
```
git clone https://github.com/zzrcxb/fusor.git
cd fusor && mkdir build && cd build
cmake ..
make && cd ..
```

# How to run
## Setup environment variable
To specify the configure file's path, set environment variable `FUSOR_CONFIG`.
```
export FUSOR_CONFIG=/path/to/config.json
```

## Change CFLAGS
Append `-Xclang -load -Xclang /path/to/libFusorPass.so` to your compiler's flag.

# Libraries
The puzzle code or the transformation code is `uint64_t` type.
## DeepArrayPuzzle
- ```array size := puzzle_code[63:56]```
- ```first dimension depth := puzzle_code[55:48]```
- ```second dimension depth := puzzle_code[47:40]```
## BogusCFGTransformer
- ```obfuscation probability := trans_code[64:56]```
- ```obfuscation times := trans_code[55:48]```


