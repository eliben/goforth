package main

import (
	"fmt"
	"strconv"
	"strings"
)

// TODO: need to provide a nice way to define words in forth, i.e. the
// interpreter will load a forth file of builtins.

type Interpreter struct {
	dataStack   Stack[int64]
	returnStack Stack[int64]

	memory [64 * 1024]byte
	memptr int

	// compileMode indicates whether the interpreter is in compile mode.
	compileMode bool

	input    string
	inputPtr int

	// ptrStack is a stack of pointers to the input string. This is used
	// to keep track of where we are in the input when executing words.
	// It serves a similar purpose to the call stack in a typical program,
	// with the input pointer being the "instruction pointer".
	ptrStack Stack[int]

	stdout strings.Builder

	builtinsMap map[string]func(string)

	// builtinImmediate marks which built-in words are IMMEDIATE. These
	// words are executed when encountered during compilation.
	builtinImmediate map[string]bool

	// dict is the Forth dictionary of user-defined words. Each word is
	// mapped to an pointer in input where the word's definition is.
	dict map[string]int
}

func NewInterpreter() *Interpreter {
	it := &Interpreter{
		dataStack:   Stack[int64]{},
		returnStack: Stack[int64]{},
		ptrStack:    Stack[int]{},
		compileMode: false,
		dict:        make(map[string]int),
	}
	it.setupBuiltins()
	return it
}

func (it *Interpreter) Run(input string) {
	it.input = input
	it.inputPtr = 0
	it.doRun()
}

// doRun interprets the input string as a Forth program, starting at
// the current input pointer.
func (it *Interpreter) doRun() {
	for {
		word := it.nextWord()
		if word == "" {
			// Done!
			break
		}

		// If this is a defined word, execute it. Otherwise, try to parse it
		// as a number.
		// Since this is an interpreter, we handle builtins and user-defined
		// words slightly differently.
		wordUpper := strings.ToUpper(word)
		if fn, ok := it.builtinsMap[wordUpper]; ok {
			fn(wordUpper)
		} else if ptr, ok := it.dict[wordUpper]; ok {
			// If the word is in the dictionary, execute it. Save the current
			// pointer on the stack and set it to the location of the word's
			// definition. In the next loop iteration, the interpreter will
			// continue executing from that point.
			it.ptrStack.Push(it.inputPtr)
			it.inputPtr = ptr
		} else {
			// Try to parse the word as an integer.
			if value, err := strconv.ParseInt(word, 10, 64); err == nil {
				it.dataStack.Push(value)
			} else {
				it.fatalErrorf("unknown word '%s'", word)
			}
		}
	}
}

// fatalErrorf reports a fatal error and stops the interpreter, providing
// a message with the current line/column.
func (it *Interpreter) fatalErrorf(format string, args ...any) {
	msg := fmt.Sprintf("program error: "+format+"\n", args...)
	panic(msg)
}

// nextWord retrieves the next word from the input string, skipping any leading
// whitespace. It returns "" if no more words are available.
func (it *Interpreter) nextWord() string {
	it.skipWhitespace()
	if it.endOfInput() {
		return ""
	}

	start := it.inputPtr
	for !it.endOfInput() && !isWhitespace(it.input[it.inputPtr]) {
		it.inputPtr++
	}

	word := it.input[start:it.inputPtr]
	it.skipWhitespace()
	return word
}

// skipWhitespace skips leading whitespace characters in the input string.
func (it *Interpreter) skipWhitespace() {
	for !it.endOfInput() && isWhitespace(it.input[it.inputPtr]) {
		it.inputPtr++
	}
}

func (it *Interpreter) endOfInput() bool {
	return it.inputPtr >= len(it.input)
}

func isWhitespace(c byte) bool {
	return c == ' ' || c == '\n' || c == '\t' || c == '\r'
}

func main() {
	it := NewInterpreter()
	it.Run("42 13 over dup drop . . .")
	fmt.Println(it.stdout.String())

	// it.Run("5 3 + .")
	// fmt.Println(it.stdout.String())
}
