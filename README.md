# fusor
Obfuscator based on logic-bombs

** Tested on clang-3.8, Debian 9.x**

# Build fusor
`git clone https://github.com/zzrcxb/fusor.git`
`cd fusor && mkdir build`
`cmake ..`
`make && cd ..`

# How to run

Append `-Xclang -load -Xclang /path/to/libArrayPass.so` to your compiler's flag.

To use `-array_size` option:
Append `-mllvm -array_size -mllvm (your array size)`
