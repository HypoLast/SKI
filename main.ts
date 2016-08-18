/// <reference path="typings/index.d.ts" />
/// <reference path="readlineSync.d.ts" />

import readlineSync = require("readline-sync");
import fs = require("fs");

let options = {
    step: false,
    maxIterations: 0
}
let macros:{ [macro:string]:string } = {};
readlineSync.setDefaultOptions({ prompt: "> " });

function findFirstApply(input:string):[string, string, string, number]|null {
    enum State {
        SCAN,
        READ
    }
    let matchStart = 0;
    let symbolStart = 0;
    let symbolDepth = 0;
    let l:string = "";
    let p:string = "";
    let state = State.SCAN;
    for (let i = 0; i < input.length; i ++) {
        let symbol = input.charAt(i);
        if (state == State.SCAN) {
            if (symbol == '`') {
                matchStart = i;
                l = "";
            } else if (symbol == '(') {
                symbolStart = i;
                symbolDepth = 1;
                state = State.READ;
            } else if (symbol.match(/S|K|I|R|[a-z]/)) {
                if (!l) l = symbol;
                else p = symbol;
            } else if (symbol == '.') {
                if (!l) l = symbol + input.charAt(i + 1);
                else p = symbol + input.charAt(i + 1);
                i ++;
            }
        } else {
            if (symbol == "(") symbolDepth ++;
            else if (symbol == ")") {
                symbolDepth --;
                if (symbolDepth == 0) {
                    if (!l) l = input.substr(symbolStart, i - symbolStart + 1);
                    else p = input.substr(symbolStart, i - symbolStart + 1);
                    state = State.SCAN;
                }
            }
        }

        if (l && p) return [l, p, input.substr(matchStart, i - matchStart + 1), matchStart];
    }
    return null;
}

function getParam(group:string):string {
    if (group.charAt(0) == ".") {
        return group.substr(0, 2);
    } else if (group.charAt(0) != "(") {
        return group.charAt(0);
    } else {
        let depth = 1;
        for (let i = 1; i < group.length; i ++) {
            if (group.charAt(i) == "(") depth ++;
            else if (group.charAt(i) == ")") depth --;
            if (depth == 0) {
                return group.substr(0, i + 1);
            }
        }
    }
    return group.substr(group.length - 1);
}

function reduce(input:string, buffer:string):[string, string] {
    let match = findFirstApply(input);
    if (!match) return [input, buffer];
    let result = "";
    if (match[0] == "S") {
        result = `(S'${match[1]})`
    } else if (match[0] == "K") {
        result = `(K'${match[1]})`
    } else if (match[0] == "I") {
        result = match[1];
    } else if (match[0].indexOf("(S''") == 0) {
        let p1 = getParam(match[0].substr("(S''".length));
        let p2 = getParam(match[0].substr("(S''".length + p1.length));
        result = "``" + p1 + match[1] + "`" + p2 + match[1];
    } else if (match[0].indexOf("(S'") == 0) {
        result = "(S''" + getParam(match[0].substr("(S'".length)) + match[1] + ")";
    } else if (match[0].indexOf("(K'") == 0) {
        result = getParam(match[0].substr("(K'".length));
    } else if (match[0] == "R") {
        console.log(buffer);
        buffer = "";
        result = match[1];
    } else if (match[0][0] == ".") {
        buffer += match[0][1];
        result = match[1];
    } else {
        result = `(${match[0]}${match[1]})`
    }
    let output = input.substr(0, match[3]) + result + input.substr(match[3] + match[2].length);
    return [output, buffer];
}

function execute(input:string):void {
    let output:string = input;
    let buffer:string = "";
    let result:[string, string] = ["", ""];
    let stop = false;
    let iterations = 0;
    do {
        result = reduce(output, buffer)
        input = output;
        output = result[0];
        buffer = result[1];
        if (options.step) {
            console.log(" ~", input);
            stop = readlineSync.keyIn("", { hideEchoBack: true, mask: '' }) == "x";
        }
        iterations ++;
    } while(input != output && !stop && (options.maxIterations == 0 || iterations < options.maxIterations));
    if (buffer) console.log(buffer);
    console.log(" =", input);
}

function expand(input:string):string {
    let original:string;
    do {
        original = input;
        for (let key in macros) {
            input = input.replace(key, macros[key]);
        }
    } while(original != input);
    return input;
}

console.log("SK(I)");

function read(line:string) {
    if (line == ":s") {
        options.step = true;
    } else if (line == ":r") {
        options.step = false;
    } else if (line.indexOf(":d ") == 0) {
        let args = line.split(/ +/g);
        if (args.length != 3) console.log("Invalid define");
        else if (!args[1].match(/^((?:[A-Z])|(?:<[A-Z]+>))$/)) console.log("Invalid identifier");
        else if (["S", "K", "I", "R"].indexOf(args[1]) >= 0) console.log("Can't redefine built-in");
        else if (args[2].indexOf(args[1]) >= 0) console.log("Can't define a macro in terms of itself");
        else {
            macros[args[1]] = args[2];
            console.log("Defined symbol", args[1]);
        }
    } else if (line.indexOf(":l ") == 0) {
        let args = line.split(/ +/g);
        let lines:string[] = [];
        try {
            lines = fs.readFileSync(args[1] + ".ski").toString().split(/\r?\n/g).filter(e => e.charAt(0) != "#");
        } catch(e) {
            console.log("Error loading file");
        }
        lines.forEach(l => read(l));
    } else if (line.indexOf(":i ") == 0) {
        console.log(macros[line.split(/ +/g)[1]]);
    } else if (line.indexOf(":m ") == 0) {
        let iters:number = parseInt(line.split(/ +/g)[1]) | 0;
        options.maxIterations = iters;
        console.log("Max iterations set to", iters == 0 ? "infinity" : iters);
    } else if (line == ":c") {
        process.stdout.write("\x1Bc");
    } else if (line == ":h") {
        console.log("All combinators are left-assosiative and take exactly one argument");
        console.log(" ` is an open bracket, it is automatically closed as soon as syntactically possible");
        console.log(" S is the S-combinator: λx.λy.λz.xz(yz)");
        console.log(" K is the K-combinator: λx.λy.x");
        console.log(" I is the I-combinator: λx.x");
        console.log(" .x is the same as the I-combinator except it adds character x to the buffer");
        console.log(" R is the same as the I-combinator except it flushes the buffer to stdout");
        console.log(" all lowercase letters are null lambdas that can be composed but not applied");
        console.log("");
        console.log("In addition this interpreter supports the following commands")
        console.log(" :q - quit the interpreter");
        console.log(" :s - turn on stepping, while stepping press x to stop or any key to continue");
        console.log(" :r - turn off stepping");
        console.log(" :d <symbol> <value> - define a macro, macro symbols are either single upper-case characters or multiple upper case characters in <>")
        console.log("\t\t macros get completely expanded pre-execution");
        console.log(" :i <symbol> - display the value for the macro <symbol>")
        console.log(" :l <filename> - load <filename>.ski and run all lines that don't begin with a #");
        console.log(" :m <number> - set the max iteration number, 0 for infinity");
        console.log(" :c - clear the console");
        console.log(" :h - display this help");
    } else {
        execute(expand(line));
    }
}

{
    let input:string;
    while((input = readlineSync.prompt()) || input == "") {
        if (input == ":q") break;
        read(input);
    }
}

console.log("Goodbye!");