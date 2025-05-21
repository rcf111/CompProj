#include <iostream>
#include <vector>
#include <string>
#include <cmath> 
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TLorentzVector.h"
#include "TF1.h"
#include "TMath.h"
#include "ROOT/RVec.hxx"


// NOTE: Update these before running
const std::string treeName = "Events";    // <-- Replace with actual TTree name
const std::vector<std::string> inputFiles = {
    "Isolated_Stricterfiltered_Smeared_filtered_Signal.root",  // <-- Replace with actual files
    "Isolated_Stricterfiltered_Smeared_filtered_Background.root"
};
const std::vector<double> scaleFactors = {
    0.250,   // <-- scale for signal file (See read.me to see how I got them)
    0.004   // <-- scale for background file
};

// Mass range for histogram and fit
const double massMin = 60.0;
const double massMax = 120.0;
const int histBins = 100;

// Signal mass window for integration (around Z boson ~91.2 GeV)
const double signalMassCenter = 91.2;
const double signalMassWindow = 10.0; // +/- 10 GeV window, adjust as needed

// Function prototypes
void buildHistogram(TH1F* hist);
double integrateFunction(TF1* func, double low, double high);

int main() {
    TH1F* combinedHist = new TH1F("combinedHist", "Dimuon Invariant Mass;Mass (GeV);Events", histBins, massMin, massMax);

    for (size_t i = 0; i < inputFiles.size(); ++i) {
        TFile* file = TFile::Open(inputFiles[i].c_str());
        if (!file || file->IsZombie()) {
            std::cerr << "Error opening file: " << inputFiles[i] << std::endl;
            continue;
        }
        TTree* tree = (TTree*)file->Get(treeName.c_str());
        if (!tree) {
            std::cerr << "TTree named '" << treeName << "' not found in file " << inputFiles[i] << std::endl;
            file->Close();
            continue;
        }

        // Create a temporary histogram for this file
        TH1F* tempHist = new TH1F(("hist_" + std::to_string(i)).c_str(),
                                  ("Dimuon Mass from file " + std::to_string(i)).c_str(),
                                  histBins, massMin, massMax);

        // Set up branches (assuming branches: pT, eta, phi are arrays or std::vector<double>)
        // Modify these types if your tree has arrays or different types
        // std::vector<float>* Pt = nullptr;
        // std::vector<float>* Theta = nullptr;
        // std::vector<float>* Phi = nullptr;

        ROOT::VecOps::RVec<float>* Pt = nullptr;
        ROOT::VecOps::RVec<float>* Theta = nullptr;
        ROOT::VecOps::RVec<float>* Phi = nullptr;

        tree->SetBranchAddress("Pt", &Pt);
        tree->SetBranchAddress("Theta", &Theta);
        tree->SetBranchAddress("Phi", &Phi);

        Long64_t nEntries = tree->GetEntries();
        
        


        for (Long64_t entry = 0; entry < nEntries; ++entry) {
            tree->GetEntry(entry);

            if (!Pt || !Theta || !Phi) {
                std::cerr << "Branch pointers are null!" << std::endl;
                continue;
            }

            if (Pt->size() != Theta->size() || Pt->size() != Phi->size()) {
                std::cerr << "Mismatched vector sizes in event " << entry << std::endl;
                continue;
            }

            // Compute Eta for this event
            ROOT::VecOps::RVec<float> Eta;
            Eta.reserve(Theta->size());
            for (float theta : *Theta) {
                float eta = -std::log(std::tan(theta / 2.0f));
                Eta.push_back(eta);
            }

            // Collect muons in this event
            std::vector<TLorentzVector> muons;

            for (size_t idx = 0; idx < Pt->size(); ++idx) {
                // Build 4-vector (mass assumed 0.105 GeV for muon)
                double mass_mu = 0.105;
                double pt = Pt->at(idx);
                double Eta_val = Eta.at(idx);
                double Phi_val = Phi->at(idx);
                TLorentzVector p4;
                p4.SetPtEtaPhiM(pt, Eta_val, Phi_val, mass_mu);
                muons.push_back(p4);
            }

            // Require at least two muons
            if (muons.size() < 2) continue;

            // Use last two muons to calculate invariant mass (as in your original code)
            TLorentzVector dimuon = muons[muons.size() - 1] + muons[muons.size() - 2];
            double mass = dimuon.M();

            tempHist->Fill(mass);
        }
        
        // Scale histogram and add to combined
        tempHist->Scale(scaleFactors[i]);
        combinedHist->Add(tempHist);
        TCanvas* c = new TCanvas("c", "Dimuon Mass Fit", 800, 600);
        c->SetLogy();
        combinedHist->Draw();
        c->SaveAs("dimuon_mass.png");
        file->Close();
        //delete tempHist; This line causes a segmentation fault if active, I don't know why
    }

    // Fit combined histogram in the mass range
    // Define signal+background function:
    // For example: background = polynomial, signal = Gaussian (adjust as needed)
    TF1* fitFunc = new TF1("fitFunc", 
        "[0]*TMath::Gaus(x,[1],[2],true) + [3] + [4]*x + [5]*x*x", massMin, massMax);

    // Initial parameter guesses:
    fitFunc->SetParameters(
        combinedHist->GetMaximum(),  // Gaussian amplitude guess
        signalMassCenter,            // Mean near Z mass
        2.5,                        // Width guess (GeV)
        10,                         // Background poly constant term
        0,                          // Background linear term
        0                           // Background quadratic term
    );

    combinedHist->Fit(fitFunc, "R");

    // Background-only function (polynomial part only)
    TF1* bgFunc = new TF1("bgFunc", "[0] + [1]*x + [2]*x*x", massMin, massMax);
    bgFunc->SetParameters(fitFunc->GetParameter(3),
                         fitFunc->GetParameter(4),
                         fitFunc->GetParameter(5));

    // Define integration window around signal peak
    double lowBound = signalMassCenter - signalMassWindow;
    double highBound = signalMassCenter + signalMassWindow;

    double totalIntegral = fitFunc->Integral(lowBound, highBound) / combinedHist->GetBinWidth(1);
    double bgIntegral = bgFunc->Integral(lowBound, highBound) / combinedHist->GetBinWidth(1);
    double signalIntegral = totalIntegral - bgIntegral;

    // Statistical significance (naive)
    double significance = signalIntegral / std::sqrt(bgIntegral);

    // Output results
    std::cout << "Integration window: [" << lowBound << ", " << highBound << "] GeV" << std::endl;
    std::cout << "Total events in window (signal+background): " << totalIntegral << std::endl;
    std::cout << "Background events in window: " << bgIntegral << std::endl;
    std::cout << "Signal events in window (total - background): " << signalIntegral << std::endl;
    std::cout << "Estimated significance (S / sqrt(B)): " << significance << std::endl;

    // Save plot with fit
    TCanvas* c = new TCanvas("c", "Dimuon Mass Fit", 800, 600);
    combinedHist->Draw();
    c->SetLogy();
    fitFunc->Draw("same");
    bgFunc->SetLineColor(kRed);
    bgFunc->SetLineStyle(2);
    bgFunc->Draw("same");
    c->SaveAs("dimuon_mass_fit.png");

    delete combinedHist;
    delete fitFunc;
    delete bgFunc;
    delete c;
    

    return 0;
}
