import sys
import ROOT
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
hist = TH1F("muon_mass", "Invariant Mass of Last Two Muons;Mass (GeV);Events", 100, 0, 140)

# Loop over events
for event in tree:
    muons = []

    # Access branches
    px = list(event.px)
    py = list(event.py)
    pz = list(event.pz)
    e  = list(event.e)
    ids = list(event.Id)

    nMuons = 0

    for i in range(len(ids)):
        if abs(ids[i]) == 13:  # Muon
            nMuons+=1
            p4 = TLorentzVector(px[i], py[i], pz[i], e[i])
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
hist.Draw()
canvas.SaveAs(input_file.removesuffix(".root")+".png")

print("Plot saved as muon_invariant_mass.png")
