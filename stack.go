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

func (s *Stack[T]) Len() int {
	return len(s.values)
}

// Get returns the element at the specified index. It panics
// if the index is out of bounds. It's recommende to use this
// method after checking the length with Len.
func (s *Stack[T]) Get(index int) T {
	return s.values[index]
}
