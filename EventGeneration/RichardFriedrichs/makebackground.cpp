#include "Pythia8/Pythia.h"
#include "TFile.h"
#include "TTree.h"
#include "TLorentzVector.h"
#include <vector>

using namespace Pythia8;

int main() {
    // Initialize PYTHIA
    Pythia pythia;
    pythia.readString("Top:gg2ttbar = on");
    pythia.readString("Top:qqbar2ttbar = on");

    pythia.readString("6:onMode = off");
    pythia.readString("6:onIfMatch = 24 5");

    pythia.readString("24:onMode = off");
    pythia.readString("24:onIfMatch = -13 14");
    pythia.readString("-24:onMode = off");
    pythia.readString("-24:onIfMatch = 13 -14");
    pythia.readString("PhaseSpace:mHatMin = 75.");
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
    tree->Branch("px", &px);
    tree->Branch("py", &py);
    tree->Branch("pz", &pz);
    tree->Branch("e", &e);
    tree->Branch("Pt", &Pt);
    tree->Branch("Eta", &Eta);
    tree->Branch("Theta", &Theta);
    tree->Branch("Phi", &Phi);
    tree->Branch("Id", &Id);

    // Event loop
    const int nEvents = 50000;
    for (int iEvent = 0; iEvent < nEvents; ++iEvent) {
        if (!pythia.next()) continue;
        ++passedEvents;

        // Clear previous event data
        Pt.clear(); Eta.clear(); Theta.clear(); Phi.clear(); Id.clear(); px.clear(); py.clear(); pz.clear(); e.clear();
        int good_muons = 0;
        for (int i = 0; i < pythia.event.size(); ++i) {
            const Particle& p = pythia.event[i];
            if (!p.isFinal()) continue;

            int id = p.id();
            TLorentzVector vec(p.px(), p.py(), p.pz(), p.e());
            
            if (abs(id) == 13 or abs(id) == 211) {  // muons and charged pions
                px.push_back(p.px());
                py.push_back(p.py());
                pz.push_back(p.pz());
                e.push_back(p.e());
                Pt.push_back(p.pT());
                Eta.push_back(p.eta());
                Theta.push_back(p.theta());
                Phi.push_back(p.phi());
                Id.push_back(id);
                if (abs(id) == 13 and abs(vec.Eta()) < 2.1 and vec.Pt() > 20) {  //setting up trigger
                    good_muons++;
            }
            }
        }
        if (good_muons ==2){
            HLT_DoubleIsoMu20_eta2p1 = 1;
        }
        else HLT_DoubleIsoMu20_eta2p1 = 0;
        tree->Fill();
    }

    // Write and clean up
    outFile->cd();
    tree->Write("Events", TObject::kOverwrite);
    outFile->Close();
    std::cout << "Events passed pythia.next(): " << passedEvents << "\n";
    return 0;
}
