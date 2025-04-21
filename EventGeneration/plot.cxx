#include <iostream>
#include <fstream>
#include <vector>
#include "TH1F.h"
#include "TCanvas.h"
#include "TApplication.h"
#include "TFile.h"  
#include "TTree.h"




void plot() {
    // Open data files
    std::ifstream pT_dataFile("pT.dat");
    std::ifstream rap_dataFile("rap.dat");

    // Create and save the ROOT file
    TFile* data_file = new TFile("data.root", "RECREATE");
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

    // Reopen the ROOT file and retrieve the tree
    data_file = new TFile("data.root", "READ");
    tree = (TTree*)data_file->Get("dataTree");

    double eta_test;
    double pT_test;
    tree->SetBranchAddress("eta", &eta_test);
    tree->SetBranchAddress("pT", &pT_test);
    tree->SetBranchStatus("eta", true); 
    tree->SetBranchStatus("pT", true);  
    

    int totalMuons = tree->GetEntries();
    int detectedMuons = 0;
    for (int iEntry = 0; tree->LoadTree(iEntry) >= 0; ++iEntry) {
    // Load the data for the given tree entry
        tree->GetEntry(iEntry);
        if (pT_test > 5.0 && fabs(eta_test) < 2.5) {
            detectedMuons++;
            }
    }
    tree->SetBranchStatus("*", true);

    std::cout << detectedMuons<< std::endl;
    double detectionProbability = detectedMuons*1. / totalMuons*1.;
    std::cout << "Muon detection probability: " << detectionProbability << std::endl;



    // Create a canvas
    TCanvas* canvas = new TCanvas("canvas", "Muon minimum bias events", 1000, 1000);
    canvas->Divide(1, 2);
    canvas->SetFillColor(kWhite);

    // Create histograms
    TH1F* eta_hist = new TH1F("eta", "Pseudorapidity", 50, -11, 11);
    TH1F* pT_hist = new TH1F("pT", "Transverse Momentum", 50, 0, 7);


    double pT1, eta1;
    tree->SetBranchAddress("pT", &pT1);
    tree->SetBranchAddress("eta", &eta1);

    // Fill histograms
    int nentries = tree->GetEntries();
    for (int i = 0; i < nentries; i++) {
        tree->GetEntry(i);
        eta_hist->Fill(eta1);
        pT_hist->Fill(pT1);
    }

    // Style histograms
    eta_hist->SetLineColor(kRed);
    eta_hist->SetLineWidth(2);
    eta_hist->SetFillColor(kYellow);
    eta_hist->GetXaxis()->SetTitle("Pseudorapidity (eta)");
    eta_hist->GetYaxis()->SetTitle("Counts");
    
    pT_hist->SetLineColor(kBlue);
    pT_hist->SetLineWidth(2);
    pT_hist->SetFillColor(kCyan);
    pT_hist->GetXaxis()->SetTitle("Transverse Momentum in GeV (pT)");
    pT_hist->GetYaxis()->SetTitle("Counts");

    // Draw histograms
    canvas->cd(1);
    eta_hist->Draw();
    
    canvas->cd(2); 
    pT_hist->Draw();

    // Update canvas
    canvas->Update();
}

int main(int argc, char** argv) {
    TApplication app("app", &argc, argv);
    plot();
    app.Run();
    return 0;
}
