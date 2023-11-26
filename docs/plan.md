# uebernotes

## places to cover

| WHAT                        | WHERE    | HOW TO COVER                                                             |
| ---                         | ---      | ---                                                                      |
| joplin                      | common   | joplin-like normal books                                                 |
| telegram saved messages     | common   | inbox deck book with fast insert on mobile                               |
| home dir text files         | computer | 1. fast inserting to deck inbox book in client                           |
|                             |          | 2. dmenu-like script to auto insert (like bookmarks.sh)                  |
| dox/TODO.md file            | computer | deck book + conversion between deck/normal                               |
| bookmarks.sh script         | computer | 1. import/export notes to compatible b-marks file                        |
|                             |          | 2. automated script to update local b-marks (directory sync)             |
| chromium bookmarks          | computer | import/export notes to compatible HTML bookmarks                         |
| knife.media links directory | computer | import/export + directory sync                                           |
| newsboat links              | computer | automated script with cli on computer to add note executed from newsboat |
| ~/.local/bin/user/          | computer | notes with code snippets support                                         |
| ssh config                  | computer | automated script with cli on computer to periodicaly update ssh config   |
| notally app                 | phone    | android app (which is such as fast and convenient)                       |
| feeder app                  | phone    | sharing to android app                                                   |
| via bookmarks               | phone    | sharing to android app                                                   |
| instagram bookmarks         | phone    | sharing to android app                                                   |

## library
+ external server WebDAV protocol sync
+ sync mechanism:
    - CRDT sync (https://en.wikipedia.org/wiki/Conflict-free_replicated_data_type)
    - diff sync mechanism
+ note storage:
    - SQLite ORM: https://github.com/fnc12/sqlite_orm
+ search
    - in opened note
    - in all notes
    - ignoring diacritics
    - ignoring case
+ implementation on C++
    - build as static lib for linux client
    - build as shared lib for android app
+ book contents notes
    - two types of books: normal and deck
    - normal book - joplin-like book
    - deck book (think about adding and previewing format...)
        + fast and simple one-button creation (with date)
        + option: key-value list
    - inbox book (created by default), deck type
    - add creation/last_update timestamp to book
+ note types
    - unsorted inbox (default book?)
    - cover all the *places* above
    - add creation/last_update timestamp to note
    - deck note (do as CSV?):
        + prefix
        + content
        + explanation
        + link
+ conversion mechanism between deck and normal
    - option: deck-book to normal-book-note
    - option: choose from several prepared formats
+ settings
    - WebDAV setup
    - sync algorithm setup
+ configuration file
    - choose simple format
    - choose sync strategy
+ logging with different modules
+ import/export - think about it...
+ tests...
    - unittests (catch2)
    - integration tests
        + with local WebDAV server mock
        + script with CLI
+ other TODOs:
    - enable unicode for sqlite(_orm), cxxopts
    - make cxxopts as precompiled header


## linux client
+ C++ lang
+ FXTUI lib
+ markdown preview
+ notes open in external editor
+ CLI interface to perform actions
    - add note
    - sync notes
    - export books (+ filter for url-only notes)
+ settings
    - include library settings
    - editor: `$EDITOR` env var, `xdg-open` or custom executable name (e.g. "vim")
+ some kind of local directory following and syncing mechanism
+ backup system (tied on import/export system)
+ csv in deckbook (delimiter '\,' - ?)

## android client
+ Kotlin lang
+ C++ shared lib integration
+ fast opening (not like `joplin`, but like `notally`)
+ implementation on Kotlin, C++, Go or Rust (need research)
+ markdown preview
+ fast access screen

## telegram bot
Android app will take some significant time to develop. To start using notes earlier a simple telegram bot will be implemented
+ tg bot python backend on digital ocean

## web client
+ try HUGO static page generator
+ GitHub - alexfoxy/lax.js: Simple & lightweight vanilla JS library to create smooth & beautiful animations when you scroll: https://github.com/alexfoxy/lax.js
+ nostalgic-css/NES.css: NES-style CSS Framework: https://github.com/nostalgic-css/NES.css

## analogues
+ Joplin (linux, android, web), [https://github.com/laurent22/joplin]
+ Notally (android), [kotlin], [https://github.com/OmGodse/Notally]
+ AppFlowy (linux, web), [rust/flutter], [https://github.com/AppFlowy-IO/AppFlowy]
+ Notion (web), [https://www.notion.so/product]
+ Bear, [https://bear.app/]
+ Simplenotes, [https://simplenote.com/], textonly
+ Standard notes, [https://github.com/standardnotes], self-host
+ Obsidian, [https://obsidian.md/]

## useful links
+ C++ TUI lib: https://github.com/ArthurSonzogni/FTXUI
+ Kotlin + NDK: https://proandroiddev.com/android-ndk-interaction-of-kotlin-and-c-c-5e19e35bac74
+ cmake shared lib: https://stackoverflow.com/questions/17511496/how-to-create-a-shared-library-with-cmake
+ sqlite_orm examples: https://github.com/fnc12/sqlite_orm/tree/master/examples
+ catch2: https://github.com/catchorg/Catch2/blob/devel/docs/
+ android screening: https://github.com/Genymobile/scrcpy
+ json: https://github.com/stephenberry/glaze
