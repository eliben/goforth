package main

import "fmt"

func (it *Interpreter) setupBuiltins() {
	// Initialize the built-in words map.
	it.builtinsMap = map[string]func(string){
		".":    it.dot,
		"+":    it.binop,
		"-":    it.binop,
		"DROP": it.drop,
		"DUP":  it.dup,
		"OVER": it.over,
		"SWAP": it.swap,
	}
}

// dot implements the . word.
// Print the TOS value to stdout, followed by a space.
func (it *Interpreter) dot(string) {
	value := it.dataStack.MustPop()
	it.stdout.WriteString(fmt.Sprintf("%d ", value))
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
