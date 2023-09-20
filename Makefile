SHELL=/bin/bash

PKG_DIR=$(CURDIR)/pkg
FTXUI_INSTALL_DIR=$(PKG_DIR)/ftxui
CATCH2_INSTALL_DIR=$(PKG_DIR)/catch2

.PHONY: core linux run

default: core linux run

### core

core:
	cmake -S . -B core/build -DBUILD_SHARED_LIBS=1
	cd core/build && make -j uebernotes-core

core-tests: build-catch2
	cmake -S . -B core/build
	cd core/build && make -j uebernotes-core-tests
	pkg/uebernotes-core-tests

linux: check-deps
	cmake -S . -B linux/build
	cd linux/build && make -j uebernotes-cli

# TODO: remove ftxui?
linux-tests: check-deps check-test-deps
	cmake -S . -B linux/build
	cd linux/build && make -j uebernotes-cli-tests
	pkg/uebernotes-cli-tests

run: pkg/uebernotes-cli
	@pkg/uebernotes-cli

clean-core:
	rm -rf core/build

clean-linux:
	rm -rf linux/build

clean: clean-core clean-linux
	rm -rf pkg

### dependencies

sb-add:
	git submodule add https://github.com/fnc12/sqlite_orm third_party/sqlite_orm || true
	git submodule add https://github.com/jarro2783/cxxopts third_party/cxxopts   || true
	git submodule add https://github.com/catchorg/Catch2 third_party/catch2      || true
	git submodule add https://github.com/ArthurSonzogni/FTXUI third_party/ftxui  || true

sb-tag:
	cd third_party/sqlite_orm && git checkout tags/v1.8.2
	cd third_party/cxxopts && git checkout tags/v3.1.1
	cd third_party/catch2 && git checkout tags/v3.4.0
	cd third_party/ftxui && git checkout tags/v5.0.0

build-catch2:
	rm -rf pkg/catch2
	mkdir -p pkg/catch2
	cd third_party/catch2 &&\
		cmake -S . -B build &&\
		cmake --build build -- -j &&\
		cmake --install build --prefix $(CATCH2_INSTALL_DIR)

build-ftxui:
	rm -rf pkg/ftxui
	mkdir -p pkg/ftxui
	cd third_party/ftxui &&\
		cmake -S . -B build -DFTXUI_BUILD_EXAMPLES=OFF &&\
		cmake --build build -- -j &&\
		cmake --install build --prefix $(FTXUI_INSTALL_DIR)

build-deps: build-catch2 build-ftxui

check-deps:
	@[ -d "$(FTXUI_INSTALL_DIR)" ] || (echo "Error: install ftxui" && exit 1)

check-test-deps:
	@[ -d "$(CATCH2_INSTALL_DIR)" ] || (echo "Error: install catch2" && exit 1)

sb-rm:
	[ -z "$(SUBMODULE_NAME)" ] && echo "set SUBMODULE_NAME" && exit 1
	git submodule deinit -f third_party/$(SUBMODULE_NAME) || true
	git rm -f third_party/$(SUBMODULE_NAME) || true
	rm -rf \
		$(CURDIR)/third_party/$(SUBMODULE_NAME) \
		$(CURDIR)/pkg/$(SUBMODULE_NAME) \
		$(CURDIR)/.git/modules/third_party/$(SUBMODULE_NAME)

sb-rm-deps:
	SUBMODULE_NAME=ftxui      make rm-dep
	SUBMODULE_NAME=catch2     make rm-dep
	SUBMODULE_NAME=cxxopts    make rm-dep
	SUBMODULE_NAME=sqlite_orm make rm-dep

clean-ftxui:
	rm -rf third_party/ftxui/build

clean-catch2:
	rm -rf third_party/catch2/build
