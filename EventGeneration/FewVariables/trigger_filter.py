#!/usr/bin/env python3

import datetime
import ROOT
import sys

ROOT.ROOT.EnableImplicitMT()
ROOT.gROOT.SetBatch(True)


if len(sys.argv) != 2:
    print("Usage: python3 trigger_filter.py <input_file.root>")
    sys.exit(1)

input_file = sys.argv[1]

tree_name = "Events"
output_file = "filtered_"+input_file

# Load the original tree with RDataFrame
df = ROOT.RDataFrame(tree_name, input_file)
# Apply filter on the boolean trigger branch
df_filtered = df.Filter("HLT_DoubleIsoMu20_eta2p1 == 1", "Trigger filter")
# Write the filtered events into a new TTree in a new ROOT file
df_filtered.Snapshot(tree_name, output_file)

fOUT = ROOT.TFile.Open(output_file, "UPDATE")
fOUT.Close()
print(f"Filtered tree saved to {output_file}")