# Drax Language

Drax is a very simple compiler that generates native code and is designed to support different backends.

*Support for architecture i386 linux only*

## Compiling

First you must generate the initial configuration

```bash
make config
```

To generate the binary executable run:

```bash
make BACKEND="ASM" 
```
for building the drax binary, you can define which backend to use.
in the example above we used "ASM" which has GNU Assembler as a backend.


To compile a new program you can run:

```bash
./bin/drax tests/hello.dx
```

After compiling a binary will be generated and just run:
```bash
./main
```
