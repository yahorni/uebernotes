SHELL=/bin/bash

PKG_DIR=$(CURDIR)/pkg
FTXUI_INSTALL_DIR=$(PKG_DIR)/ftxui
CATCH2_INSTALL_DIR=$(PKG_DIR)/catch2
ELPP_INSTALL_DIR=$(PKG_DIR)/elpp

.PHONY: core linux

default: core linux run-tui

### core

core:
	cmake -S . -B core/build -DBUILD_SHARED_LIBS=1
	cd core/build && make -j uebernotes-core

core-tests: build-test-deps
	cmake -S . -B core/build
	cd core/build && make -j uebernotes-core-tests
	pkg/uebernotes-core-tests

linux: build-deps
	cmake -S . -B linux/build
	cd linux/build && make -j uebernotes-cli

build-linux-tests: build-deps build-test-deps
	cmake -S . -B linux/build
	cd linux/build && make -j uebernotes-cli-tests

linux-tests: build-linux-tests
	pkg/uebernotes-cli-tests

linux-tests-tui: build-linux-tests
	pkg/uebernotes-cli-tests manual-tui -c focus # scroll

run: run-tui

run-cli: pkg/uebernotes-cli
	@pkg/uebernotes-cli --list-books

run-tui: pkg/uebernotes-cli
	@pkg/uebernotes-cli

run-help:
	@pkg/uebernotes-cli --help

clean-core:
	rm -rf core/build

clean-linux:
	rm -rf linux/build

clean: clean-core clean-linux
	rm -rf pkg

### dependencies

sb-add:
	git submodule add https://github.com/fnc12/sqlite_orm     third_party/sqlite_orm || true
	git submodule add https://github.com/jarro2783/cxxopts    third_party/cxxopts    || true
	git submodule add https://github.com/catchorg/Catch2      third_party/catch2     || true
	git submodule add https://github.com/ArthurSonzogni/FTXUI third_party/ftxui      || true
	git submodule add https://github.com/abumq/easyloggingpp  third_party/elpp       || true

sb-tag:
	cd third_party/sqlite_orm && git checkout tags/v1.8.2
	cd third_party/cxxopts    && git checkout tags/v3.1.1
	cd third_party/catch2     && git checkout tags/v3.4.0
	cd third_party/ftxui      && git checkout tags/v5.0.0
	cd third_party/elpp       && git checkout tags/v9.97.1

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

build-elpp:
	rm -rf pkg/elpp
	mkdir -p pkg/elpp
	# use ELPP_STL_LOGGING to log stl containers
	cd third_party/elpp &&\
		cmake -S . -B build -Dbuild_static_lib=1 -DCMAKE_INSTALL_PREFIX=$(ELPP_INSTALL_DIR) &&\
		cmake --build build -- -j &&\
		cmake --install build

build-deps:
	@[ -d "$(FTXUI_INSTALL_DIR)"  ] || make build-ftxui
	@[ -d "$(ELPP_INSTALL_DIR)" ] || make build-elpp

build-test-deps:
	@[ -d "$(CATCH2_INSTALL_DIR)" ] || make build-catch2

sb-rm:
	[ -n "$(DEP_NAME)" ] || (echo "set DEP_NAME" ; exit 1)
	git submodule deinit -f third_party/$(DEP_NAME) || true
	git rm -f third_party/$(DEP_NAME) || true
	rm -rf \
		$(CURDIR)/third_party/$(DEP_NAME) \
		$(CURDIR)/pkg/$(DEP_NAME) \
		$(CURDIR)/.git/modules/third_party/$(DEP_NAME)

sb-rm-deps:
	DEP_NAME=ftxui      make sb-rm
	DEP_NAME=catch2     make sb-rm
	DEP_NAME=cxxopts    make sb-rm
	DEP_NAME=sqlite_orm make sb-rm

clean-ftxui:
	rm -rf third_party/ftxui/build

clean-catch2:
	rm -rf third_party/catch2/build

clean-elpp:
	rm -rf third_party/elpp/build
