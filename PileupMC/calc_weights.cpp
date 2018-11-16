#include "ComputeWeights.h"
#include <TGraph.h>
#include <TCanvas.h>
#include <TTree.h>
#include <TFile.h>
#include <TObject.h>
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char** argv){
    
    int verbosity = 0;
    int firstSample = 3; // 3 for current weights configuration. 
  	bool dofitbaseline = 1; // 1 to use negative weights for baseline subtraction, 0 for positive weights. 
	bool dofittime = 0; // 1 to create timing jitter weights. 0 to not.  
  	static int nPulseSamples = 5; // 0 to 10
  	int prepulsesamples = 0; // 0 to 10 (need to understand correlation to npulsesamples)
  	double P = 0; //Simulated pedestal value. Shouldn't affect bias with dofitbaseline on. 
    
    // ComputerWeight instance
    ComputeWeights cw (verbosity, dofitbaseline, dofittime, nPulseSamples, prepulsesamples);

    TFile* file = new TFile("output.root", "update");
    TTree* tree_samples = (TTree*) file->Get("samples");

    // Digis from tree
    std::vector<double> * digis = new std::vector<double>();
    tree_samples->SetBranchAddress("digis", &digis);

    // vector for weights to be added to the tree
    double weights [nPulseSamples];
    TBranch* wbranch = tree_samples->Branch("weights", &weights,
            ("weights["+ std::to_string(nPulseSamples)+"]/D").c_str());

    std::vector<double> pulse_deriv; // not used for amplitude weights

    for (int i = 0; i< tree_samples->GetEntries(); i++){
        tree_samples->GetEntry(i);

        cw.compute(*digis, pulse_deriv, firstSample); 
        
        for (int iw = 0; iw< nPulseSamples; iw++){
            weights[iw] = cw.getAmpWeight(iw + firstSample -1);
        }

        wbranch->Fill();
        
    }
    
    std::cout << "ok" << std::endl;
    tree_samples->Write("", TObject::kOverwrite);
    file->Close();
}