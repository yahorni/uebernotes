# tasks

## investigation
+ C++ CLI argument parsing
    + article: https://medium.com/@mostsignificant/3-ways-to-parse-command-line-arguments-in-c-quick-do-it-yourself-or-comprehensive-36913284460f
    + option: https://github.com/jarro2783/cxxopts
+ C++ logging
    + article: https://habr.com/ru/post/313686/
    + option: Baical P7 logging https://baical.net/index.html
    + option: loguru https://github.com/emilk/loguru
    + option: easyloggingpp https://github.com/abumq/easyloggingpp
+ WebDAV implementation
    + option: simple impl https://github.com/erikzenker/WebdavClient
    + option: big library https://github.com/CloudPolis/webdav-client-cpp
    + TUI input: https://github.com/mingsheng13/ip-input-ftxui
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
    + compilation with sqlite (v1.8)
        - static compilation [TODO later]
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
    + implement logging [ok]
        - log storage [ok]
        - log to file [ok]
        - logrotate [ok]
    + implement TUI [...]
        - implement scrollable list [ok]
        - screen resize handling [ok]
        - implement scrollable text viewer [ok]
        - global bindings: [...]
            + bind: quit app with q [ok]
            + bind: quit app with Ctrl-c
        - three panes joplin-like interface [...]
            + print books on left pane [ok]
            + print notes first line on middle pane (think of separating it as a replacable format)
            + print note content on right pane [ok]
            + bind: switch panes with h/l, Left/Right, Shift+Tab/Tab
            + bind: switch panes to the right with Enter (only on left and middle panes)
            + bind: scroll items in lists with j/k, Down/Up
            + bind: scroll note content with j/k, Down/Up
            + bind: refresh content with r (refresh cache)
        - status line last log
            + bind: search button with '/' (mock search)
            + bind: command input with ':' (mock commands)
            + bind: ESC to cancel status line input (both search and command)
        - command/keybinding proper implementation
            + cycle (toggle) commands
        - toggle panel with logs
        - floating window with help on keybindings
            + bind: help message with '?' (mock floating window, implement later)
        - status-line commands:
            + search (should know call source to differentiate search)
            + status (commons stats for all books/notes)
            + panel (show last logs and commands)
            + help
        - implement search
            + book titles (vim-like)
            + note titles (vim-like)
            + all notes content (think of where to show results)
            + book notes content (think of where to show results)
            + bind: n/N to jump across results
            + status-line interaction (vim-like, without logging jumps)
        - editor opening
            + hardcoded options in priority: nvim, vim, nano, vi
            + configurable through cli (like "database")
            + bind: Enter in right panel to open the note (remember: book may be empty)
            + algorithm:
                1. create temp file
                2. dump note content to it
                3. freeze app keys handling
                4. open with editor in separated process or thread (need to think)
                5. read temp file content
                6. update note in cache/database
                7. unfreeze app
                8. remove temp file
            + chosen editor should be printed in help
    + implement CLI functions in TUI
        - add book
            + bind: 'a' [left pane]
            + command: 'add-book'
        - add note
            + bind: 'a' [middle pane]
            + command: 'add-note'
        - update book:
            + bind: 'u' [left pane]
            + command: 'update-book'
        - update note:
            + bind: 'u' [middle/right panes]
            + command: 'update-note'
        - implement prompt "Are you sure ... [y/N]"
        - remove book:
            + bind: 'd' [left pane]
            + command: 'delete-book'
            + use are_you_sure prompt
        - remove note:
            + bind: 'd' [middle/right panes]
            + command: 'delete-note'
            + use are_you_sure prompt
    + technical debt:
        - properly configure segfault handling
        - add error messages when CLI client fails [ok]
        - divide rendering logic
        - when UI crash during initialization logs are not written in file and myeasylog.log file created
+ common:
    + project level cmake to build lib and linux client
+ testing: create simple testing server
+ lib: add webdav sync for files
+ linux: sync to CLI client
+ android: POC app to sync and show books
+ linux unit tests

## for simplicity in POC
+ handle signals later
+ add proper return codes in client (instead of booleans) later
+ make error print from client, not from database wrapper later
+ non-static compilation with sqlite made for now
+ skip sqlite unicode support enabling
+ skip cxxopts unicode support enabling (https://github.com/jarro2783/cxxopts/wiki/Unicode-support)
+ define proper public API for core later
+ linux: only one app instance assumed running
+ no specific support for huge files viewing in terminal
+ usage of std::string_view to access note content to scroll in terminal
