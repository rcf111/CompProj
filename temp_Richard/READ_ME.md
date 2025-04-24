commands: 
make
g++ main.cpp -L. -lMyAnalysis $(root-config --cflags) $(root-config --glibs) -o my_program
./my_program

Beforehand make sure that all the files are in the same folder. The result should be a .png file.