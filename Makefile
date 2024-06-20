# Makefile

# Extract the -j option if present, or default to 1 if not
J_OPTION=$(filter -j%, $(MAKEFLAGS))
JOBS=$(if $(J_OPTION), $(patsubst -j%, %, $(J_OPTION)), 1)

all:
	cmake -B build/ -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DPGO=ON
	cmake --build build/ --target Chessatron -- -j $(JOBS)
	cd build && ./Chessatron bench
	llvm-profdata merge -output="build/chessatron.profdata" build/*.profraw
	cmake -B build/ -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DPGO=ON
	cmake --build build/ --target Chessatron -- -j $(JOBS)
	mv build/Chessatron ${EXE}

clean:
	rm -rf build/

.PHONY: all clean