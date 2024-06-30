# C-- Compiler by Rylan Casanova and Emmy Voita [CST-405]
## Instructions to Run Compiler (on Linux VM):
1. Download ZIP
2. Extract .zip folder and move to your desired Linux directory
3. Navigate to the directory in Linux console using 'cd <directory-name>' command
4. If you would like to modify the test program, it can be found in the InputCode folder as 'testProg.cmm'
5. In the console, execute the following in order:
- make clean
- make insert_included_files
- make parser
6. The compiler will ask if you would like view copied symbol table entries, enter yes or no (y/n)
7. To find the MIPS Assembly code that is generated from the compiler, navigate to the OutputFiles folder and find MipsScript.s

## Instructions to Execute Generated MIPS Assembly Code (in QtSpim):
1. Launch QtSpim
2. Go to File > Reinitialize and Load File
3. Find and select the MipsScript.s file in the OutputFiles folder of your Linux directory
4. Click the green 'Run/Continue' Play button at the top of QtSpim
5. Program output appears in QtSpim's Console window
