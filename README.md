# Compilazione

## Server

```sh
gcc -std=c2x src/server/*.c src/common/*.c -Isrc -D_POSIX_C_SOURCE=200809L -o server
```

## Client

```sh
gcc -std=c2x src/client/*.c src/common/*.c -Isrc -D_POSIX_C_SOURCE=200809L -o client
```
