package main

import (
	"strings"
	"testing"
)

func compareExpected(t *testing.T, expected, actual string) {
	if expected != actual {
		t.Errorf("Output mismatch.\nExpected:\n%s\nActual:\n%s", expected, actual)

		expectedLines := strings.Split(expected, "\n")
		actualLines := strings.Split(actual, "\n")

		if len(expectedLines) != len(actualLines) {
			t.Logf("number of lines differs (expected %d, got %d)", len(expectedLines), len(actualLines))
		}

		for i := 0; i < len(expectedLines); i++ {
			expLine := expectedLines[i]
			actLine := actualLines[i]
			if len(expLine) != len(actLine) {
				t.Logf("line %d: length differs (expected %d, got %d)", i+1, len(expLine), len(actLine))
			}
			for j := 0; j < max(len(expLine), len(actLine)); j++ {
				var exp, act string
				if j < len(expLine) {
					exp = string(expLine[j])
				} else {
					exp = "<none>"
				}
				if j < len(actLine) {
					act = string(actLine[j])
				} else {
					act = "<none>"
				}

				if exp != act {
					t.Logf("line %d, offset %d: expected '%s', got '%s'", i+1, j+1, exp, act)
					if len(exp) == 1 {
						t.Logf("  (expected character #%d)", exp[0])
					}
					if len(act) == 1 {
						t.Logf("  (actual character #%d)", act[0])
					}
					return
				}
			}
		}
	}
}
