#include "MyAnalysis.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

void MyAnalysis::Init(TTree *tree) {
   fReader.SetTree(tree);
}

bool MyAnalysis::Notify() {
   return true;
}

void MyAnalysis::Begin(TTree *tree) {
   TString option = GetOption();
   pileup_distribution = new TH1F("pileup_distribution","",80,-40,40);
}

void MyAnalysis::SlaveBegin(TTree *tree) {
   TString option = GetOption();
}

bool MyAnalysis::Process(Long64_t entry) {
   fReader.SetLocalEntry(entry);
   if (*HLT_IsoMu24){
      pileup_distribution->Fill(*PV_npvs);
      return true;
   }
   return false;
}

void MyAnalysis::SlaveTerminate() {}

void MyAnalysis::Terminate() {
   // Create a canvas to draw the histogram
   TCanvas *c1 = new TCanvas("c1", "Pileup Distribution", 800, 600); 
   pileup_distribution->Draw();

   // Save the histogram as a PNG file
   c1->SaveAs("pileup_distribution.png");

   // Optionally, you can delete the canvas if it's no longer needed
   delete c1;
}

ClassImp(MyAnalysis)  // Ensure this is in the implementation file
