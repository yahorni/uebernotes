.PHONY: core linux run

core:
	cmake -S . -B core/build -DBUILD_SHARED_LIBS=1
	cd core/build && make -j uebernotes-core

build-catch2:
	mkdir -p pkg/catch2
	cd third_party/catch2 &&\
		cmake -S . -B build &&\
		cmake --build build -- -j &&\
		cmake --install build --prefix ../../pkg/catch2

core-tests: build-catch2
	cmake -S . -B core/build
	cd core/build && make -j uebernotes-core-tests

linux:
	cmake -S . -B linux/build
	cd linux/build && make -j uebernotes-cli

run: pkg/uebernotes-cli
	@pkg/uebernotes-cli

run-tests: pkg/uebernotes-core-tests
	@pkg/uebernotes-core-tests

clean-catch2:
	rm -rf third_party/catch2/build

clean-core:
	rm -rf core/build

clean-linux:
	rm -rf linux/build

clean: clean-core clean-linux
	rm -rf pkg
