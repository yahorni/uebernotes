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
+ lib: POC normal note and book prototype [ok]
    + basic cmake for library [ok]
    + compilation as shared lib [ok]
    + compilation as static lib [ok]
+ linux: POC CLI client with add/show [...]
    + basic cmake [ok]
    + include static library [ok]
    + ~static~ compilation with sqlite (v1.8)
    + static compilation with catch2 [ok]
    + static compilation with cxxopts [ok]
    + implement functions below through CLI [...]
        - add book [ok]
        - show books [ok]
        - add note to book [...]
            + read content from CLI argument [ok]
            + read content from STDIN [TODO later]
        - show list of notes from book [ok]
        - update book name [ok]
        - update note content [...]
            + read content from CLI argument [ok]
            + read content from STDIN [TODO later]
        - add caching of loaded items [ok]
        - show note by ID [...]
            + print to console [ok]
            + open in editor [TODO later]
            + open in pager [TODO later]
        - remove book [ok]
        - remove note [ok]
    + implement same functions as above with TUI
+ common:
    + project level cmake to build lib and linux client
+ testing: create simple testing server
+ lib: add webdav sync for files
+ linux: sync to CLI client
+ android: POC app to sync and show books
+ linux unit tests

## for simplicity in POC
+ add proper return codes in client (instead of booleans) later
+ make error print from client, not from database wrapper later
+ non-static compilation with sqlite made for now
+ skip sqlite unicode support enabling
+ skip cxxopts unicode support enabling (https://github.com/jarro2783/cxxopts/wiki/Unicode-support)
+ define proper public API for core later
+ linux: only one app instance assumed running
