package main

// LoopState represents the state of DO...LOOP loop in the Forth interpreter.
type LoopState struct {
	// index is the current loop index.
	index int64

	// limit is the loop iteration limit.
	limit int64

	// startPtr is the input pointer to the beginning of the loop (the next
	// word after DO).
	startPtr int
}
