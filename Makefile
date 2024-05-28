# Makefile

# Extract the -j option if present, or default to 1 if not
J_OPTION=$(filter -j%, $(MAKEFLAGS))
JOBS=$(if $(J_OPTION), $(patsubst -j%, %, $(J_OPTION)), 1)

all: build
	cmake --build build/ --target Chessatron -- -j $(JOBS)
	mv build/Chessatron ${EXE}

build:
	cmake -B build/ -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++

clean:
	rm -rf build/

.PHONY: build all clean