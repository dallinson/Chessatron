# Makefile

all: build
	cmake --build build/

build:
	cmake -B build/ -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=clang-17 -DCMAKE_CXX_COMPILER=clang++-17

clean:
	rm -rf build/

.PHONY: build all clean