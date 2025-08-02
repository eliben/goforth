package main

import (
	"os"
	"path/filepath"
	"strings"
	"testing"
)

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
			code, expected, err := getTestFileData(filepath.Join(testDir, file.Name()))
			if err != nil {
				t.Fatalf("Failed to read file: %v", err)
			}

			it := NewInterpreter()
			it.Run(code)
			actual := strings.TrimSpace(it.stdout.String())

			compareExpected(t, expected, actual)
		})
	}
}
