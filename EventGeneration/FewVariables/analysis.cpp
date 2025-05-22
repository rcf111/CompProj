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
#include "TLegend.h"


// NOTE: Update these before running
const std::string treeName = "Events";    // <-- Replace with actual TTree name
const std::vector<std::string> inputFiles = {
    "Isolated_Stricterfiltered_Smeared_filtered_Signal.root",  // <-- Replace with actual files
    "Isolated_Stricterfiltered_Smeared_filtered_Background.root"
};
const std::vector<double> scaleFactors = {
    //4.542*0.6,   // <-- scale for signal file (See read.me to see how I got them)
    //0.062 *0.6 // <-- scale for background file
   3.684, 17.401
   //22.82, 0.29
};

// Mass range for histogram and fit
const double massMin = 60.0;
const double massMax = 120.0;
const int histBins = 120;

// Signal mass window for integration (around Z boson ~91.2 GeV)
const double signalMassCenter = 91.2;
const double signalMassWindow = 10.0; // +/- 10 GeV window, adjust as needed

// Function prototypes
void buildHistogram(TH1F* hist);
double integrateFunction(TF1* func, double low, double high);

Double_t doubleCB(Double_t *x, Double_t *par) {
    // par[0] = Norm
    // par[1] = Mean
    // par[2] = Sigma
    // par[3] = AlphaL
    // par[4] = nL
    // par[5] = AlphaR
    // par[6] = nR
    // par[7] = offset

    Double_t xx = x[0];
    Double_t norm = par[0];
    Double_t mean = par[1];
    Double_t sigma = par[2];
    Double_t alphaL = par[3];
    Double_t nL = par[4];
    Double_t alphaR = par[5];
    Double_t nR = par[6];
    Double_t offset = par[7];

    Double_t t = (xx - mean) / sigma;
    Double_t result = 0.0;

    if (t < -alphaL) {
        Double_t A = pow(nL / fabs(alphaL), nL) * exp(-0.5 * alphaL * alphaL);
        Double_t B = nL / fabs(alphaL) - fabs(alphaL);
        result = norm * A * pow(B - t, -nL) + offset;
    }
    else if (t > alphaR) {
        Double_t A = pow(nR / fabs(alphaR), nR) * exp(-0.5 * alphaR * alphaR);
        Double_t B = nR / fabs(alphaR) - fabs(alphaR);
        result = norm * A * pow(B + t, -nR)+ offset;
    }
    else {
        result = norm * exp(-0.5 * t * t)+ offset;
    }
    return result;
    }

