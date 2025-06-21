package main

type Stack[T any] struct {
	values []T
}

func (s *Stack[T]) Push(value T) {
	s.values = append(s.values, value)
}

func (s *Stack[T]) IsEmpty() bool {
	return len(s.values) == 0
}

func (s *Stack[T]) Pop() (T, bool) {
	if s.IsEmpty() {
		var zero T
		return zero, false
	}

	index := len(s.values) - 1
	el := s.values[index]
	s.values = s.values[:index]
	return el, true
}

// MustPop pops a value from the stack and panics if the stack is empty.
func (s *Stack[T]) MustPop() T {
	value, ok := s.Pop()
	if !ok {
		panic("stack underflow")
	}
	return value
}
