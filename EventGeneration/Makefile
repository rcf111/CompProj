ROOTINC := $(shell root-config --cflags)
ROOTLIBS := $(shell root-config --glibs)

PYTHIAINC = -I/home/jan/Documents/00_Master/Computing_Methods_in_HEP/pythia/pythia8313/include/
PYTHIALIB = -L/home/jan/Documents/00_Master/Computing_Methods_in_HEP/pythia/pythia8313/lib/


SRC1 = $(wildcard *.cc)
OUT1 = $(basename $(SRC1))
SRC2 = $(wildcard *.cxx)
OUT2 = $(basename $(SRC2))
CXXFLAGS = $(PYTHIAINC) $(ROOTINC)
LDFLAGS = $(PYTHIALIB) $(ROOTLIBS) -L/usr/lib/x86_64-linux-gnu/ -ltbb


GARBAGE = $(wildcard *.o) $(wildcard *.dat) $(OUT1) $(OUT2) 

all: $(OUT1) $(OUT2)

$(OUT1): $(SRC1)
	$(CXX)  $(ROOTINC) $(PYTHIAINC) $(SRC1) $(ROOTLIBS) $(PYTHIALIB) -lpythia8 -o $(OUT1)

compile_plot $(OUT2): $(SRC2)
	$(CXX) $(ROOTINC) $(SRC2) $(ROOTLIBS) -o $(OUT2)

run: $(OUT1)
	LD_LIBRARY_PATH=/home/jan/Documents/00_Master/Computing_Methods_in_HEP/pythia/pythia8313/lib ./main202

runPlot: $(OUT2)
	./plot

clean: $(GARBAGE)
	rm -f $(GARBAGE)



