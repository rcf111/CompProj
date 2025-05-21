import sys
import ROOT
import numpy as np
from ROOT import TLorentzVector, TCanvas, TH1F

ROOT.gROOT.SetBatch(True)

# Check for even number of arguments after the script name
if len(sys.argv) < 3 or len(sys.argv[1:]) % 2 != 0:
    print("Usage: python3 plot_muon_mass.py <input1.root> <scale1> [<input2.root> <scale2> ...]")
    sys.exit(1)

# Final combined histogram
combined_hist = TH1F("combined_muon_mass", "Combined Invariant Mass of Last Two Muons;Mass (GeV);Events", 140, 0, 120)

# Process each pair of (file, scale)
args = sys.argv[1:]

signal = args[0]
scale_signal = float(args[1])
background = args[2]
scale_background = float(args[3])


signal_file = ROOT.TFile.Open(signal)
sig_tree = signal_file.Get("Events")
if not sig_tree:
    print(f"Could not find TTree named 'Events' in the file {signal}. Skipping.")

sig_hist = TH1F(f"muon_mass_", f"Invariant Mass from {signal};Mass (GeV);Events", 120, 0, 120)

background_file = ROOT.TFile.Open(background)
bg_tree = background_file.Get("Events")
if not bg_tree:
    print(f"Could not find TTree named 'Events' in the file {background}. Skipping.")

bg_hist = TH1F(f"muon_mass_", f"Invariant Mass from {background};Mass (GeV);Events", 120, 0, 120)


for event in sig_tree:
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
            p1 = muons[-1]
            p2 = muons[-2]
            mass = (p1 + p2).M()
            sig_hist.Fill(mass)


for event in bg_tree:
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
            p1 = muons[-1]
            p2 = muons[-2]
            mass = (p1 + p2).M()
            bg_hist.Fill(mass)

sig_hist.Scale(scale_signal)
bg_hist.Scale(scale_background)
sig_hist.Add(bg_hist)

# Draw and save final combined histogram
canvas = TCanvas("canvas", "canvas", 800, 600)
canvas.SetLogy(True)
sig_hist.Draw()
canvas.SaveAs("combined_muon_invariant_mass.png")

print("Combined plot saved as combined_muon_invariant_mass.png")
