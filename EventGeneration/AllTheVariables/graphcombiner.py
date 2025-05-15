import sys
import ROOT
from ROOT import TLorentzVector, TCanvas, TH1F

ROOT.gROOT.SetBatch(True)

# Check for even number of arguments after the script name
if len(sys.argv) < 3 or len(sys.argv[1:]) % 2 != 0:
    print("Usage: python3 plot_muon_mass.py <input1.root> <scale1> [<input2.root> <scale2> ...]")
    sys.exit(1)

# Final combined histogram
combined_hist = TH1F("combined_muon_mass", "Combined Invariant Mass of Last Two Muons;Mass (GeV);Events", 100, 0, 140)

# Process each pair of (file, scale)
args = sys.argv[1:]
for i in range(0, len(args), 2):
    input_file = args[i]
    scale_factor = float(args[i + 1])

    # Open ROOT file
    file = ROOT.TFile.Open(input_file)
    tree = file.Get("Events")
    if not tree:
        print(f"Could not find TTree named 'Events' in the file {input_file}. Skipping.")
        continue

    # Temporary histogram for this file
    temp_hist = TH1F(f"muon_mass_{i}", f"Invariant Mass from {input_file};Mass (GeV);Events", 100, 0, 140)

    for event in tree:
        muons = []

        # Access branches
        px = list(event.px)
        py = list(event.py)
        pz = list(event.pz)
        e  = list(event.e)
        ids = list(event.Id)

        for i in range(len(ids)):
            if abs(ids[i]) == 13:  # Muon
                p4 = TLorentzVector(px[i], py[i], pz[i], e[i])
                muons.append(p4)

        if len(muons) >= 2:
            p1 = muons[-1]
            p2 = muons[-2]
            mass = (p1 + p2).M()
            temp_hist.Fill(mass)

    # Scale and add to combined histogram
    temp_hist.Scale(scale_factor)
    combined_hist.Add(temp_hist)

# Draw and save final combined histogram
canvas = TCanvas("canvas", "canvas", 800, 600)
combined_hist.Draw()
canvas.SaveAs("combined_muon_invariant_mass.png")

print("Combined plot saved as combined_muon_invariant_mass.png")
