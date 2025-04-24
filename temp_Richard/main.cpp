#include <TFile.h>
#include <TTree.h>
#include <iostream>
#include "MyAnalysis.h"
#include "MyAnalysis.C"

int main() {
    // Open the ROOT file
    TFile *file = TFile::Open("DYJetsToLL.root");
    if (!file || file->IsZombie()) {
        std::cerr << "Failed to open file!" << std::endl;
        return 1;
    } else {
        std::cout << "File opened successfully!" << std::endl;
    }

    // Retrieve the tree from the file
    TTree *tree = (TTree*)file->Get("Events");
    if (!tree) {
        std::cerr << "Failed to retrieve tree 'Events'" << std::endl;
        return 1;
    } else {
        std::cout << "Tree 'Events' retrieved with " << tree->GetEntries() << " entries." << std::endl;
    }

    // Create an instance of your analysis class
    MyAnalysis *selector = new MyAnalysis();

    // Process the tree with the selector
    tree->Process(selector);

    // Clean up
    delete selector;
    file->Close();

    return 0;
}
