package main

import (
	"fmt"
	"strconv"
	"strings"
)

type Interpreter struct {
	dataStack   Stack[int64]
	returnStack Stack[int64]

	memory [64 * 1024]byte
	memptr int

	// compileMode indicates whether the interpreter is in compile mode,
	// which is required for some words to know how to behave.
	compileMode bool

	input    string
	inputPtr int

	// ptrStack is a stack of pointers to the input string. This is used
	// to keep track of where we are in the input when executing words.
	// It serves a similar purpose to the call stack in a typical program,
	// with the input pointer being the "instruction pointer".
	ptrStack Stack[int]

	loopStack Stack[LoopState]

	stdout strings.Builder

	// builtinImmediate marks which built-in words are IMMEDIATE. These
	// words are executed when encountered during compilation.
	builtinImmediate map[string]bool

	// dict is the Forth dictionary
	dict map[string]DictEntry
}

// NewInterpreter creates a new Interpreter instance.
func NewInterpreter() *Interpreter {
	it := &Interpreter{
		memptr:           0,
		compileMode:      false,
		dict:             make(map[string]DictEntry),
		builtinImmediate: make(map[string]bool),
	}
	it.setupBuiltins()
	return it
}

// Run interprets the input string as a Forth program. State is retained
// between runs, so you can call Run multiple times with different input
// strings to execute multiple Forth programs in the same interpreter instance.
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

		it.executeWord(word)
	}
}

// executeWord executes a single word in the Forth program.
func (it *Interpreter) executeWord(word string) {
	// If this is a defined word, execute it. Otherwise, try to parse it
	// as a number.
	// Since this is an interpreter, we handle builtins and user-defined
	// words slightly differently.
	if entry, ok := it.dict[word]; ok {
		switch entry := entry.(type) {
		case UserFunc:
			// Execute user-defined word. Save the current pointer on the
			// stack and set it to the location of the word's definition. In
			// the next loop iteration, the interpreter will continue
			// executing from that point.
			it.ptrStack.Push(it.inputPtr)
			it.inputPtr = entry.Ptr
		case Value:
			it.dataStack.Push(entry.Val)
		case BuiltinFunc:
			entry.Impl(word)
		default:
			it.fatalErrorf("unknown dictionary entry type for word '%s'", word)
		}
	} else {
		// Try to parse the word as an integer.
		if value, err := strconv.ParseInt(word, 10, 64); err == nil {
			it.dataStack.Push(value)
		} else {
			it.fatalErrorf("unknown word '%s'", word)
		}
	}
}

// fatalErrorf reports a fatal error and stops the interpreter, providing
// a message with the current line/column.
// Note: this error reporting can be improved in many ways - more accurate
// location, "stack trace" etc.
func (it *Interpreter) fatalErrorf(format string, args ...any) {
	// Find the line/column number of the current input pointer.
	line, col := 1, 1
	for i := 0; i < it.inputPtr; i++ {
		if it.input[i] == '\n' {
			line++
			col = 1
		} else {
			col++
		}
	}
	prefix := fmt.Sprintf("program error at (line %d, col %d): ", line, col)
	msg := fmt.Sprintf(prefix+format+"\n", args...)
	panic(msg)
}

// popDataStack pops a value from the data stack and returns it.
// If the stack is empty, it reports a stack underflow error.
func (it *Interpreter) popDataStack() int64 {
	value, ok := it.dataStack.Pop()
	if !ok {
		it.fatalErrorf("data stack underflow")
	}
	return value
}

// popReturnStack pops a value from the return stack and returns it.
// If the stack is empty, it reports a stack underflow error.
func (it *Interpreter) popReturnStack() int64 {
	value, ok := it.returnStack.Pop()
	if !ok {
		it.fatalErrorf("return stack underflow")
	}
	return value
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
	return strings.ToUpper(word)
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
