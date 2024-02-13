# CMIN: Crashing Input Minimizer(Delta Debugging)

When running multiple test inputs to find a bug in your program, it can be difficult to tell exactly which part of the input caused the error if the input that caused the error is very large.<br>
The Crashing Input Minimizer uses the Delta Debugging Algorithm to extract the parts of a large input value that cause specific errors. This program can be used in the process of fuzzing.<br><br>

## How to build?
You can use Makefile to build the program.
```bash
make
```
This command will make executable file named 'cmin'<br><br>
If you want to see details of Delta Debugging, use ```make debug```.

## How to run?
CMIN receives command-line arguments as following foramat.
```bash
./cmin -i [input_file] -m [target_error_msg] -o [output_file] [executable file]
```
- example
```bash
./cmin -i received -m "SEGV" -o reduced ./a.out
```
After cmin is run with these options, a file named reduced is created.
This file is a minified version of the input file and raises the target error given by the -m option.<br>
If you need options for the target program, you can enter them in the [executable_file] part after the executable name.

## If something goes wrong
## Requirements