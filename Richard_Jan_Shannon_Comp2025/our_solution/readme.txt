To generate the data it is recommended to generate signal and background separately.
For data generation type into command line:

make compile
make makesignal    or    make makebackground
./makesignal >> signal.output    or    ./makebackground >> background.output


To run filters (trigger_filter.py, stricter_filter.py, isolation_filter.py)
and gaussian blur (gaussian_smear.py)
or graphing script (plot_invariant_muon_mass.py) you type into your commandline:

python3 <codename>.py <dataname>.root


To plot an invariant mass spectrum of any of the resulting root files and generate a png image, type:

python3 plot_invariant_muon_mass.py <dataname>.root


To combine the histograms and run the fitting routine and analysis, type:

make compile
make analysis
./analysis >> fit.output


For a more in-depth description please check the "Manual of Use" section in the project report pdf.
