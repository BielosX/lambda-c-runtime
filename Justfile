compiler := "clang"

build:
    mkdir -p bin
    {{compiler}} -o bin/bootstrap -I src -std=c18 src/*.c

clean:
    rm -rf bin