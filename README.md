## Author
Mohammad Taquie


## Task
The goal of this project was to implement end to end encryption using a key generator, encryption harnesses, and decryption harness.


## Building
You can use the Makefile to compile this program

```bash
make all

```

## Usage
You can use the beneath command to display all the available options on all executables within this file. Here is an example of how the usage function will prompt on the encryption file. This should prompt a usage help menu.
```bash
./encrypt -H

```
Example of using encryption with verbose output and a specified file.
```bash
./encrypt -i [insert file name here] -v
```
How to generate keys:

```bash
./keygen
```

## Files required to run
~~
encrypt.c

decrypt.c

keygen.c

randstate.c

numtheory.c

ss.c

Makefile
