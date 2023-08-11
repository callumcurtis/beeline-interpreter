Beeline Interpreter

This repository contains the Beeline interpreter, an application that accepts
Beeline source code from standard input and executes the program according to
the well-defined grammar of the Beeline programming language.

The Beeline programming language is a novel language designed for this
project. For the formal grammar of the language, please visit doc/grammar.md.
For a more informal introduction to the Beeline syntax and semantics, please
consider section 3.2 of doc/proposal.pdf.

For the original project proposal, please refer to doc/proposal.pdf. The
proposal document contains many details about the project, including scope,
limitations, and user interface.

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
  formatted as <line number>:<start column>-<end column>. The original proposal
  specified that the non-compliant text would be output. The delivered solution
  does not print the non-compliant text, as the position and explanation together
  effectively identify the non-compliant text.

(The following instructions are taken from the instructor's example repository:
https://github.com/mdadams/uvic_elec586_project_example/blob/master/README.txt)

Let $TOP_DIR denote the directory containing this README file.
Let $INSTALL_DIR denote the directory into which this
software is to be installed.
To build and install the software, use the commands:
    cd $TOP_DIR
    cmake -H. -Bbuild -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR
    cmake --build build --clean-first --target install

To run a demonstration, use the command:
    $INSTALL_DIR/bin/demo

To write your own program, use the following command, write your program in
the terminal, then send EOF (CTRL+D on Linux) to signal the interpreter to start
processing:
    $INSTALL_DIR/bin/beeline

Alternatively, write your program in a file and redirect it to the interpreter:
    $INSTALL_DIR/bin/beeline < path_to_your_input_file

For advanced usage information, use the command:
    $INSTALL_DIR/bin/beeline --help
