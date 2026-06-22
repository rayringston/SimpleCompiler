# Simple Compiler

This is a simple compiler written in C++, to compile a pseudocode-type language into ARM64 instructions.
Tested using WSL running Ubuntu, using GNU's assembler and linker to create machine code. In the future, these too will be integrated into the simple compiler.

# How to Use
Basically you don't. This is meant to be run using Linux. Hypothetically it should also work using a different assembler and linker, but as of now it only uses the GNU toolchain for 64-bit ARM processors. Having this type of processor isn't actually necessary because of the toolchain, it'll kinda just cross compile to whatever architecture you have.

So, to run this you first need the Aarch64 Linux GNU tool chain:

```console
sudo apt-get update
sudo apt-get install binutils-aarch64-linux-gnu
```

Then go to the root directory of wherever you stored this repo, and run the build script:
```console
./build.sh test/test.txt
```

You can replace ```test/test.txt``` with the path to whatever file you want to compile.


# Features
## Data Types
This programming language, unnamed as of now, features 2 primitive types TEXT and INTs. These represent strings, and whole numbers, including ASCII characters respectively. Variables can be defined with identifiers, the same way as in other programming languages. TEXT variables can be defined using string literals, or using some other string operation. Floating point values and compound structures like arrays and objects have not yet been implemented.

All these can be printed using the built-in PRINT statement. Both INTs and TEXT can be printed with the PRINT statement, and INT values that are acceptable ASCII characters (including character literals) can be printed using PUTCHAR.

## Flow Control
The primary method to control the execution of the program, is by using IF and ELSE statements. IF statements must be followed by a conditional expression (must include an comparison operator), followed by a THEN token. There are not yet features for brackets or indentation, so blocks use certain keywords to indicate where they start. The body of the IF statement must be on the line following the last THEN keyword. After these statements, there must be an ENDIF token, or and ELSE block followed by the ELSEIF

Another method is using the WHILE keyword. The WHILE statement is very similar to the IF, it must be followed by a condition, and then the DO keyword. Then is the body of the WHILE block, followed by an ENDWHILE keyword to signify the end of the block. Infinite loops are very possible in the loops, so don't mess up.

Finally, there are GOTOs and LABELs. These function nearly identically to branches in the assembly version. Any LABEL is a label in the compiled code, and any GOTO is an unconditional branch to the specified label.

## Built-in Functions
These are the cool parts. The built-in functions are heavily dependent on the runtime library. Most built-ins are based heavily on their C counterparts, including LEN, SUBSTR, CHARAT, INPUT, and INTTOTEXT.

- LEN
  - The LEN built-in takes a TEXT expression as an input and return it's length as an INT
  - [INT] <= LEN( [TEXT] )
- SUBSTR
  - SUBSTR takes a given TEXT value and returns the substring based on the start and stop index
  - [TEXT] <= SUBSTR( [TEXT], [INT], [INT] )
  - The first INT is the start index, inclusive, and second is the stop index, exclusive
  - May cause an exit if the indexes are out of bounds
- CHARAT
  - CHARAT returns the INT value of the character at a certain index in a string
  - [INT] <= CHARAT( [TEXT], [INT] )
  - Might be able to reach past the end of string, so this might be a dangerous function to use.
- INPUT
  - Returns a TEXT value based on the input from the user
  - [TEXT] <= INPUT()
  - The buffer size for the usr_input runtime is 256 bytes, which would allow for a maximum of 256 characters to be inputted at once.
- INTTOTEXT
  - This one just converts the INT parameter into a string of that number
  - [TEXT] <= INTTOTEXT( [INT] )
 
## User-Defined Functions
This area is still kinda a mess. You can define a function using the FUNC keyword, followed by it's names. Optionally this can be followed by a list of parameters and their types. Again the syntax rules here are sitll messy. 

## Memory Allocation
To allow for strings to be defined during runtime, i.e. concatenating two strings together, I needed to add a bump allocator. This allocator is present in the alloc.s runtime. It has a capacity of 8192 bytes or 8 KiB, and is used to store these runtime strings. Anytime additional memory is needed for a variable or expression, the amount of space needed is sent to the allocator, and the address of the avaiable slot is returned. However, any memory that becomes unused, or freed when that is implemented, will just become unusable space with the memory. The solution to this will be the heap. But that might take a while.

---
Made by Ray Ringston (ray.ringston@gmail.com)

Last Updated: 6/17/2026
