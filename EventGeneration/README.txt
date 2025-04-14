You need to change the path to pythia in the makefile according to where it is saved on your system, since I hardcoded the paths into the makefile. Perhaps also the path for the root libraries
might need adjustment.
Then you can do:

1. compile everything:
	make

2. run event generator:
	make run

3. compile plot.cxx:
	make compile_plot
I added this so one can work on the plotting separately without having to simulate the events again

4. run plot executable:
	make runPlot

5. delete all files except necessary ones:
	make clean 

