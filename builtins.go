package main

import (
	"fmt"
	"strings"
)

func (it *Interpreter) setupBuiltins() {
	addBuiltin := func(name string, fn func(string)) {
		it.dict[strings.ToUpper(name)] = BuiltinFunc{Impl: fn}
	}

	addBuiltin(`\`, it.backslash)
	addBuiltin(`(`, it.paren)
	addBuiltin(`:`, it.colon)
	addBuiltin(`;`, it.semicolon)
	addBuiltin(`.`, it.dot)
	addBuiltin(`."`, it.dotQuote)
	addBuiltin(`+`, it.binop)
	addBuiltin(`-`, it.binop)
	addBuiltin(`*`, it.binop)
	addBuiltin(`.S`, it.dotS)
	addBuiltin(`CELL`, it.cell)
	addBuiltin(`CELL+`, it.cellPlus)
	addBuiltin(`CELLS`, it.cells)
	addBuiltin(`CLEARSTACK`, it.clearstack)
	addBuiltin(`DROP`, it.drop)
	addBuiltin(`DUP`, it.dup)
	addBuiltin(`EMIT`, it.emit)
	addBuiltin(`HERE`, it.here)
	addBuiltin(`OVER`, it.over)
	addBuiltin(`SWAP`, it.swap)
	addBuiltin(`R>`, it.fromR)
	addBuiltin(`>R`, it.toR)
	addBuiltin(`R@`, it.copyFromR)
	addBuiltin(`RDROP`, it.dropR)

	it.builtinImmediate = map[string]bool{
		`."`: true,
		`(`:  true,
		`\`:  true,
	}
}

// dotS implements the .S word.
func (it *Interpreter) dotS(string) {
	it.stdout.WriteString(fmt.Sprintf("<%v> ", it.dataStack.Len()))
	for i := range it.dataStack.Len() {
		value := it.dataStack.Get(i)
		it.stdout.WriteString(fmt.Sprintf("%d ", value))
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
	defer func() {
		it.compileMode = false
	}()

	// The next word is the name of the definition. We save it in the
	// dictionary, mapping to the pointer in the input where its code starts.
	defName := it.nextWord()

	if defName == ";" {
		// Special case for handling an empty definition ": ;".
		// This is a no-op.
		return
	}
	it.dict[strings.ToUpper(defName)] = UserFunc{Ptr: it.inputPtr}

	// Now we need to skip the definition until we find a ';'.
	for {
		word := it.nextWord()
		if word == "" {
			it.fatalErrorf("unterminated definition in ':'")
		}
		if word == ";" {
			// End of the definition.
			break
		}

		// If the word is IMMEDIATE, execute it. It may manipulate the
		// input pointer. Otherwise, just keep going.
		if entry, ok := it.dict[strings.ToUpper(word)]; ok {
			if bfn, ok := entry.(BuiltinFunc); ok && it.builtinImmediate[word] {
				bfn.Impl(word)
			}
		}
	}
}

// semicolon implements the ; word. When it's executed by the interpreter,
// it pops the current input pointer from the return stack, which
// effectively returns to the point where the definition was called.
func (it *Interpreter) semicolon(string) {
	var ok bool
	it.inputPtr, ok = it.ptrStack.Pop()
	if !ok {
		it.fatalErrorf("semicolon called with empty return stack")
	}
}

// dot implements the . word.
// Print the TOS value to stdout, followed by a space.
func (it *Interpreter) dot(string) {
	value := it.popDataStack()
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
	v2 := it.popDataStack()
	v1 := it.popDataStack()
	var result int64
	switch op {
	case "+":
		result = v1 + v2
	case "-":
		result = v1 - v2
	case "*":
		result = v1 * v2
	default:
		it.fatalErrorf("unknown binary operator '%s'", op)
	}
	it.dataStack.Push(result)
}

// cell implements the CELL word. We use 8 bytes as the size of a cell.
func (it *Interpreter) cell(string) {
	it.dataStack.Push(8)
}

// cellPlus implements the CELL+ word.
func (it *Interpreter) cellPlus(string) {
	n := it.popDataStack()
	it.dataStack.Push(n + 8)
}

// cells implements the CELLS word.
func (it *Interpreter) cells(string) {
	count := it.popDataStack()
	it.dataStack.Push(count * 8)
}

// clearstack implements the CLEARSTACK word.
// It clears the data stack, effectively removing all values from it.
func (it *Interpreter) clearstack(string) {
	it.dataStack = Stack[int64]{}
}

// drop implements the DROP word.
func (it *Interpreter) drop(string) {
	it.popDataStack()
}

// dup implements the DUP word.
// Duplicate the TOS value and push it onto the stack.
func (it *Interpreter) dup(string) {
	value := it.popDataStack()
	it.dataStack.Push(value)
	it.dataStack.Push(value)
}

// TODO: reimplement the "10 emit" with CR once defined...
// emit implements the EMIT word.
// Print the TOS value as a character to stdout.
func (it *Interpreter) emit(string) {
	value := it.popDataStack()
	if value < 0 || value > 255 {
		it.fatalErrorf("value '%d' out of range for EMIT", value)
	}
	it.stdout.WriteByte(byte(value))
}

// here implements the HERE word.
func (it *Interpreter) here(string) {
	it.dataStack.Push(int64(it.memptr))
}

// swap implements the SWAP word.
func (it *Interpreter) swap(string) {
	v1 := it.popDataStack()
	v2 := it.popDataStack()
	it.dataStack.Push(v1)
	it.dataStack.Push(v2)
}

// over implements the OVER word.
func (it *Interpreter) over(string) {
	v1 := it.popDataStack()
	v2 := it.popDataStack()
	it.dataStack.Push(v2)
	it.dataStack.Push(v1)
	it.dataStack.Push(v2)
}

// fromR implements the R> word.
// It pops a value from the return stack and pushes it onto the data stack.
func (it *Interpreter) fromR(string) {
	v := it.popReturnStack()
	it.dataStack.Push(v)
}

// toR implements the >R word.
// It pops a value from the data stack and pushes it onto the return stack.
func (it *Interpreter) toR(string) {
	v := it.popDataStack()
	it.returnStack.Push(v)
}

// copyFromR implements the R@ word.
// It copies the value from the top of the return stack to the data stack
// without removing it from the return stack.
func (it *Interpreter) copyFromR(string) {
	if it.returnStack.Len() == 0 {
		it.fatalErrorf("R@ called with empty return stack")
	}
	v := it.returnStack.Get(it.returnStack.Len() - 1)
	it.dataStack.Push(v)
}

// dropR implements the RDROP word.
// Drops the value from the top of the return stack.
func (it *Interpreter) dropR(string) {
	it.popReturnStack()
}
