# tasks

## investigation
+ C++ CLI argument parsing
    + article: https://medium.com/@mostsignificant/3-ways-to-parse-command-line-arguments-in-c-quick-do-it-yourself-or-comprehensive-36913284460f
    + option: https://github.com/jarro2783/cxxopts
+ C++ logging
    + article: https://habr.com/ru/post/313686/
    + option: Baical P7 logging https://baical.net/index.html
+ WebDAV implementation
    + option: simple impl https://github.com/erikzenker/WebdavClient
    + option: big library https://github.com/CloudPolis/webdav-client-cpp
+ XML parsing lib for WebDAV PROPFIND operation
    + nice chart: https://stackoverflow.com/questions/9387610/what-xml-parser-should-i-use-in-c
    + option: https://github.com/leethomason/tinyxml2
    + option: https://github.com/zeux/pugixml
+ local WebDAV server for testing

## development
+ lib: POC normal note and book prototype
    + basic cmake for library
    + compilation as shared lib
    + compilation as static lib
+ linux: POC CLI client with add/show
    + basic cmake
    + include static library
    + ~static~ compilation with sqlite (v1.8)
    + static compilation with catch2
    + static compilation with cxxopts
    + implement functions below through CLI:
        - add book
        - show books
        - add note to book
            + read content from CLI argument
            + read content from STDIN
        - show list of notes from book
        - show note from book (at first - by ID, then - by name, maybe with note name prefix)
            + print to console
            + open in editor
            + open in pager
    + implement same functions as above with TUI
+ common:
    + project level cmake to build lib and linux client
+ testing: create simple testing server
+ lib: add webdav sync for files
+ linux: sync to CLI client
+ android: POC app to sync and show books

## for simplicity in POC
+ store all book notes in book object (without limited caching or LRU)
+ non-static compilation with sqlite made for now
+ skip sqlite unicode support enabling
+ skip cxxopts unicode support enabling (https://github.com/jarro2783/cxxopts/wiki/Unicode-support)
