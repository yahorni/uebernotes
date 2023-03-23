# style

| type                | case                    |
| ---                 | ---                     |
| class               | CamelCase               |
| method              | pascalCase              |
| argument            | pascalCase              |
| private class field | underscore + pascalCase |

one-line `if` and `for` should be used with brackets
in cpp file header is put on the first line, then one blank line followed by other includes

## notes
+ move private headers to src/, public to include/
+ [add to clang-format] order of headers:
    1. (for cpp file) corresponding header file
    2. headers from same project
    3. headers from subprojects (e.g. core for linux)
    4. headers from third_party
    5. standard library headers
+ [add to clang-format] try-catch: except on the same line, no oneliner in except block
