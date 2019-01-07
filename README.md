# A Python Tokenizer
A tokenizer for the python programming language in C.

## Differences
There are a couple of differences between this tokenizer and what cpython
tokenizer does.

This tokenizer:

- **does not** emit NL.
- **does not** recognize encoding comment.
- **does not** support unicode.
- **does** emit tokens for each reserved keyword.

I plan on adding support for some of these but not all. The implementation will
differ from the cpython's in many ways.

## Building And Testing
1. Clone the repository and initialize the submodules.
    ```
    git clone --recurse-submodules https://github.com/aymanizz/python-tokenizer.git
    ```
2. Run make. The following make commands (targets) are available:
    - build the tokenizer: `make`.
    - build the tests runner and run it: `make test`.
    - remove the binaries directory: `make clean`.

Ouput executable files can be found in `bin` directory after building.