# A Python Tokenizer
A tokenizer for the python programming language in C.

## Differences
There are a couple of differences between this tokenizer and what cpython
tokenizer does.

This tokenizer:

- **does not** emit NL (I still haven't figured what's the use of this token. Probably just to make tokenizing easier as it's emitted where a newline is not significant, i.e. doesn't indicate the end of a statement.)
- **does not** recognize encoding comment.
- **does not** support unicode.
- **does** emit tokens for each reserved keyword.

I plan on adding support for some of these but not all. The implementation will
differ from the cpython's in many ways.
