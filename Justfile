compiler := "clang"

build:
    mkdir -p bin
    {{compiler}} -pthread -o bin/bootstrap -I include -std=c18 src/*.c

mock:
    cd mock && node src/index.js

clean:
    rm -rf bin