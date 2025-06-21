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

	input  string
	stdout strings.Builder

	builtinsMap map[string]func(string)
}

func NewInterpreter() *Interpreter {
	it := &Interpreter{
		dataStack:   Stack[int64]{},
		returnStack: Stack[int64]{},
	}
	it.setupBuiltins()
	return it
}

func (it *Interpreter) Run(input string) {
	it.input = input

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
				fmt.Printf("Unknown word: %s\n", word)
			}
		}
	}
}

// nextWord retrieves the next word from the input string, skipping any leading
// whitespace. It returns "" if no more words are available.
func (it *Interpreter) nextWord() string {
	// Skip whitespace
	for len(it.input) > 0 && isWhitespace(it.input[0]) {
		it.input = it.input[1:]
	}

	if len(it.input) == 0 {
		return ""
	}

	end := 0
	for end < len(it.input) && !isWhitespace(it.input[end]) {
		end++
	}

	word := it.input[:end]
	it.input = it.input[end:]

	return word
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
