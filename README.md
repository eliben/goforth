# goforth

<p align="center">
  <img alt="Logo" src="doc/goforth-logo-sm.png" />
</p>

----

This repository contains two implementations of the Forth programming language.

The first is just called `goforth` and lives in the root directory of this
repository. The second is called `ctil` and lives in the `ctil` directory.

## goforth - Go implementation of Forth

`goforth` is implemented in Go takes an unusual approach to Forth. It acts
as a source-level interpreter. There is no intermediate representation of
the program - the source string is interpreted directly. When a word definition
is encountered, `goforth` links it to the offset in the input string where
the word is defined, and when this word is called the interpreter just
processes its string definition word by word.

## Testing

goforth has an extensive automated test harness. Each test consists of a file
in the `testdata` directory that contains some Forth code and the expected
output from that code. The test harness runs the Forth code using each
interpreter and compares the output with what's expected.

Both interpreters are tested using a Go test harness; `goforth_test.go` tests
the Go implementation, and `ctil_test.go` tests the C implementation. Since the
C implementation supports more features, some test files in `testdata` are
prefixed by `ctil-` and are run only for the C implementation.
