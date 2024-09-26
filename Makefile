all: build-prod

build-dev:
	cmake -S . -B build/debug -DCMAKE_BUILD_TYPE=Debug
	cmake --build build/debug

build-prod:
	cmake -S . -B build/release -DCMAKE_BUILD_TYPE=Release
	cmake --build build/release

run-hello-world: build-dev
	./build/debug/hello_world

run-asio-hello-world: build-dev
	./build/debug/asio_hello_world

run-switch: build-dev
	./build/debug/switch

clean:
	rm -rf build
