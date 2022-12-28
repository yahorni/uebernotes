.PHONY: core linux run

core:
	cmake -S . -B core/build/ -DBUILD_SHARED_LIBS=1
	cmake --build core/build -- -j
	cmake --install core/build

linux:
	cmake -S . -B linux/build
	cmake --build linux/build -- -j
	cmake --install linux/build

run:
	@pkg/uebernotes-cli

clean-core:
	rm -rf core/build

clean-linux:
	rm -rf linux/build

clean: clean-core clean-linux
	rm -rf pkg/
