Whitebox DES
============

This is an explanation on how a whitebox can be implemented,
It follows [Chow et al's paper][1] on the subject.

It uses our previous implementation of [DES in C][2].

[1]: http://www.scs.carleton.ca/%7Epaulv/papers/whitedes1.ps
[2]: https://github.com/mimoo/DES
 
To Do List
----------

* Create encodings for the network of XOR tables.
* Split-Pathing for the XOR tables
* Mixing-bijections for M_0, M_1, M_2 and

How to use the C implementation
-------------------------------

* **WBDES.c** and **WBDES.h** contain the functions used in the Whitebox-DES algorithm.

* **main.c** builds the algorithm and allows you to decrypt an input file using the Whitebox DES. To encrypt you can use a simple DES encryption.

use make to build desbox.

    $ make
    
    $ whitebox --help

* **LtablesGen.c** is a Lookup table generator that takes a key and generate a `LtablesGen` executable. Use it before building the main executable

use `make` to build the lookup table generator:

    $ make Ltablesgen

    $ ./Ltablesgen key

key must be in binary format.
