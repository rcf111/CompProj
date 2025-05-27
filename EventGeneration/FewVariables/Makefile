PYTHIA8_URL = https://pythia.org/download/pythia83/pythia8313.tgz
PYTHIA8_TGZ = $(notdir $(PYTHIA8_URL))
#PYTHIA8_DIR = $(basename $(PYTHIA8_TGZ))

PYTHIA8_DIR = /home/jan/Documents/00_Master/Computing_Methods_in_HEP/pythia/pythia8313


# pythia8*

GARBAGE = $(wildcard *~ *.o *.a *.so xsec *.job *.pdf *.C *.dat *.exe *.tgz) makebackground makesignal analysis

ROOTCFLAGS = $(shell root-config --cflags)
ROOTLIBS   = $(shell root-config --libs)


OPT = -O2 -std=c++11 -pedantic -W -Wall -Wshadow -fPIC
INC = -I$(PYTHIA8_DIR)/include -I$(ROOTSYS)/include
LIBS = -L$(PYTHIA8_DIR)/lib -lpythia8 -ldl

OBJ = $(addsuffix .o,$(basename $(wildcard *.cpp)))

all:
	$(MAKE) compile
	$(MAKE) makesignal
	$(MAKE) makebackground
	$(MAKE) analysis
	$(MAKE) job
	./test.job


.cpp.o:
	$(CXX) $(OPT) $(ROOTCFLAGS) $(INC) -c $< -o $@


compile: $(OBJ)


makesignal: makesignal.o
	$(CXX) $(OPT) $(ROOTCFLAGS) $(INC) makesignal.o $(ROOTLIBS) $(LIBS) -o makesignal

makebackground: makebackground.o
	$(CXX) $(OPT) $(ROOTCFLAGS) $(INC) makebackground.o $(ROOTLIBS) $(LIBS) -o makebackground

analysis: analysis.o
	$(CXX) $(OPT) $(ROOTCFLAGS) $(INC) analysis.o $(ROOTLIBS) $(LIBS) -o analysis

job:
	echo '#!'`which bash` > test.job 
	echo 'export LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(PYTHIA8_DIR)/lib' >> test.job
	echo 'export PYTHIA8DATA=$(PWD)/pythia8313/share/Pythia8/xmldoc' >> test.job
	echo './makesignal' >> test.job
	echo './makebackground' >> test.job
	chmod +x test.job

clean:
	rm -rf $(GARBAGE)

