PYTHIA8_URL = https://pythia.org/download/pythia83/pythia8313.tgz
PYTHIA8_TGZ = $(notdir $(PYTHIA8_URL))
PYTHIA8_DIR = $(basename $(PYTHIA8_TGZ))

# pythia8*

GARBAGE = $(wildcard *~ *.o *.a *.so xsec *.job *.png *.pdf *.C *.dat *.exe *.tgz)

ROOTCFLAGS = $(shell root-config --cflags)
ROOTLIBS   = $(shell root-config --libs)


OPT = -O2 -std=c++11 -pedantic -W -Wall -Wshadow -fPIC
INC = -I$(PYTHIA8_DIR)/include -I$(ROOTSYS)/include
LIBS = -L$(PYTHIA8_DIR)/lib -lpythia8 -ldl

OBJ = $(addsuffix .o,$(basename $(wildcard *.cpp)))

all:
	$(MAKE) p8get
	$(MAKE) compile
	$(MAKE) makesignal
	$(MAKE) makebackground
	$(MAKE) job
	./test.job

p8get:
	wget $(PYTHIA8_URL) && tar xfvz $(PYTHIA8_TGZ) && cd $(PYTHIA8_DIR) && ./configure && make

.cpp.o:
	$(CXX) $(OPT) $(ROOTCFLAGS) $(INC) -c $< -o $@




compile: $(OBJ)


makesignal: makesignal.o
	$(CXX) $(OPT) $(INC) $(ROOTCFLAGS) makesignal.o $(LIBS) $(ROOTLIBS) -o makesignal

makebackground: makebackground.o
	$(CXX) $(OPT) $(INC) $(ROOTCFLAGS) makebackground.o $(LIBS) $(ROOTLIBS) -o makebackground

job:
	echo '#!'`which bash` > test.job 
	echo 'export LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(PYTHIA8_DIR)/lib' >> test.job
	echo 'export PYTHIA8DATA=$(PWD)/pythia8313/share/Pythia8/xmldoc' >> test.job
	echo './makesignal' >> test.job
	echo './makebackground' >> test.job
	chmod +x test.job

clean:
	rm -rf $(GARBAGE)

