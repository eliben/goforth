# goforth

Thoughts on Go implementation: Forth was designed with a very specific
implementation (in assembly) in mind, and its "compile-mode" and
"interpreter-mode" are manifestations of that. Implementing Forth using a
different approach is difficult as a result.

For example, in my Go implementation I opted for a source-based interpreter,
where the source code is the representation the interpreter reads and acts
upon, without translating it into another representation first (like a syntax
tree, some sort of IR, etc). This is because Forth assumes the source code
is available (e.g. for words like CHAR), so using an intermediate representation
would create its own challenges.

But interepreting the source is challenging: see the implementation of
DO...LOOP, for example, and the subtle interactions with nested IFs, etc.
It would be possible to implement unbounded loops (BEGIN...) similarly, but
it would have equivalent complications.

## Testing

goforth has an extensive automated test harness. Each test consists of a file
in the `testdata` directory that contains some Forth code and the expected
output from that code. The test harness runs the Forth code using each
interpreter and compares the output with what's expected.

Both interpreters are tested using a Go test harness; `goforth_test.go` tests
the Go implementation, and `ctil_test.go` tests the C implementation. Since the
C implementation supports more features, some test files in `testdata` are
prefixed by `ctil-` and are run only for the C implementation.
