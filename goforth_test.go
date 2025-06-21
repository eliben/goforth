package main

import (
	"os"
	"path/filepath"
	"strings"
	"testing"
)

const delim = "\\ ---- OUT ----"

func TestForthFiles(t *testing.T) {
	testDir := "testdata"
	files, err := os.ReadDir(testDir)
	if err != nil {
		t.Fatalf("Failed to read testdata directory: %v", err)
	}

	for _, file := range files {
		if file.IsDir() || !strings.HasSuffix(file.Name(), ".4th") {
			continue
		}
		t.Run(file.Name(), func(t *testing.T) {
			content, err := os.ReadFile(filepath.Join(testDir, file.Name()))
			if err != nil {
				t.Fatalf("Failed to read file: %v", err)
			}
			parts := strings.SplitN(string(content), delim, 2)
			if len(parts) != 2 {
				t.Fatalf("Test file must contain '%s' delimiter", delim)
			}
			code := strings.TrimSpace(parts[0])
			expected := strings.TrimSpace(parts[1])

			it := NewInterpreter()
			it.Run(code)
			actual := strings.TrimSpace(it.stdout.String())

			compareExpected(t, expected, actual)
		})
	}
}

func compareExpected(t *testing.T, expected, actual string) {
	if expected != actual {
		t.Errorf("Output mismatch.\nExpected:\n%s\nActual:\n%s", expected, actual)
		// fmt.Println([]byte(expected))
		// fmt.Println([]byte(actual))

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
				return
			}
			for j := 0; j < len(expLine); j++ {
				if expLine[j] != actLine[j] {
					t.Logf("line %d, offset %d: expected '%c', got '%c'", i+1, j+1, expLine[j], actLine[j])
					return
				}
			}
		}
	}
}
