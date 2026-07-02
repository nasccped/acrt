import os, sys, subprocess
from enum import Enum, auto
from os import path

"""Default output directory."""
OUT_DIR: str = "./out"

"""Default testing directory."""
TEST_DIR: str = "./tests"

class TestItemStatus(Enum):
    """Different status for test item."""

    """Variant used when the provided file was already compiled."""
    COMPILED = auto()

    """Variant used when the provided file wasn't compiled."""
    NOT_COMPILED = auto()

    def __str__(self) -> str:
        return (
            "\x1b[92mcompiled\x1b[0m"
            if self is TestItemStatus.COMPILED
            else "\x1b[91mnot compiled\x1b[0m"
        )

    def is_compiled(self) -> bool:
        """If the status for this current item is compiled."""
        return self is TestItemStatus.COMPILED

    @staticmethod
    def from_file_path(file_path: str) -> object:
        """
        Generate a custom TestItemStatus over a file path. Note that the file path must refer to
        the '.exe' file, not the source code.
        """
        return (
            TestItemStatus.COMPILED
            if path.isfile(file_path)
            else TestItemStatus.NOT_COMPILED
        )

class TestResult:

    def __init__(self, name: str, result: object):
        self.name = name
        self.passed = result.returncode == 0
        self.output = result.stdout

    def display_result(self, f: object):
        """Display result pannel for the current test."""
        if self.passed:
            print("[\x1b[92m passed \x1b[0m] {}".format(self.name), file = f)
        else:
            print("[\x1b[91m failed \x1b[0m] {}".format(self.name), file = f)
            print("output:", file = f)
            print(self.output, file = f)

class TestItem:
    """A single test item."""

    def __init__(self, name: str):
        source = f"{TEST_DIR}/{name}"
        self.name = name
        self.source_path = source
        self.compiled_path = TestItem.compiled_path_from_source(source)
        self.compile_status = TestItemStatus.from_file_path(self.compiled_path)

    @staticmethod
    def compiled_path_from_source(source: str) -> str:
        """Returns a compiled path over a source one."""
        return "{}/{}".format(OUT_DIR, "test-" + source.split("/")[-1].replace(".c", ".exe"))

    @staticmethod
    def is_valid_path(p: str) -> bool:
        """Returns if the file path is a valid one (for TestItem building)."""
        return path.isfile(p) and p.endswith(".c")

    def as_detailed_string(self) -> str:
        """Returns the self item as an detailed string."""
        return " > {} [ \x1b[90m{}\x1b[0m ] {}".format(
            self.source_path, self.compiled_path, str(self.compile_status)
        )

    def run_test(self) -> TestResult:
        """Run the self TestItem and returns its results as an object."""
        return TestResult(
            self.name,
            subprocess.run(
                [self.compiled_path],
                stdout = subprocess.PIPE,
                stderr = subprocess.STDOUT,
                text = True,
            )
        )

def all_passed(results: list[TestResult]) -> bool:
    """If there's no errors."""
    return all([r.passed for r in results])

def all_compiled(items: list[TestItem]) -> bool:
    """Returns if all TestItem(s) are already compiled."""
    return all([i.compile_status.is_compiled() for i in items])

def get_items() -> list[TestItem]:
    """Returns all valid 'TestItem' objects."""
    names = [f for f in os.listdir(TEST_DIR)]
    return [TestItem(n) for n in names if TestItem.is_valid_path(f"{TEST_DIR}/{n}")]

def display_missing_compilation_error(items: list[TestItem]):
    """Error message when file compilation missing."""
    print("Some files weren't compiled yet:", end = "\n\n", file = sys.stderr)
    file_list = "\n".join([i.as_detailed_string() for i in items])
    print(file_list, end = "\n\n", file = sys.stderr)
    print("Consider running `make build-test` before...", file = sys.stderr)

def main():
    items = get_items()

    if not all_compiled(items):
        display_missing_compilation_error(items)
        sys.exit(1)

    results = [i.run_test() for i in items]
    f = sys.stdout if all_passed(results) else sys.stderr
    exit_code = 0 if all_passed(results) else 1

    print("Tests result:", end = "\n\n", file = f)

    for result in results:
        result.display_result(f)

    print()
    sys.exit(exit_code)

if __name__ == "__main__":
    main()
