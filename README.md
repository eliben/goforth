# goforth

<p align="center">
  <img alt="Logo" src="doc/goforth-logo-sm.png" />
</p>

----

This repository contains two implementations of the
[Forth programming language](https://en.wikipedia.org/wiki/Forth_(programming_language)).

The first is just called `goforth` and lives in the root directory of this
repository. The second is called `ctil` and lives in the `ctil` directory.

## goforth: Go implementation of Forth

`goforth` is implemented in Go and takes an unusual approach to Forth. It acts
as a source-level interpreter. There is no intermediate representation of
the program - the source string is interpreted directly. When a word definition
is encountered, `goforth` links it to the offset in the input string where
the word is defined, and when this word is called the interpreter just
processes its string definition word by word.

This is an interesting experiment but clearly not the best way to implement
Forth; in addition to the performance implications, it makes implementing things
like loops quite difficult (having to carefully keep track of nested constructs
like `IF` to enable proper `LEAVE`), and it's hard to support Forth's famous
self-extension mechanisms - for example, implementing `IF...THEN...ELSE` in
Forth itself using lower-level primitives.

To run `goforth` on a piece of Forth code, simply execute:

```
$ go run . < testdata/fizzbuzz.4th

... or ...
$ go build
$ ./goforth < testdata/fizzbuzz.4th
```

There are many small Forth programs in the `testdata` directory that can serve
as examples to run and play with.

## ctil: lower-level C implementation

`ctil` (stands for "C [Thread Interpretive Language](https://en.wikipedia.org/wiki/Threaded_code))
is a C implementation of Forth. It takes a much more traditional
Forth implementation approach, where the Forth code is actually compiled into
linked dictionary entries, and word invocations are replaced with the addresses
of the dictionary entries for these words.

`ctil` still deviates from the Forth convention of Assembly language
implementations, but it should be able to support pretty much everything.
As an example, take a look at `ctil/prelude.4th` - it contains implementations
of the `variable` word, `IF...THEN...ELSE` conditions and `BEGIN...REPEAT`
loops in Forth itself.

To run `ctil` on a piece of Forth code, first build `ctil`. A Makefile is
included - `ctil` has no external dependencies other than a standard C compiler.

```
$ cd ctil
$ make
```

To run realistic Forth programs, the "prelude" has to be included first - it's
an implementation of several Forth primitives in Forth itself. If you're still
in the `ctil` directory, run:

```
$ ./ctil -i prelude.4th < ../testdata/fizzbuzz.4th
```

The design of `ctil` is inspired by the [jonesforth](http://git.annexia.org/?p=jonesforth.git)
implementation, though some things work differently (I tried to stick closer to standard
Forth).

## Testing

goforth has an extensive automated test harness. Each test consists of a file
in the `testdata` directory that contains some Forth code and the expected
output from that code. The test harness runs the Forth code using each
interpreter and compares the output with what's expected.

Both interpreters are tested using a Go test harness; `goforth_test.go` tests
the Go implementation, and `ctil_test.go` tests the C implementation. Since the
C implementation supports more features, some test files in `testdata` are
prefixed by `ctil-` and are run only for `ctil`.
