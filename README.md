# Asmtoy
Asmtoy is a Just A Toy Assembly-style language interpreter for personal practice.

### Variables Declaration and Assignment
1. **`show “sad”`**: Print `"sad"`, following the escape character rules (for example, `"\n"`).
2. **`num axx`**: Declare a `num` variable named `axx`, with a default value of `0`.
3. **`assign a 10`**: Assign the value `10` to `a` (provided that `a` has been declared and is of type `num` only).
4. **`assign a b`**: Assign the value of variable `b` to `a` (provided that both `b` and `a` are of type `num`).
5. **`inputnum a`**: Get the user's input number and store it in `a` (provided that `a` has been declared).
6. **`str b`**: Declare a string variable `b`.

### Control Flow Statements
1. **`goto x`**: Jump the reading pointer to the place where the label is `x`, where `x` is a `str` variable (it can be combined with `if` to implement loops).
2. **`goto ”asd“`**: Jump the reading pointer to the place where the label is `"asd"` (note that the label can be either above or below in the code).
3. **`label ”asd “`**: Here the label is `" asd"` (it is prohibited to assign a variable to a label, as it may lead to unpredictable consequences).

### String Manipulation
1. **`cpystr b “xxxxx”`**: Assign `"xxxxx"` to `b`.
2. **`cpystr b c`**: Assign the value of `c` to `b` (provided that both `b` and `c` are strings).

### Output Statements
**`show b`**: Print the variable `b`, where `b` can be either of type `num` or `str`.

### Conditional Statements
**`if a x`**: If `a` is equal to `x` (only for variables of type `num`), read the next line; otherwise, read the line after the next one.

### Arithmetic Operations
1. **`add a 1 3`**: Equivalent to `a = 1 + 3` in C++ (only for variables of type `num`).
2. **`add a a 1`**: Equivalent to `a++` in C++ (only for variables of type `num`).
3. **`add a a b`**: Equivalent to `a += b` in C++ (only for variables of type `num`).
Similar operators also include `sub` (subtraction), `div` (division), and `mul` (multiplication).

### Function Definition and Calling
1. **Function Definition**: The following syntax is used to define a parameterless function. The following defines a function named `test` without parameters:
```
proc test
| add a a b
| if a 10
| return
| add a a b
```
In the function body, `"| "` indicates that it is inside the function body, and `return` is used to exit the function body; otherwise, the entire function body will be executed.
2. **`call test`**: Call the function `test`. The order of `call` and `proc` doesn't matter. 
