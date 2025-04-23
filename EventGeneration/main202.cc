#include "Pythia8/Pythia.h"
#include "Pythia8/PythiaParallel.h"
#include "iostream"
#include <fstream>
#include <chrono>
#include "TFile.h"  
#include "TTree.h"


using namespace Pythia8;

//==========================================================================

int main() {
  // We run pythia in parallel mode because its faster
  PythiaParallel pythia;

  // // For generating the signal
  // pythia.readString("Print:quiet=on");
  // pythia.readString("WeakSingleBoson:ffbar2gmZ=on");
  // // Force Z bosons only to decay into muons
  // pythia.readString("23:oneChannel = 1 1.0 0 13 -13");
  

/**
 * Possible physics settings for background:
 * Top:gg2ttbar
 * 
 */

  // For generating the background
  pythia.readString("Top:gg2ttbar = on");
  pythia.readString("Top:qqbar2ttbar = on");

  pythia.readString("6:onMode = off");
  pythia.readString("6:onIfMatch = 24 5");

  pythia.readString("24:onMode = off");
  pythia.readString("24:onIfMatch = -13 14");
  pythia.readString("-24:onMode = off");
  pythia.readString("-24:onIfMatch = 13 -14");

// Optional: Turn off hadronization if you want partons only
// pythia.readString("HadronLevel:all = off");

  pythia.readString("PhaseSpace:mHatMin = 80.");
  pythia.readString("Beams:eCM = 13600");

  if(!pythia.init()) return 1;

  std::ofstream pT_out("pT.dat");
  std::ofstream rap_out("rap.dat");

  //The following is for measuring runtime and seeing how parallelisation 
  // effects it
  // start stop watch
  auto start = std::chrono::high_resolution_clock::now();
  
  //Paralllised version runtime for 15000 events; 42 s
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


  // // Open data files
  std::ifstream pT_dataFile("pT.dat");
  std::ifstream rap_dataFile("rap.dat");

  // Create and save the ROOT file
  //TFile* data_file = new TFile("signal.root", "RECREATE");
  TFile* data_file = new TFile("background.root", "RECREATE"); 
  TTree* tree = new TTree("dataTree", "Data from ASCII file");

  double pT, eta;
  tree->Branch("pT", &pT, "pT/D");
  tree->Branch("eta", &eta, "eta/D");

  int count = 0;
  while (pT_dataFile >> pT && rap_dataFile >> eta) {
    tree->Fill();
    count++;
  }

  tree->Write();
  data_file->Close();

  pT_dataFile.close();
  rap_dataFile.close();

  return 0;
}
