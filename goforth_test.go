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
