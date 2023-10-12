# uebernotes

## Build

```bash
# download dependencies
make init-deps

# make shared lib
make core

# make client
make linux
```

## Run

```bash
# with make target
make run

# with binary
./pkg/uebernotes
```

## Tests

```bash
# build and run all tests
make tests

# lib tests
make core-tests

# client tests
make linux-tests
```

## Clean

```bash
# clean both lib and client builds
make clean

# clean shared lib build
make clean-core

# clean client build
make clean-linux

# remove all build artifacts
make clean-all

# clean dependencies builds
make clean-deps
```
