# Compilazione

## Server

```sh
gcc -std=c2x src/server/*.c src/common/*.c -Isrc -o server
```

## Client

```sh
gcc -std=c2x src/client/*.c src/client/*.c -Isrc -o client
```
