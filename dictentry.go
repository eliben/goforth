package main

// DictEntry is an interface that represents an entry in the Forth dictionary.
type DictEntry interface {
	isDictEntry()
}

// BuiltinFunc represents a built-in function that is implemented in Go.
// It's passed the name of the word as a string argument when called.
type BuiltinFunc struct {
	Impl func(string)
}

// UserFunc represents a user-defined function. It contains a pointer to
// in the input string where the word's definition starts.
type UserFunc struct {
	Ptr int
}

// Value represents a simple int64 value.
type Value struct {
	Val int64
}

func (_ BuiltinFunc) isDictEntry() {}
func (_ UserFunc) isDictEntry()    {}
func (_ Value) isDictEntry()       {}
