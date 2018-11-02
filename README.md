# A Python Tokenizer
A tokenizer for the python programming language in C.

## Differences
There are a couple of differences between this tokenizer and what cpython
tokenizer does.

This tokenizer:

- does not emit NL.
- does not report some of the errors.
- does not recognize encoding comment.
- does not support unicode.

I plan on adding support for some of these but not all. The implementation will
differ from the cpython's in many ways.
