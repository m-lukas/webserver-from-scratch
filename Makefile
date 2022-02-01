setup:
	mkdir build && cd build && conan install ..
	cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release

compile:
	cmake --build ./build -v

compile-client:
	g++ client/client-test.cpp -std=gnu++11 -o client/client
run:
	./build/bin/main

run-verbose:
	./build/bin/main -v

run-client:
	./client/client