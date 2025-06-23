package main

import (
	"fmt"
	"strings"
)

func (it *Interpreter) setupBuiltins() {
	it.builtinsMap = map[string]func(string){
		`\`:    it.backslash,
		"(":    it.paren,
		":":    it.colon,
		";":    it.semicolon,
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

	it.builtinImmediate = map[string]bool{
		`."`: true,
		`(`:  true,
		`\`:  true,
	}
}

// backslash implements the \ word.
// It ignores everything until the end of the line.
func (it *Interpreter) backslash(string) {
	if idx := strings.Index(it.input[it.inputPtr:], "\n"); idx != -1 {
		// Truncate the input until the end of the line, skipping the
		// newline character.
		it.inputPtr += idx + 1
	} else {
		// No newline found: input is done.
		it.inputPtr = len(it.input)
	}
}

// paren implements the ( word.
// It ignores everything until a closing parenthesis is encountered.
// Does not handle nesting -- the first closing paren ends the commend (this
// is consistent with gforth).
func (it *Interpreter) paren(string) {
	if idx := strings.Index(it.input[it.inputPtr:], ")"); idx != -1 {
		// Truncate the input until the closing parenthesis, skipping the
		// parenthesis itself.
		it.inputPtr += idx + 1
	} else {
		// No closing parenthesis found: input is done.
		it.inputPtr = len(it.input)
	}
}

// colon implements the : word, entering compilation mode.
// It collects the word's definition as a string until the closing ';', and
// stores it in the dictionary. It needs to handle IMMEDIATE words, giving
// them control of parsing (e.g. for commends or .")
func (it *Interpreter) colon(string) {
	it.compileMode = true

	// The next word is the name of the definition. We save it in the
	// dictionary, mapping to the pointer in the input where its code starts.
	defName := it.nextWord()
	it.dict[strings.ToUpper(defName)] = it.inputPtr

	// Now we need to skip the definition until we find a ';'.
	for {
		// TODO: what about ": ;" ???
		word := it.nextWord()
		if word == "" {
			// TODO: we need better error reporting with line/column number.
			// all panics should go through a common error handler that finds
			// this.
			it.fatalErrorf("unterminated definition in ':'")
		}
		if word == ";" {
			// End of the definition.
			break
		}

		// If the word is IMMEDIATE, execute it. It may manipulate the
		// input pointer. Otherwise, just keep going.
		if fn, ok := it.builtinsMap[strings.ToUpper(word)]; ok {
			if it.builtinImmediate[word] {
				fn(word)
			}
		}
	}

	it.compileMode = false
}

// semicolon implements the ; word. When it's executed by the interpreter,
// it pops the current input pointer from the return stack, which
// effectively returns to the point where the definition was called.
func (it *Interpreter) semicolon(string) {
	it.inputPtr = it.ptrStack.MustPop()
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
	if idx := strings.Index(it.input[it.inputPtr:], `"`); idx != -1 {
		s := it.input[it.inputPtr : it.inputPtr+idx]
		it.inputPtr += idx + 1
		if !it.compileMode {
			it.stdout.WriteString(s)
		}
	} else {
		it.fatalErrorf("unterminated string in '.\"'")
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
		it.fatalErrorf("unknown binary operator '%s'", op)
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
		it.fatalErrorf("value '%d' out of range for EMIT", value)
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
