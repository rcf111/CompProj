#include "Pythia8/Pythia.h"
#include "iostream"
#include <fstream>

using namespace Pythia8;

//==========================================================================

int main() {
  Pythia pythia;

  pythia.readString("Print:quiet=on");
  pythia.readString("SoftQCD:nonDiffractive = on");
  pythia.readString("Beams:eCM = 13600");
  pythia.init();

  std::ofstream pT_out("pT.dat");
  std::ofstream rap_out("rap.dat");

  for(int iEvent=0; iEvent<100000; iEvent++){
    if(!pythia.next()) continue;

    for(int iPart = 0; iPart < pythia.event.size(); iPart++){
      if(pythia.event[iPart].id() == 13){
        pT_out << pythia.event[iPart].pT() << endl;
        rap_out << pythia.event[iPart].eta() << endl;
      }
    }
    int steps = iEvent % 1000;
    if(steps==0){
      std::cout << "Event number " << iEvent << " has been processed" << std::endl;
    }
  }

  std::cout << "DONE!" << std::endl;
  // Done.
  return 0;
}
