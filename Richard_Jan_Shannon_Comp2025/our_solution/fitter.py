import ROOT
import math

ROOT.gROOT.SetBatch(True)


file = ROOT.TFile("combined_hist.root")
hist = file.Get("muon_mass_") 

#fun1 = "[0]*TMath::Exp(-[1]*x)"
#fun1 = "[0]*x + [1]"
fun1 = "[0]/(x*[2]*sqrt(2*TMath::Pi())) * TMath::Exp(-0.5*((TMath::Log(x)-[1])/[2])^2)"
#fun2 = "[3]*TMath::Gaus(x, [4], [5], true)"
fun2 = "[3]/((x^2 - [4]^2)^2 + [4]^2*[5]^2)"

fit_func = ROOT.TF1("fit_func", fun1+" + "+fun2, 0, 140)
fit_func.SetParameters(100, 1, 1, 100, 90, 5) 

hist.Fit(fit_func, "R")


A, lambd, lambd2, B, mu, sigma = [fit_func.GetParameter(i) for i in range(6)]
print(f"Background params: A={A:.2f}, lambda={lambd:.4f}, lambda2={lambd2:.4f}")
print(f"Signal params: B={B:.2f}, mu={mu:.2f}, sigma={sigma:.2f}")


x_low = mu - 2 * abs(sigma)
x_high = mu + 2 * abs(sigma)


exp_func = ROOT.TF1("Background", fun1, 0, 140)
exp_func.SetParameters(A, lambd, lambd2)
exp_func.SetLineColor(ROOT.kBlue)
exp_func.SetLineStyle(2)
exp_func.SetLineWidth(2)


fit_func.SetLineColor(ROOT.kRed)
fit_func.SetLineWidth(2)


c = ROOT.TCanvas("c", "Fit Result", 800, 600)
#c.SetLogy()
#c.SetLogx()

hist.SetMarkerStyle(20)
hist.Draw("E")
exp_func.Draw("SAME")
fit_func.Draw("SAME")


legend = ROOT.TLegend(0.6, 0.7, 0.88, 0.88)
legend.AddEntry(hist, "Data", "lep")
legend.AddEntry(exp_func, "Background", "l")
legend.AddEntry(fit_func, "Background + Signal fit", "l")
legend.Draw()



c.SaveAs("fit_result.png") 


area_under_exp = exp_func.Integral(x_low, x_high)


bin_low = hist.FindBin(x_low)
bin_high = hist.FindBin(x_high)
area_under_hist = hist.Integral(bin_low, bin_high)


area_above_exp = area_under_hist - area_under_exp

print(f"Area under exponential in ±2σ: {area_under_exp:.2f}")
print(f"Area under histogram in ±2σ: {area_under_hist:.2f}")
print(f"Area above exponential in ±2σ (signal): {area_above_exp:.2f}")
print(f"Statistical significance (N_signal/sqrt(N_background)): {area_above_exp/(area_under_exp**(0.5)):.2f}")