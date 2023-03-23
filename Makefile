.PHONY: core linux run

init-deps:
	git submodule update --init

build-catch2:
	rm -rf pkg/catch2
	mkdir -p pkg/catch2
	cd third_party/catch2 &&\
		cmake -S . -B build &&\
		cmake --build build -- -j &&\
		cmake --install build --prefix ../../pkg/catch2

build-ftxui:
	rm -rf pkg/ftxui
	mkdir -p pkg/ftxui
	cd third_party/FTXUI &&\
		cmake -S . -B build -DFTXUI_BUILD_EXAMPLES=OFF &&\
		cmake --build build -- -j &&\
		cmake --install build --prefix ../../pkg/ftxui

build-deps: build-catch2 build-ftxui

core:
	cmake -S . -B core/build -DBUILD_SHARED_LIBS=1
	cd core/build && make -j uebernotes-core

core-tests: build-catch2
	cmake -S . -B core/build
	cd core/build && make -j uebernotes-core-tests

# TODO: get rid of reinstalling ftxui?
linux: build-ftxui
	cmake -S . -B linux/build
	cd linux/build && make -j uebernotes-cli

# TODO: remove ftxui?
linux-tests: build-catch2 build-ftxui
	cmake -S . -B linux/build
	cd linux/build && make -j uebernotes-cli-tests

run: pkg/uebernotes-cli
	@pkg/uebernotes-cli

run-core-tests: pkg/uebernotes-core-tests
	@pkg/uebernotes-core-tests

run-linux-tests: pkg/uebernotes-cli-tests
	@pkg/uebernotes-cli-tests

tests: core-tests linux-tests run-core-tests run-linux-tests

clean-catch2:
	rm -rf third_party/catch2/build

clean-ftxui:
	rm -rf third_party/FTXUI/build

clean-core:
	rm -rf core/build

clean-linux:
	rm -rf linux/build

clean: clean-core clean-linux
	rm -rf pkg
