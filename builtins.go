package main

import (
	"fmt"
	"strings"
)

func (it *Interpreter) setupBuiltins() {
	// Initialize the built-in words map.
	it.builtinsMap = map[string]func(string){
		`\`:    it.backslash,
		"(":    it.paren,
		":":    it.colon,
		".":    it.dot,
		`."`:   it.dotQuote,
		"+":    it.binop,
		"-":    it.binop,
		"DROP": it.drop,
		"DUP":  it.dup,
		"EMIT": it.emit,
		"OVER": it.over,
		"SWAP": it.swap,
	}
}

// backslash implements the \ word.
// It ignores everything until the end of the line.
func (it *Interpreter) backslash(string) {
	if endIdx := strings.Index(it.input, "\n"); endIdx != -1 {
		// Truncate the input until the end of the line, skipping the
		// newline character.
		it.input = it.input[endIdx+1:]
	} else {
		// No newline found: input is done.
		it.input = ""
	}
}

// paren implements the ( word.
// It ignores everything until a closing parenthesis is encountered.
// Does not handle nesting -- the first closing paren ends the commend (this
// is consistent with gforth).
func (it *Interpreter) paren(string) {
	if endIdx := strings.Index(it.input, ")"); endIdx != -1 {
		// Truncate the input until the closing parenthesis, skipping the
		// parenthesis itself.
		it.input = it.input[endIdx+1:]
	} else {
		// No closing parenthesis found: input is done.
		it.input = ""
	}
}

// colon implements the : word.
// It parses a definition until a semicolon is encountered, and stores the
// definition in the dictionary.
func (it *Interpreter) colon(string) {
	// TODO
}

// dot implements the . word.
// Print the TOS value to stdout, followed by a space.
func (it *Interpreter) dot(string) {
	value := it.dataStack.MustPop()
	it.stdout.WriteString(fmt.Sprintf("%d ", value))
}

// dotQuote implements the ." word.
// This is an IMMEDIATE word; if it.compileMode, it does nothing except advance
// the input pointer to the next word. If not in compile mode, it actually
// prints out the string to stdout.
func (it *Interpreter) dotQuote(string) {
	// Find the end of the string, which is delimited by a double quote.
	if endIdx := strings.Index(it.input, `"`); endIdx != -1 {
		s := it.input[:endIdx]
		it.input = it.input[endIdx+1:] // Advance the input pointer
		if !it.compileMode {
			it.stdout.WriteString(s)
		}
	} else {
		panic("Unterminated string in .\"")
	}
}

// binop implements binary operators like +, -, etc. that take two values from
// the stack, perform the operation, and push the result back onto the stack.
func (it *Interpreter) binop(op string) {
	v2 := it.dataStack.MustPop()
	v1 := it.dataStack.MustPop()
	var result int64
	switch op {
	case "+":
		result = v1 + v2
	case "-":
		result = v1 - v2
	default:
		panic(fmt.Sprintf("Unknown binary operator: %s", op))
	}
	it.dataStack.Push(result)
}

// drop implements the DROP word.
func (it *Interpreter) drop(string) {
	it.dataStack.MustPop()
}

// dup implements the DUP word.
// Duplicate the TOS value and push it onto the stack.
func (it *Interpreter) dup(string) {
	value := it.dataStack.MustPop()
	it.dataStack.Push(value)
	it.dataStack.Push(value)
}

// TODO: reimplement the "10 emit" with CR once defined...
// emit implements the EMIT word.
// Print the TOS value as a character to stdout.
func (it *Interpreter) emit(string) {
	value := it.dataStack.MustPop()
	if value < 0 || value > 255 {
		panic(fmt.Sprintf("Value out of range for EMIT: %d", value))
	}
	it.stdout.WriteByte(byte(value))
}

// swap implements the SWAP word.
func (it *Interpreter) swap(string) {
	v1 := it.dataStack.MustPop()
	v2 := it.dataStack.MustPop()
	it.dataStack.Push(v1)
	it.dataStack.Push(v2)
}

// over implements the OVER word.
func (it *Interpreter) over(string) {
	v1 := it.dataStack.MustPop()
	v2 := it.dataStack.MustPop()
	it.dataStack.Push(v2)
	it.dataStack.Push(v1)
	it.dataStack.Push(v2)
}
