package main

import (
	"encoding/binary"
	"fmt"
	"slices"
	"strings"
)

func (it *Interpreter) setupBuiltins() {
	addBuiltin := func(name string, fn func(string)) {
		it.dict[name] = BuiltinFunc{Impl: fn}
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
	addBuiltin(`/`, it.binop)
	addBuiltin(`MOD`, it.binop)
	addBuiltin(`=`, it.binop)
	addBuiltin(`<`, it.binop)
	addBuiltin(`,`, it.comma)
	addBuiltin(`!`, it.exclamation)
	addBuiltin(`+!`, it.plusExclamation)
	addBuiltin(`?`, it.question)
	addBuiltin(`@`, it.at)
	addBuiltin(`.S`, it.dotS)
	addBuiltin(`ALLOT`, it.allot)
	addBuiltin(`CELL`, it.cell)
	addBuiltin(`CELL+`, it.cellPlus)
	addBuiltin(`CELLS`, it.cells)
	addBuiltin(`CLEARSTACK`, it.clearstack)
	addBuiltin(`CONSTANT`, it.constant)
	addBuiltin(`CREATE`, it.create)
	addBuiltin(`VARIABLE`, it.variable)
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
	addBuiltin(`IF`, it.if_)
	addBuiltin(`DO`, it.do_)
	addBuiltin(`LEAVE`, it.leave)
	addBuiltin(`I`, it.loopIndex)
	addBuiltin(`J`, it.loopIndex)
	addBuiltin(`K`, it.loopIndex)

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
	it.dict[defName] = UserFunc{Ptr: it.inputPtr}

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
		if entry, ok := it.dict[word]; ok {
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
	case "MOD":
		if v2 == 0 {
			it.fatalErrorf("division by zero in 'MOD'")
		}
		result = v1 % v2
	case "/":
		if v2 == 0 {
			it.fatalErrorf("division by zero in '/'")
		}
		result = v1 / v2
	case "=":
		if v1 == v2 {
			result = -1
		} else {
			result = 0
		}
	case "<":
		if v1 < v2 {
			result = -1
		} else {
			result = 0
		}
	default:
		it.fatalErrorf("unknown binary operator '%s'", op)
	}
	it.dataStack.Push(result)
}

// allot implements the ALLOT word.
func (it *Interpreter) allot(string) {
	// The next word is the number of bytes to allot.
	count := int(it.popDataStack())
	if count < 0 {
		it.fatalErrorf("ALLOT called with negative count %d", count)
	}
	if it.memptr+count > len(it.memory) {
		it.fatalErrorf("ALLOT exceeds memory bounds: %d + %d > %d", it.memptr, count, len(it.memory))
	}
	it.memptr += count
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

// constant implements the CONSTANT word.
func (it *Interpreter) constant(string) {
	// The next word is the name of the constant.
	defName := it.nextWord()
	if defName == "" {
		it.fatalErrorf("CONSTANT called with no name")
	}
	val := it.popDataStack()
	it.dict[defName] = Value{Val: val}
}

// comma implements the , word.
func (it *Interpreter) comma(string) {
	// The next word is the value to store in memory.
	value := it.popDataStack()
	if it.memptr+8 > len(it.memory) {
		it.fatalErrorf("memory overflow in ,: cannot store value %d at address %d", value, it.memptr)
	}

	binary.LittleEndian.PutUint64(it.memory[it.memptr:], uint64(value))
	it.memptr += 8
}

// exclamation implements the ! word.
func (it *Interpreter) exclamation(string) {
	addr := it.popDataStack()
	value := it.popDataStack()
	if addr < 0 || addr >= int64(it.memptr) {
		it.fatalErrorf("address %d out of bounds for !", addr)
	}

	binary.LittleEndian.PutUint64(it.memory[addr:], uint64(value))
}

// plusExclamation implements the +! word.
func (it *Interpreter) plusExclamation(string) {
	addr := it.popDataStack()
	addend := it.popDataStack()
	if addr < 0 || addr >= int64(it.memptr) {
		it.fatalErrorf("address %d out of bounds for +!", addr)
	}

	value := int64(binary.LittleEndian.Uint64(it.memory[addr:])) + addend
	binary.LittleEndian.PutUint64(it.memory[addr:], uint64(value))
}

// question implements the ? word.
func (it *Interpreter) question(string) {
	addr := it.popDataStack()
	if addr < 0 || addr >= int64(it.memptr) {
		it.fatalErrorf("address %d out of bounds for ?", addr)
	}

	value := binary.LittleEndian.Uint64(it.memory[addr:])
	it.stdout.WriteString(fmt.Sprintf("%d ", int64(value)))
}

// at implements the @ word.
func (it *Interpreter) at(string) {
	addr := it.popDataStack()
	if addr < 0 || addr >= int64(it.memptr) {
		it.fatalErrorf("address %d out of bounds for @", addr)
	}

	value := binary.LittleEndian.Uint64(it.memory[addr:])
	it.dataStack.Push(int64(value))
}

// create implements the CREATE word.
func (it *Interpreter) create(string) {
	// The next word is the name of the created word.
	defName := it.nextWord()
	if defName == "" {
		it.fatalErrorf("CREATE called with no name")
	}

	it.dict[defName] = Value{Val: int64(it.memptr)}
}

// variable implements the VARIABLE word.
func (it *Interpreter) variable(string) {
	// The next word is the name of the created word.
	defName := it.nextWord()
	if defName == "" {
		it.fatalErrorf("CREATE called with no name")
	}

	it.dict[defName] = Value{Val: int64(it.memptr)}
	it.memptr += 8
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

// if_ implements the IF word.
// We interpret IF statements each time we encounter them.
//
//	word IF word word ELSE word word THEN word word
//	     ^
//	     |
//
// When an IF is encountered, it checks the condition on TOS, and if it's true
// it keeps executing until ELSE or THEN are encountered. If the condition is
// false, it skips words until the next ELSE or THEN.
// Nested IFs will result in recursive calls of if_ through executeWord.
func (it *Interpreter) if_(string) {
	condition := it.popDataStack()
	if condition != 0 {
		// Execute words until we encounter ELSE or THEN.
		for {
			switch word := it.nextWord(); word {
			case "":
				it.fatalErrorf("IF statement not terminated with ELSE or THEN")
			case "ELSE":
				// ELSE is skipped because the condition was true.
				it.skipIfUntil("THEN")
				return
			case "THEN":
				// End of the IF statement.
				return
			default:
				it.executeWord(word)
			}

			// If a THEN was skipped while executing this word, we have to
			// return. The counter is kept because we may have nested IFs
			// whose THENs are skipped.
			if it.skippedThenCount > 0 {
				it.skippedThenCount--
				return
			}
		}
	} else {
		// Condition is false, so we don't execute the IF clause.
		// Skip until we encounter:
		// - ELSE: in which case we execute the words until THEN
		// - THEN: in which case the IF statement is done
		if it.skipIfUntil("ELSE", "THEN") == "ELSE" {
			for {
				switch word := it.nextWord(); word {
				case "":
					it.fatalErrorf("IF statement not terminated with THEN")
				case "THEN":
					// End of the IF statement.
					return
				default:
					// Execute the word.
					it.executeWord(word)
				}
			}
		}
	}
}

// do_ implements the DO word.
func (it *Interpreter) do_(string) {
	start := it.popDataStack()
	limit := it.popDataStack()

	// Push the current loop state onto the loop stack.
	it.loopStack.Push(LoopState{
		index:    start,
		limit:    limit,
		startPtr: it.inputPtr,
	})

	// Execute words until we encounter stop words like LOOP or LEAVE. If we
	// encounter a nested DO, recurse.
	for {
		switch word := it.nextWord(); word {
		case "":
			it.fatalErrorf("DO statement not terminated with LOOP")
		case "LOOP":
			// Pop the loop state from the loop stack, check exit condition;
			// then either exit, or continue the loop with an updated state.
			loopState, ok := it.loopStack.Pop()
			if !ok {
				it.fatalErrorf("LOOP called with empty loop stack")
			}

			loopState.index++
			if loopState.index >= loopState.limit {
				// Loop is done, exit.
				return
			}
			it.loopStack.Push(loopState)
			it.inputPtr = loopState.startPtr
		case "+LOOP":
			// Same as LOOP, but increments the loop index by the value on TOS.
			loopState, ok := it.loopStack.Pop()
			if !ok {
				it.fatalErrorf("+LOOP called with empty loop stack")
			}
			increment := it.popDataStack()
			loopState.index += increment
			if loopState.index >= loopState.limit {
				// Loop is done, exit.
				return
			}
			it.loopStack.Push(loopState)
			it.inputPtr = loopState.startPtr
		case "DO":
			it.do_(word)
		default:
			it.executeWord(word)
		}
	}
}

// loopIndex implements the I, J, K words.
func (it *Interpreter) loopIndex(word string) {
	var stackDepth int
	switch word {
	case "I":
		stackDepth = 0
	case "J":
		stackDepth = 1
	case "K":
		stackDepth = 2
	default:
		it.fatalErrorf("unknown loop index word '%s'", word)
	}

	if it.loopStack.Len() <= stackDepth {
		it.fatalErrorf("loop not deep enough for %s", word)
	}
	idx := it.loopStack.Get(it.loopStack.Len() - 1 - stackDepth).index
	it.dataStack.Push(idx)
}

// leave implements the LEAVE word.
// While IF...THEN and DO...LOOP usually nest nicely (which is handled by
// recursive calls to if_ and do_), LEAVE presents some challenges because
// it breaks out of the current loop.
// When LEAVE is encountered, we have to skip all the words until we encounter
// LOOP. In the process, we may also skip over THEN words of IFs embedded in
// the LOOP. Since the enclosing if_ call is still looking for a THEN, we
// have to keep track of this.
// Moreover, the do_ method is looking for a terminating LOOP word, so we
// have to rewind the input pointer to leave LOOP in the input after we
// find it. This is handled by skipLoop().
func (it *Interpreter) leave(string) {
	loopState, ok := it.loopStack.Pop()
	if !ok {
		it.fatalErrorf("LEAVE called with empty loop stack")
	}
	loopState.index = loopState.limit - 1
	it.loopStack.Push(loopState)
	it.skipLoop()
}

// skipUntil skips input words until it finds one of the specified stopWords.
// It returns the word that was found.
func (it *Interpreter) skipIfUntil(stopWords ...string) string {
	nestingDepth := 0
	for {
		word := it.nextWord()
		if word == "" {
			it.fatalErrorf("unable to find terminating %v", stopWords)
		}

		if word == "IF" {
			nestingDepth += 1
		} else if word == "THEN" && nestingDepth > 0 {
			nestingDepth -= 1
		} else if slices.Contains(stopWords, word) {
			return word
		}
	}
}

// skipLoop() skips the input until it finds a LOOP word. When it finds it,
// it leaves it in the input so it can be executed in the do_ method.
func (it *Interpreter) skipLoop() {
	nestingDepth := 0
	for {
		switch word := it.nextWord(); word {
		case "":
			it.fatalErrorf("unable to find LOOP")
		case "DO":
			nestingDepth += 1
		case "LOOP", "+LOOP":
			if nestingDepth == 0 {
				it.rewindWord()
				return
			}
			nestingDepth -= 1
		case "THEN":
			// Take note of THEN words skipped while leaving a loop. This will
			// notify an enclosing if_ that a closing THEN was seen.
			it.skippedThenCount++
		}
	}
}
