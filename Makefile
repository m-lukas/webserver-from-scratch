setup:
	mkdir build && cd build && conan install ..
	cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release

compile:
	cmake --build ./build -v

run:
	./build/bin/main

run-verbose:
	./build/bin/main -v

run-client:
	./client/client