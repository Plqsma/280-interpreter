# 📘 SPL Parser and Interpreter (CS280 Project 3)

This project implements a parser and interpreter for a simplified programming language called SPL (Simple Programming Language), designed as part of a compiler construction course. It includes a **lexical analyzer**, **recursive descent parser**, and **expression evaluator**, supporting control structures, declarations, arithmetic expressions, and print statements.

## 🛠️ Features

- Tokenization of SPL source code using a custom lexer.
- Recursive-descent parsing and syntax validation of SPL grammar.
- Evaluation of arithmetic and logical expressions.
- Support for data types: `INTEGER`, `REAL`, `CHARACTER`.
- Control structures: `IF`, `THEN`, `ELSE`, `PRINT`, `PROGRAM`, `END`.
- Expression operators: `+`, `-`, `*`, `/`, `^`, `=`, `>`, `<`, `//` (concat), `::` (type definition).

## 📂 File Structure

| File | Description |
|------|-------------|
| `prog3.cpp` | Entry point: reads a file, invokes parser/interpreter |
| `parserInterp.h/.cpp` | Parser function declarations and implementations |
| `lex.h/.cpp` | Lexical analyzer (tokenizer) |
| `val.h/.cpp` | Value class: type management and operator overloading |
| `test_input.txt` | (You can include this in your repo) Example SPL program to test parsing/execution |

## ▶️ How to Run

1. Compile:
```bash
g++ -std=c++11 prog3.cpp parserInterp.cpp lex.cpp val.cpp -o spl
```

2. Run with an SPL program as input:
```bash
./spl test_input.txt
```

## ✅ Sample Output

```
Successful Execution
```
Or in case of a syntax/semantic error:
```
Unsuccessful Interpretation
Number of Errors: 3
```

## 💡 Example SPL Code (test_input.txt)

```spl
PROGRAM
    INTEGER :: x, y
    x = 10
    y = x * 2
    PRINT x, y
END
```

## 📚 Skills Demonstrated

- Recursive descent parsing
- Tokenization and lexical analysis
- Abstract syntax tree simulation through nested function calls
- Operator overloading in C++
- Managing multiple C++ source/header files
- Error handling and debugging parsers

---

**Short GitHub Description:**  
> Recursive descent parser and interpreter for a custom language (SPL), built in C++ with lexical analysis and expression evaluation.
