# style

| type                | case                    |
| ---                 | ---                     |
| class               | CamelCase               |
| method              | pascalCase              |
| argument            | pascalCase              |
| class field         | pascalCase              |
| private class field | underscore + pascalCase |

in cpp file header is put on the first line, then one blank line followed by other includes

pitchfork structure: <https://github.com/vector-of-bool/pitchfork>

## notes

+ move private headers to `src/`, public to `include/`
+ [add to clang-format] order of headers:
    1. (for cpp file) corresponding header file
    2. headers from same project
    3. headers from subprojects (e.g. core for linux)
    4. headers from `extras/`
    5. standard library headers
+ [add to clang-format] try-catch: except on the same line, no oneliner in except block
