import ROOT
import sys
import math
import numpy as np

def smear_momentum(Pt, Phi, Theta):


    # Smear values
    pt_smeared = np.random.normal(Pt, 0.01 * Pt)
    theta_smeared = np.random.normal(Theta, 0.002)
    if theta_smeared <= 0:
        theta_smeared = -theta_smeared
    phi_smeared = np.random.normal(Phi, 0.002)
    if phi_smeared <= 0:
        phi_smeared+= 2* np.pi


    # Return new
    return pt_smeared, phi_smeared, theta_smeared



def main():
    if len(sys.argv) != 2:
        print("Usage: python gaussian_smear.py input.root")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = "Smeared_"+input_file

    # Open input ROOT file
    infile = ROOT.TFile.Open(input_file, "READ")
    intree = infile.Get("Events")

    # Create output ROOT file and clone the tree structure
    outfile = ROOT.TFile.Open(output_file, "RECREATE")
    outfile.cd()
    outtree = intree.CloneTree(0)  # Clone structure, no entries

    # Set up branch addresses (read)


    n_entries = intree.GetEntries()
    for i in range(n_entries):
        intree.GetEntry(i)
        Id = intree.Id
        Phi = intree.Phi
        Theta = intree.Theta
        Pt = intree.Pt

        # Modify in-place
        for j in range(len(Id)):
            if abs(Id[j]) == 13:  # Only muons
                new_Pt, new_Phi, new_Theta = smear_momentum(Pt[j], Phi[j], Theta[j])

                Pt[j] = new_Pt

                Phi[j] = new_Phi
                Theta[j] = new_Theta

        outtree.Fill()

    # Write and close files
    outtree.Write()
    outfile.Close()
    infile.Close()
    print(f"Processed {n_entries} entries and saved to {output_file}")

if __name__ == "__main__":
    main()
