#include "Pythia8/PythiaParallel.h"
#include "TFile.h"
#include "TTree.h"
#include "TLorentzVector.h"
#include <vector>

using namespace Pythia8;

int main() {
    // Initialize PYTHIA

    PythiaParallel pythia;

    pythia.readString("Top:gg2ttbar = on");
    pythia.readString("Top:qqbar2ttbar = on");
    pythia.readString("24:onMode = off");
    pythia.readString("24:onIfMatch = -13 14");
    pythia.readString("-24:onMode = off");
    pythia.readString("-24:onIfMatch = 13 -14");
    pythia.readString("PhaseSpace:mHatMin = 60.");
    pythia.readString("Beams:eCM = 13600");
    
    pythia.init();
    int passedEvents=0;

    // Output ROOT file and tree
    TFile* outFile = new TFile("Background.root", "RECREATE");
    TTree* tree = new TTree("Events", "Data");

    // Declare variables and branches
    std::vector<float> px, py, pz, e;


    std::vector<float> Pt, Eta, Theta, Phi;
    std::vector<int> Id;


    bool HLT_DoubleIsoMu20_eta2p1;
    tree->Branch("HLT_DoubleIsoMu20_eta2p1", &HLT_DoubleIsoMu20_eta2p1);
    tree->Branch("Pt", &Pt);
    tree->Branch("Theta", &Theta);
    tree->Branch("Phi", &Phi);
    tree->Branch("Id", &Id);


    pythia.run(1000000, [&](Pythia* pythiaPtr){
        ++passedEvents;
        Pt.clear(); Theta.clear(); Phi.clear(); Id.clear();
        int good_muons = 0;
        
        for(int iPart = 0; iPart < pythiaPtr->event.size(); iPart++){
            const Particle& p = pythiaPtr->event[iPart];
            if (!p.isFinal()) continue;

            int id = p.id();
            TLorentzVector vec(p.px(), p.py(), p.pz(), p.e());
            
            if (abs(id) == 13 or abs(id) == 211) {  // muons and charged pions
                Pt.push_back(p.pT());
                Theta.push_back(p.theta());
                Phi.push_back(p.phi());
                Id.push_back(id);
                if (abs(id) == 13 and abs(vec.Eta()) < 2.1 and vec.Pt() > 20) {  //setting up trigger
                    good_muons++;
            }
            }
        }
        if (good_muons >=2){
            HLT_DoubleIsoMu20_eta2p1 = 1;
        }
        else HLT_DoubleIsoMu20_eta2p1 = 0;
        tree->Fill();
  });

  
    // // Event loop
    // const int nEvents = 10000;
    // for (int iEvent = 0; iEvent < nEvents; ++iEvent) {
    //     if (!pythia.next()) continue;
    //     ++passedEvents;

    //     // Clear previous event data
    //     Pt.clear();  Theta.clear(); Phi.clear(); Id.clear();
    //     int good_muons = 0;
    //     for (int i = 0; i < pythia.event.size(); ++i) {
    //         const Particle& p = pythia.event[i];
    //         if (!p.isFinal()) continue;

    //         int id = p.id();
    //         TLorentzVector vec(p.px(), p.py(), p.pz(), p.e());
            
    //         if (abs(id) == 13 or abs(id) == 211) {  // muons and charged pions
    //             Pt.push_back(p.pT());
    //             Theta.push_back(p.theta());
    //             Phi.push_back(p.phi());
    //             Id.push_back(id);
    //             if (abs(id) == 13 and abs(vec.Eta()) < 2.1 and vec.Pt() > 20) {  //setting up trigger
    //                 good_muons++;
    //         }
    //         }
    //     }
    //     if (good_muons >= 2){
    //         HLT_DoubleIsoMu20_eta2p1 = 1;
    //     }
    //     else HLT_DoubleIsoMu20_eta2p1 = 0;
    //     tree->Fill();
    // }

    // Write and clean up
    outFile->cd();
    tree->Write("Events", TObject::kOverwrite);
    outFile->Close();
    std::cout << "Events passed pythia.next(): " << passedEvents << "\n";


    double sigma = pythia.sigmaGen(); // cross section in mb
    std::cout << "Generated cross section: " << sigma * 1e9 << " pb" << std::endl;
  return 0;
}
