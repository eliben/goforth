Proposed dictionary entry structure, with this header:

* Link pointer (8 bytes)
* Name length and flags (2 bytes)
* Word name (padded to 8 byte boundary)
* Definition

One of the things the flags specify is whether the word is direct (written in
C) or not (list of other Forth words). The TIL book calls these "primitives"
and "secondaries".

For direct words: the definition is an 8-byte address of a C function to call.

For indirect words: the definition is a list of 8-byte addresses, each pointing
to the header of another word. The last word in a word definition is a special
EXIT word.

