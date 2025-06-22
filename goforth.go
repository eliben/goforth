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

	stdout strings.Builder

	builtinsMap map[string]func(string)

	// builtinImmediate marks which built-in words are IMMEDIATE. These
	// words are executed when encountered during compilation.
	builtinImmediate map[string]bool

	// dict is the Forth dictionary of user-defined words. Each word is
	// mapped to an offset in input where the word's definition is.
	dict map[string]int
}

func NewInterpreter() *Interpreter {
	it := &Interpreter{
		dataStack:   Stack[int64]{},
		returnStack: Stack[int64]{},
		compileMode: false,
		dict:        make(map[string]int),
	}
	it.setupBuiltins()
	return it
}

func (it *Interpreter) Run(input string) {
	it.input = input
	it.inputPtr = 0

	for {
		word := it.nextWord()
		if word == "" {
			// Done!
			break
		}

		// TODO: if word in dict, run it. If not, treat it as a number.
		// Implement built-in words as interpreter methods? E.g. :, ." etc...
		wordUpper := strings.ToUpper(word)
		if fn, ok := it.builtinsMap[wordUpper]; ok {
			fn(wordUpper)
		} else {
			// Try to parse the word as an integer.
			if value, err := strconv.ParseInt(word, 10, 64); err == nil {
				it.dataStack.Push(value)
			} else {
				panic(fmt.Sprintf("Unknown word: %s\n", word))
			}
		}
	}
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
