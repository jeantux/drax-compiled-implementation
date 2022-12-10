# Drax Language

Drax is a very simple compiler that generates native code written in c and thought to support different backends.

*Support for architecture x86_32 linux only*

## Compiling

First you must generate the initial configuration

```bash
make config
```

To generate the binary executable run:

```bash
make
```

To compile a new program you can run:

```bash
./bin/drax tests/hello.dx
```

After compiling a binary will be generated and just run:
```bash
./main
```
