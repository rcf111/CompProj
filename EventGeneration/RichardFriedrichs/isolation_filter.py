import ROOT
import sys
import math

def deltaR(eta1, phi1, eta2, phi2):
    dphi = abs(phi1 - phi2)
    if dphi > math.pi:
        dphi = 2*math.pi - dphi
    deta = eta1 - eta2
    return math.sqrt(deta**2 + dphi**2)

def is_isolated(muon_idx, Id, Eta, Phi, px, py, pz):
    mu_eta = Eta[muon_idx]
    mu_phi = Phi[muon_idx]
    sum_pion_p = 0.0

    for i in range(len(Id)):
        if i == muon_idx:
            continue
        if abs(Id[i]) == 211:
            if deltaR(mu_eta, mu_phi, Eta[i], Phi[i]) < 0.3:
                pion_p = math.sqrt(px[i]**2 + py[i]**2 + pz[i]**2)
                sum_pion_p += pion_p

    return sum_pion_p < 1.5

def main():
    if len(sys.argv) != 2:
        print("Usage: python isolation_filter.py input.root")
        return

    input_file = sys.argv[1]
    output_file = "Isolated_"+input_file

    f_in = ROOT.TFile.Open(input_file)
    tree_in = f_in.Get("Events")

    # Create output file and tree
    f_out = ROOT.TFile(output_file, "RECREATE")
    tree_out = tree_in.CloneTree(0)  # Empty tree with same structure

    # Set branch addresses
    # Loop over events
    for i in range(tree_in.GetEntries()):
        tree_in.GetEntry(i)

        # These are now auto-accessed
        Id = tree_in.Id
        Eta = tree_in.Eta
        Phi = tree_in.Phi
        px = tree_in.px
        py = tree_in.py
        pz = tree_in.pz

        muons = [j for j in range(len(Id)) if abs(Id[j]) == 13]

        if all(is_isolated(mu_idx, Id, Eta, Phi, px, py, pz) for mu_idx in muons):
            tree_out.Fill()


    #tree_out.Write()
    f_out.Close()
    print(f"Filtered events written to: {output_file}")

if __name__ == "__main__":
    main()
