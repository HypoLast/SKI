# SK(I) Combinator Calculus Calculator

SKI is a slightly incomplete featured calculator for SK(I) combinator expressions written in NodeJS Typescript.

Requires Typescript 2.0.0+ and typings to build
```
$ typings i
$ tsc -p .
```
There is also a precompiled main.js in the bin directory for running directly.

Requires nodejs and npm to run
```
$ npm i
$ node bin/main.js
```
All combinators are left-assosiative and take exactly one argument
* ` is an open bracket, it is automatically closed as soon as syntactically possible
* S is the S-combinator: λx.λy.λz.xz(yz)
* K is the K-combinator: λx.λy.x
* I is the I-combinator: λx.x
* .x is the same as the I-combinator except it adds character x to the buffer
* R is the same as the I-combinator except it flushes the buffer to stdout
* all lowercase letters are null lambdas that can be composed but not applied

In addition the interpreter supports the following commands
* :q - quit the interpreter
* \:s - turn on stepping, while stepping press x to stop or any key to continue
* \:s \<value\> - step through the evaluation of the given value
* :r - turn off stepping
* \:d \<symbol\> \<value\> - define a macro, macro symbols are either single upper-case characters or multiple upper case characters in \<>. Macros get completely expanded pre-execution
* :i \<symbol\> - display the value for the macro \<symbol\>
* :l \<filename\> - load \<filename\>.ski and run all lines that don't begin with a #
* :m \<number\> - set the max iteration number, 0 for infinity
* :c - clear the console
* :h - display this help

stdlib.ski and fibs.ski are provided as sample files.

stdlib.ski defines two symbols F and V. F takes two arguments and applies the second to the first. V takes one argument and returns itself.

fibs.ski calculates the Fibonacci numbers and outputs them as lines of asterisks. You probably want to set max iterations to 1000 or so otherwise you'll need to kill the calculator as it results in an infinite loop.