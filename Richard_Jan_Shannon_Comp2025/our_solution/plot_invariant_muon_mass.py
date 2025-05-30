import sys
import ROOT
import numpy as np
from ROOT import TLorentzVector, TCanvas, TH1F

ROOT.gROOT.SetBatch(True)

if len(sys.argv) != 2:
    print("Usage: python3 plot_invariant_muon_mass.py <input_file.root>")
    sys.exit(1)

input_file = sys.argv[1]

# Open ROOT file
file = ROOT.TFile.Open(input_file)
tree = file.Get("Events")
if not tree:
    print("Could not find TTree named 'Events' in the file.")
    sys.exit(1)

# Create histogram
hist = TH1F("muon_mass", "Invariant Mass of Last Two Muons;Mass (GeV);Events", 120, 60, 120)

# Loop over events
for event in tree:
    muons = []

    # Access branches
    Pt = list(event.Pt)
    Theta = list(event.Theta)
    Phi = list(event.Phi)
    ids = list(event.Id)

    nMuons = 0
    mass = 0.1057 # for muons

    for i in range(len(ids)):
        if abs(ids[i]) == 13:  # Muon
            nMuons+=1
            px = Pt[i] * np.cos(Phi[i])
            py = Pt[i] * np.sin(Phi[i])
            pz = Pt[i] / np.tan(Theta[i])

            # Total p vector and energy
            p = np.sqrt(px**2 + py**2 + pz**2)
            E = np.sqrt(p**2 + mass**2)

            p4 = TLorentzVector(px, py, pz, E)
            muons.append(p4)

    if len(muons) >= 2:
        # Take the last two muons
        p1 = muons[-1]
        p2 = muons[-2]
        mass = (p1 + p2).M()
        hist.Fill(mass)
    #print("Number of muons in event",nMuons)
# Draw and save histogram
canvas = TCanvas("canvas", "canvas", 800, 600)
canvas.SetLogy(True)
hist.Draw()
canvas.SaveAs(input_file.removesuffix(".root")+".png")

print("Plot saved as muon_invariant_mass.png")
