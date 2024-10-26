# Beeline Interpreter

## About

> This project was selected and completed by Callum Curtis in satisfaction
of requirements for SENG 475: Advanced Programming Techniques for Robust
Efficient Computing at the University of Victoria during the summer of 2023.
Many of the design and documentation decisions in this project were selected
with this specific context in mind. This project counted for 20% of the overall
course grade and was one of six projects selected to serve as examples for
future students in the course.

This repository contains the Beeline interpreter, an application that accepts
Beeline source code from standard input and executes the program according to
the well-defined grammar of the Beeline programming language.

The Beeline programming language is a novel language designed for this
project. For the formal grammar of the language, please visit
[doc/grammar.md](doc/grammar.md). For a less formal introduction to the
Beeline syntax and semantics, please consider section 3.2 of
[doc/proposal.pdf](doc/proposal.pdf).

For the original project proposal, please refer to
[doc/proposal.pdf](doc/proposal.pdf). The proposal document contains many
details about the project, including scope, limitations, and user interface.

## Deviations from the Proposal

During development, some ambiguities in the accepted proposal were discovered.
Additionally, some stretch goals were achieved. The differences between the
delivered and proposed solutions are described below.

- Printing newlines: the proposal specified that the "print" statement would
  not append a newline. The proposal also did not describe any escape
  characters. The preferred solution would have been to modify the "print"
  statement to append a newline. For the sake of consistency with the original
  proposal, this change was not made. Therefore, there is currently no support
  for printing newlines or other escaped characters (e.g., tabs).

- Negative numeric literals: The original proposal specified that numeric
  literals must be nonnegative. The final solution deviates from this by
  offering support for negative numeric literals.

- Error messages: for any syntax, parsing, or runtime error, an explanation
  and position is provided to the user through std::cerr. The position is
  formatted as \<line number\>:\<start column\>-\<end column\>. The original proposal
  specified that the non-compliant text would be output. The delivered solution
  does not print the non-compliant text, as the position and explanation together
  effectively identify the non-compliant text.

## Instructions

The following instructions are adapted from the instructor's [example repository](https://github.com/mdadams/uvic_elec586_project_example/blob/master/README.txt).

Let $TOP_DIR denote the directory containing this README file.
Let $INSTALL_DIR denote the directory into which this
software is to be installed.

To build and install the software, use the commands:

```bash
cd $TOP_DIR
cmake -H. -Bbuild -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR
cmake --build build --clean-first --target install
```

To run a demonstration, use the command:

```bash
$INSTALL_DIR/bin/demo
```

To write your own program, use the following command, write your program in
the terminal, then send EOF (CTRL+D on Linux) to signal the interpreter to start
processing:

```bash
$INSTALL_DIR/bin/beeline
```

Alternatively, write your program in a file and redirect it to the interpreter:

```bash
$INSTALL_DIR/bin/beeline < path_to_your_input_file
```

For advanced usage information, use the command:

```bash
$INSTALL_DIR/bin/beeline --help
```

## Examples

For examples of well-formed Beeline programs, please refer to the [examples](example).

## Benchmarks

### Basic Exponential Smoothing

Basic exponential smoothing over the sequence 0...999,999 was calculated using different languages.

```
-- Beeline
Benchmark 1: build/result/bin/beeline < benchmark/basic-exponential-smoothing/smoothing.txt
  Time (mean ± σ):      2.884 s ±  0.144 s    [User: 2.852 s, System: 0.008 s]
  Range (min … max):    2.733 s …  3.207 s    10 runs

-- Python 3.13.0
Benchmark 2: python3 benchmark/basic-exponential-smoothing/smoothing.py
  Time (mean ± σ):     786.8 ms ±  23.0 ms    [User: 768.2 ms, System: 12.5 ms]
  Range (min … max):   744.9 ms … 836.0 ms    10 runs
 
Summary
  python3 benchmark/basic-exponential-smoothing/smoothing.py ran
    3.67 ± 0.21 times faster than build/result/bin/beeline < benchmark/basic-exponential-smoothing/smoothing.txt
```

