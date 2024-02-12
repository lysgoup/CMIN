# CMIN: Crashing Input Minimizer(Delta Debugging)

When running multiple test inputs to find a bug in your program, it can be difficult to tell exactly which part of the input caused the error if the input that caused the error is very large.<br>
The Crashing Input Minimizer uses the Delta Debugging Algorithm to extract the parts of a large input value that cause specific errors. This program can be used in the process of fuzzing.<br><br>

## How to build?
You can use Makefile to build the program.
```c
make
```
This command will make executable file named 'cmin'<br><br>
If you want to see details of Delta Debugging, use ```make debug```.

## How to run?