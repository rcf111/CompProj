#include "Pythia8/Pythia.h"
#include "Pythia8/PythiaParallel.h"
#include "iostream"
#include <fstream>
#include <chrono>


using namespace Pythia8;

//==========================================================================

int main() {
  // We run pythia in parallel mode because its faster
  PythiaParallel pythia;

  pythia.readString("Print:quiet=on");
  pythia.readString("SoftQCD:nonDiffractive = on");
  pythia.readString("Beams:eCM = 13600");

  //Force Z bosons only to decay into muons
  pythia.readString("23:oneChannel = 1 1.0 0 13 -13");

  if(!pythia.init()) return 1;

  std::ofstream pT_out("pT.dat");
  std::ofstream rap_out("rap.dat");

  //The following is for measuring runtime and seeing how parallelisation 
  // effects it
  // start stop watch
  auto start = std::chrono::high_resolution_clock::now();
  
  //Paralllised version runtime for 15000 events; 13.5 s
  pythia.run(1000000, [&](Pythia* pythiaPtr){
    for(int iPart = 0; iPart < pythiaPtr->event.size(); iPart++){
      if(pythiaPtr->event[iPart].id() == 13){
        pT_out << pythiaPtr->event[iPart].pT() << endl;
        rap_out << pythiaPtr->event[iPart].eta() << endl;
      }
    }
  });
  // stop stopwatch
  auto end = std::chrono::high_resolution_clock::now();

  // Calculate and print the elapsed time
  std::chrono::duration<double> elapsed = end - start;
  std::cout << "Elapsed time: " << elapsed.count() << " seconds\n";

  std::cout << "DONE!" << std::endl;
  // Done.

  return 0;
}
