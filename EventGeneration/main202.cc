#include "Pythia8/Pythia.h"
#include "Pythia8/PythiaParallel.h"
#include "iostream"
#include <fstream>
#include <chrono>

using namespace Pythia8;

//==========================================================================

int main() {
  //Choose which one you want to use, needs to be compatible with what version you choose below
  //Pythia pythia;
  PythiaParallel pythia;

  pythia.readString("Print:quiet=on");
  pythia.readString("SoftQCD:nonDiffractive = on");
  pythia.readString("Beams:eCM = 13600");
  if(!pythia.init()) return 1;

  std::ofstream pT_out("pT.dat");
  std::ofstream rap_out("rap.dat");

  //The following is for measuring runtime and seeing how parallelisation 
  // effects it
  // start stop watch
  auto start = std::chrono::high_resolution_clock::now();
  
  // Unparallelised version runtime for 15000 events: 42.5 s
  // for(int iEvent=0; iEvent<15000; iEvent++){
  //   if(!pythia.next()) continue;

  //   for(int iPart = 0; iPart < pythia.event.size(); iPart++){
  //     if(pythia.event[iPart].id() == 13){
  //       pT_out << pythia.event[iPart].pT() << endl;
  //       rap_out << pythia.event[iPart].eta() << endl;
  //     }
  //   }
  //   int steps = iEvent % 1000;
  //   if(steps==0){
  //     std::cout << "Event number " << iEvent << " has been processed" << std::endl;
  //   }
  // }

  //Paralllised version runtime for 15000 events; 13.5 s
  pythia.run(15000, [&](Pythia* pythiaPtr){
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
