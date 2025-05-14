import ROOT
import sys
import math
import numpy as np

def smear_momentum(px, py, pz, e, Pt, Eta, Phi, Theta):
    # Extract components
    mass = 0.1057 # for muons
    

    # Smear values
    pt_smeared = np.random.normal(Pt, 0.01 * Pt)
    theta_smeared = np.random.normal(Theta, 0.002)
    if theta_smeared <= 0:
        theta_smeared = -theta_smeared
    phi_smeared = np.random.normal(Phi, 0.002)
    if phi_smeared <= 0:
        phi_smeared+= 2* np.pi
    eta_smeared = - np.log(np.tan(theta_smeared/2))

    # Recalculate momentum components
    px_smeared = pt_smeared * np.cos(phi_smeared)
    py_smeared = pt_smeared * np.sin(phi_smeared)
    pz_smeared = pt_smeared / np.tan(theta_smeared)

    # Total p vector and energy
    p_smeared = np.sqrt(px_smeared**2 + py_smeared**2 + pz_smeared**2)
    E_smeared = np.sqrt(p_smeared**2 + mass**2)


    # Return new
    return px_smeared, py_smeared, pz_smeared, E_smeared, pt_smeared, eta_smeared, phi_smeared, theta_smeared



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
        Eta = intree.Eta
        Phi = intree.Phi
        Theta = intree.Theta
        Pt = intree.Pt
        px = intree.px
        py = intree.py
        pz = intree.pz
        e = intree.e

        # Modify in-place
        for j in range(len(Id)):
            if abs(Id[j]) == 13:  # Only muons
                new_px, new_py, new_pz, new_e, new_Pt, new_Eta, new_Phi, new_Theta = smear_momentum(
                    px[j], py[j], pz[j], e[j],
                    Pt[j], Eta[j], Phi[j], Theta[j]
                )
                px[j] = new_px
                py[j] = new_py
                pz[j] = new_pz
                e[j] = new_e
                Pt[j] = new_Pt
                Eta[j] = new_Eta
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
