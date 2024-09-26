all:
	cmake -S . -B build
	cmake --build build

run-hello-world: all
	./build/hello_world

run-asio-hello-world: all
	./build/asio_hello_world

run-switch: all
	./build/switch

clean:
	rm -rf build
