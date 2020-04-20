setup:
	mkdir build && cd build && conan install ..
	cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release

compile:
	cmake --build ./build -v

run:
	./build/bin/main

run-client:
	./client/client