int main() {
    TH1F* combinedHist = new TH1F("combinedHist", "Dimuon Invariant Mass;Mass (GeV);Events", histBins, massMin, massMax);
    TH1F* sig_Hist = new TH1F("hist_signal", "Dimuon Mass from signal file ", histBins, massMin, massMax);
    TH1F* bg_Hist = new TH1F("hist_background", "Dimuon Mass from background file ", histBins, massMin, massMax);

    for (size_t i = 0; i < inputFiles.size(); ++i) {
        //Only for debugging
        //if(i ==1){break;}

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


        // Set up branches (assuming branches: pT, eta, phi are arrays or std::vector<double>)
        // Modify these types if your tree has arrays or different types
        // std::vector<float>* Pt = nullptr;
        // std::vector<float>* Theta = nullptr;
        // std::vector<float>* Phi = nullptr;

        ROOT::VecOps::RVec<float>* Pt = nullptr;
        ROOT::VecOps::RVec<float>* Theta = nullptr;
        ROOT::VecOps::RVec<float>* Phi = nullptr;
        ROOT::VecOps::RVec<int>* Id = nullptr;

        tree->SetBranchAddress("Pt", &Pt);
        tree->SetBranchAddress("Theta", &Theta);
        tree->SetBranchAddress("Phi", &Phi);
        tree->SetBranchAddress("Id", &Id);

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
            TLorentzVector p4;

            for (size_t idx = 0; idx < Id->size(); ++idx) {
                // Build 4-vector (mass assumed 0.105 GeV for muon)
                if(Id->at(idx) == 13 or Id->at(idx) == -13){
                    double mass_mu = 0.105;
                    double px = Pt->at(idx) *std::cos(Phi->at(idx));
                    double py = Pt->at(idx) *std::sin(Phi->at(idx));
                    double pz = Pt->at(idx)/std::tan(Theta->at(idx));
                    double p = std::sqrt(px*px + py*py + pz*pz);
                    double e = std::sqrt(p*p + mass_mu*mass_mu);
                    p4.SetPxPyPzE(px,py,pz,e);
                    muons.push_back(p4);
                }
            }

            // Require at least two muons
            if (muons.size() >= 2){

            // Use last two muons to calculate invariant mass (as in your original code)
            double mass = (muons[muons.size()-1] + muons[muons.size()-2]).M();
            //std::cout<< mass <<std::endl;
            if(i==0){
                sig_Hist->Fill(mass);
            } else if(i==1){
                bg_Hist->Fill(mass);
            }
            
            }
        }
       
        // Scale histogram and add to combined
        file->Close();
        //delete tempHist; This line causes a segmentation fault if active, I don't know why
    }

    
    sig_Hist->Scale(scaleFactors[0]);
    bg_Hist->Scale(scaleFactors[1]);
    TCanvas* canvas = new TCanvas("c", "Dimuon Mass Fit", 800, 600);
    canvas->SetLogy();
    combinedHist->Add(sig_Hist);
    combinedHist->Add(bg_Hist);
    combinedHist->SetMinimum(75);
    combinedHist->SetLineColor(kRed);
    sig_Hist->SetLineColor(kBlue);
    bg_Hist->SetLineColor(kGreen);
    combinedHist->Draw();
    sig_Hist->Draw("Same");
    bg_Hist->Draw("Same");

    TLegend* legend = new TLegend(0.15, 0.7, 0.35, 0.9);  // top-left corner
    legend->AddEntry(combinedHist, "Combined", "l");
    legend->AddEntry(sig_Hist, "Signal", "l");
    legend->AddEntry(bg_Hist, "Background", "l");
    legend->Draw();

    canvas->SaveAs("dimuon_mass.png");


    // Fit combined histogram in the mass range
    // Define signal+background function:
    // For example: background = exponential, signal = Gaussian (adjust as needed)
    // TF1* fitFunc = new TF1("fitFunc", 
    //     "[0]*TMath::Gaus(x,[1],[2],true) + [3] + [4]*exp(-[5]*x)", massMin, massMax);

    
    // Initial parameter guesses:
    // fitFunc->SetParameters(
    //     combinedHist->GetMaximum(),  // Gaussian amplitude guess
    //     signalMassCenter,            // Mean near Z mass
    //     2.5,                        // Width guess (GeV)
    //     100,                         // Background  constant term
    //     0.0,                          // exp decay amplitude
    //     0.0                          // decay factor
    // );

    // TF1* bgFunc = new TF1("bgFunc", "[0]+[1] * exp(-[2]*x)", massMin, massMax);
    // bgFunc->SetParameters(fitFunc->GetParameter(3), fitFunc->GetParameter(4),
    //                      fitFunc->GetParameter(5));
    



    
    TF1 *fitFunc = new TF1("fitFunc", doubleCB, massMin, massMax, 8);
    fitFunc->SetParNames("Norm", "Mean", "Sigma", "AlphaL", "nL", "AlphaR", "nR", "offset");
    fitFunc->SetParameters(
    1e5,    // Norm
    91.1,   // Mean
    2.0,    // Sigma
    1.5,    // AlphaL
    3.0,    // nL
    2.0,    // AlphaR
    4.0,    // nR
    1e3     // Offset (background floor)
    );
    // TF1* bgFunc = new TF1("bgFunc", "[0]+[1] * exp(-[2]*x)", massMin, massMax);
    // bgFunc->SetParameters(5000., 0.1, -0.01);



    combinedHist->Fit(fitFunc, "R");

   

    // Define integration window around signal peak
    // double lowBound = signalMassCenter - signalMassWindow;
    // double highBound = signalMassCenter + signalMassWindow;

    // double totalIntegral = fitFunc->Integral(lowBound, highBound) / combinedHist->GetBinWidth(1);
    // double bgIntegral = bgFunc->Integral(lowBound, highBound) / combinedHist->GetBinWidth(1);
    // double signalIntegral = totalIntegral - bgIntegral;

    // // Statistical significance (naive)
    // double significance = signalIntegral / std::sqrt(bgIntegral);

    // Output results
    // std::cout << "Integration window: [" << lowBound << ", " << highBound << "] GeV" << std::endl;
    // std::cout << "Total events in window (signal+background): " << totalIntegral << std::endl;
    // std::cout << "Background events in window: " << bgIntegral << std::endl;
    // std::cout << "Signal events in window (total - background): " << signalIntegral << std::endl;
    // std::cout << "Estimated significance (S / sqrt(B)): " << significance << std::endl;

    // Save plot with fit
    TCanvas* c = new TCanvas("c", "Dimuon Mass Fit", 800, 600);
    combinedHist->SetMinimum(2000);
    combinedHist->Draw();
    c->SetLogy();
    fitFunc->SetLineColor(kBlue);
    fitFunc->Draw("same");
    // bgFunc->SetLineColor(kGreen);
    // bgFunc->SetLineStyle(2);
    // bgFunc->Draw("same");
    // bg_Hist->SetLineColor(kOrange);
    // bg_Hist->Draw("Same");

    TLegend* legend2 = new TLegend(0.15, 0.7, 0.35, 0.9);  // top-left corner
    legend2->AddEntry(combinedHist, "Combined hist", "l");
    legend2->AddEntry(fitFunc, "Fit", "l");
    //legend2->AddEntry(bgFunc, "Background fit", "l");
    legend2->Draw();

    c->SaveAs("dimuon_mass_fit.png");

    delete combinedHist;
    delete fitFunc;
    //delete bgFunc;
    delete c;
    

    return 0;
}
