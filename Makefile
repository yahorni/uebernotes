SHELL=/bin/bash

PKG_DIR=$(CURDIR)/pkg
FTXUI_INSTALL_DIR=$(PKG_DIR)/ftxui
CATCH2_INSTALL_DIR=$(PKG_DIR)/catch2
ELPP_INSTALL_DIR=$(PKG_DIR)/elpp

# TODO: rename linux client to tui/cli
.PHONY: core linux

default: core linux run

### core

core:
	cmake -S . -B core/build -DBUILD_SHARED_LIBS=1
	cd core/build && make -j uebernotes-core

core-tests: build-test-deps
	cmake -S . -B core/build
	cd core/build && make -j uebernotes-core-tests
	pkg/uebernotes-core-tests

### linux

pkg/uebernotes: linux

linux: build-linux-deps
	cmake -S . -B linux/build
	cd linux/build && make -j uebernotes

build-linux-tests: build-linux-deps build-test-deps
	cmake -S . -B linux/build
	cd linux/build && make -j uebernotes-tests

linux-tests: build-linux-tests
	pkg/uebernotes-tests

linux-tests-tui: build-linux-tests
	@# file: linux/tests/tui.cpp
	@# focus
	@# scroll
	@# switch-with-tab
	pkg/uebernotes-tests manual-tui -c "scroll"

### common

tests: core-tests linux-tests

run: pkg/uebernotes
	@pkg/uebernotes

clean-core:
	rm -rf core/build
	rm -f pkg/libuebernotes-core.so

clean-linux:
	rm -rf linux/build
	rm -f pkg/libuebernotes-core.a pkg/uebernotes

clean: clean-core clean-linux

clean-all: clean-core clean-linux
	rm -rf pkg

### dependencies

init-deps: sb-add-deps sb-tag-deps
clean-deps: sb-rm-deps

sb-add-deps:
	git submodule add --force https://github.com/fnc12/sqlite_orm     extras/sqlite_orm || true
	git submodule add --force https://github.com/jarro2783/cxxopts    extras/cxxopts    || true
	git submodule add --force https://github.com/catchorg/Catch2      extras/catch2     || true
	git submodule add --force https://github.com/ArthurSonzogni/FTXUI extras/ftxui      || true
	git submodule add --force https://github.com/abumq/easyloggingpp  extras/elpp       || true

sb-tag-deps:
	cd extras/sqlite_orm && git checkout tags/v1.8.2
	cd extras/cxxopts    && git checkout tags/v3.1.1
	cd extras/catch2     && git checkout tags/v3.4.0
	cd extras/ftxui      && git checkout tags/v5.0.0
	cd extras/elpp       && git checkout tags/v9.97.1

sb-rm-deps:
	DEP_NAME=sqlite_orm make sb-rm
	DEP_NAME=cxxopts    make sb-rm
	DEP_NAME=catch2     make sb-rm
	DEP_NAME=ftxui      make sb-rm
	DEP_NAME=elpp       make sb-rm

sb-rm:
	[ -n "$(DEP_NAME)" ] || (echo "usage: DEP_NAME=<dep> make sb-rm" ; exit 1)
	git submodule deinit -f extras/$(DEP_NAME) || true
	git rm -f extras/$(DEP_NAME) || true
	rm -rf \
		extras/$(DEP_NAME) \
		pkg/$(DEP_NAME) \
		.git/modules/extras/$(DEP_NAME)

build-linux-deps:
	@[ -d "$(FTXUI_INSTALL_DIR)"  ] || make build-ftxui
	@[ -d "$(ELPP_INSTALL_DIR)" ] || make build-elpp

build-test-deps:
	@[ -d "$(CATCH2_INSTALL_DIR)" ] || make build-catch2

build-catch2:
	rm -rf pkg/catch2
	mkdir -p pkg/catch2
	cd extras/catch2 &&\
		cmake -S . -B build &&\
		cmake --build build -- -j &&\
		cmake --install build --prefix $(CATCH2_INSTALL_DIR)

build-ftxui:
	rm -rf pkg/ftxui
	mkdir -p pkg/ftxui
	cd extras/ftxui &&\
		cmake -S . -B build \
			-DFTXUI_BUILD_EXAMPLES=OFF &&\
		cmake --build build -- -j &&\
		cmake --install build --prefix $(FTXUI_INSTALL_DIR)

build-elpp:
	rm -rf pkg/elpp
	mkdir -p pkg/elpp
	# use ELPP_STL_LOGGING to log stl containers
	# ELPP_DEFAULT_LOG_FILE to set default file
	cd extras/elpp &&\
		cmake -S . -B build \
			-Dbuild_static_lib=1 \
			-DCMAKE_INSTALL_PREFIX=$(ELPP_INSTALL_DIR) \
			-DCMAKE_CXX_FLAGS=-DELPP_NO_DEFAULT_LOG_FILE &&\
		cmake --build build -- -j &&\
		cmake --install build

clean-ftxui:
	rm -rf $(FTXUI_INSTALL_DIR)
	rm -rf extras/ftxui/build

clean-catch2:
	rm -rf $(CATCH2_INSTALL_DIR)
	rm -rf extras/catch2/build

clean-elpp:
	rm -rf $(ELPP_INSTALL_DIR)
	rm -rf extras/elpp/build
