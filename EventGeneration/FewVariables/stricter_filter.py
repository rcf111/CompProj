import ROOT
import sys

def main():
    if len(sys.argv)  != 2:
        print("Usage: python stricter_filter.py input.root")
        return

    input_file = sys.argv[1]
    output_file = "Stricterfiltered_"+input_file

    # Create a RDataFrame from the input file
    df = ROOT.RDataFrame("Events", input_file)

    # Define muon selection mask
    df_muon = df.Define("muon_mask", "abs(Id) == 13 && Pt > 30") \
                .Define("n_muons", "Sum(muon_mask)") \
                .Filter("n_muons == 2", "Exactly 2 muons with pt > 30")

    # Save filtered events to new ROOT file
    df_muon.Snapshot("Events", output_file)

    print(f"Filtered events saved to {output_file}")

if __name__ == "__main__":
    main()
