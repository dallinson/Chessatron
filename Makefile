# Makefile

all: build
	cmake --build build/

build:
	cmake -B build/ -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++

clean:
	rm -rf build/

.PHONY: build all clean