#ifndef MyAnalysis_h
#define MyAnalysis_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>

// Headers needed by this particular selector

class MyAnalysis : public TSelector {
public :
   TTreeReader     fReader;  //!the tree reader
   TTree          *fChain = 0;   //!pointer to the analyzed TTree or TChain

   // Readers to access the data (delete the ones you do not need).
   TTreeReaderValue<Bool_t> HLT_IsoMu24 = {fReader, "HLT_IsoMu24"};
   TTreeReaderValue<Int_t> PV_npvs = {fReader, "PV_npvs"};

   MyAnalysis(TTree * /*tree*/ = 0) { }
   ~MyAnalysis() override { }
   
   Int_t   Version() const override { return 2; }
   void    Begin(TTree *tree) override;
   void    SlaveBegin(TTree *tree) override;
   void    Init(TTree *tree) override;
   bool    Notify() override;
   bool    Process(Long64_t entry) override;
   Int_t   GetEntry(Long64_t entry, Int_t getall = 0) override { return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0; }
   void    SetOption(const char *option) override { fOption = option; }
   void    SetObject(TObject *obj) override { fObject = obj; }
   void    SetInputList(TList *input) override { fInput = input; }
   TList  *GetOutputList() const override { return fOutput; }
   void    SlaveTerminate() override;
   void    Terminate() override;

   TH1F* pileup_distribution;
   ClassDefOverride(MyAnalysis, 0);  // Ensure this is in the header file

};

#endif
