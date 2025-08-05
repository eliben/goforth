package main

import (
	"io"
	"os"
	"os/exec"
	"path/filepath"
	"slices"
	"strings"
	"testing"
)

const ctilDir = "ctil"
const ctilBinaryName = "ctil"

func ensureCtilBinaryExists(t *testing.T) {
	if _, err := os.Stat(filepath.Join(ctilDir, ctilBinaryName)); os.IsNotExist(err) {
		// cd into the ctil directory and build the binary using 'make'.
		if err := os.Chdir(ctilDir); err != nil {
			t.Fatalf("Failed to change directory to %s: %v", ctilDir, err)
		}
		t.Logf("Building ctil binary in %s", ctilDir)
		// Run 'make' to build the ctil binary.
		if err := exec.Command("make").Run(); err != nil {
			t.Fatalf("Failed to build ctil binary: %v", err)
		}
		// Change back to the original directory.
		if err := os.Chdir(".."); err != nil {
			t.Fatalf("Failed to change back to original directory: %v", err)
		}
	}
}

// slice of supported tests
var supportedTests = []string{
	"arith1.4th",
	"stack1.4th",
	"stack2.4th",
	"word-def1.4th",
}

func TestCtilForthFiles(t *testing.T) {
	ensureCtilBinaryExists(t)
	testDir := "testdata"
	files, err := os.ReadDir(testDir)
	if err != nil {
		t.Fatalf("Failed to read testdata directory: %v", err)
	}

	for _, file := range files {
		if file.IsDir() || !strings.HasSuffix(file.Name(), ".4th") {
			continue
		}
		if !slices.Contains(supportedTests, file.Name()) {
			continue
		}
		t.Run(file.Name(), func(t *testing.T) {
			code, expected, err := getTestFileData(filepath.Join(testDir, file.Name()))
			if err != nil {
				t.Fatalf("Failed to read file: %v", err)
			}

			// t.Logf("Sending code to ctil binary:\n===\n%s\n===", code)

			cmd := exec.Command(filepath.Join(ctilDir, ctilBinaryName))
			cmdStdin, err := cmd.StdinPipe()
			if err != nil {
				t.Fatalf("Failed to create stdin pipe: %v", err)
			}

			if _, err := io.WriteString(cmdStdin, code); err != nil {
				t.Fatalf("Failed to write code to stdin: %v", err)
			}
			if err := cmdStdin.Close(); err != nil {
				t.Fatalf("Failed to close stdin pipe: %v", err)
			}

			b, err := cmd.CombinedOutput()
			if err != nil {
				t.Fatalf("Failed to run ctil binary: %v\n%v", err, string(b))
			}

			// t.Logf("Ctil binary output:\n===\n%s\n===", string(b))

			compareExpected(t, expected, string(b))
		})
	}
}
