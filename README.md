# TIPP2023 - Study of integrated and transverse momentum differential production rates for particles emitted in high energy collisions at the LHC

This project proposes to determine integrated production rates for hadron species obtained in proton-proton (pp) and lead-lead (Pb–Pb) collisions at 5.02 TeV per nucleon pair using data collected by the ALICE experiment [1]{https://arxiv.org/abs/1910.07678} [2]{https://arxiv.org/abs/1910.14419}.

# Usage 

- **Yield.cpp**: Display the data wanted, compute the different fits associated, the bin integral and the yield extrapolation depending on the scenario set.
- **Contours.cpp**: Compute and display the contour graph of the temperature T in function of the <img src="https://latex.codecogs.com/gif.latex?\langle \beta_T \rangle" /> parameter of the hydrodynamic blastwave model for protons in PbPb collisions for all centralities.

- **Functions.h**: Mathematical expression of the different functions used to fit the data of the project.
- **ExtractFile.h**: ROOT macro to extract data in different centrality collisions, useful for `Contours.cpp`.

- **rootLogonV2.c**: ROOT preconfig file. 
- **.rootrc**: Where to put the `rootLogonV2.c` path.
 
- **HEPData-1569102768-v1-root.root**: ROOT tree where to find *protons* [data]{https://www.hepdata.net/record/sandbox/1569102768}.
- **HEPData-ins1762368-v1-root.root**: ROOT tree where to find *phi* [data]{https://www.hepdata.net/record/ins1762368}.

- **/ResultExamples**: Examples of few graph computed thanks to these programs.


# Environment and Installation

ROOT version used: `6.26/10`
Ubuntu version used: `22.04.1 LTS`

1. Download the git project.

2. Make sure the right ROOT environment is setup, to be in the same condition as this project use the `rootlogonV2.c` file (put the complet path to it in the right allocated section in the `.rootrc` file).


# Compilation

1. Compile `Yield.cpp`:
```
>> root 'Yield.cpp++(particle,type)'
```
Where `particle (TString)` is either `"proton"` or `"phi"` and `type (TString)` is either `"pp"` or `"PbPb"`. 
The `++` option is to compile the program, you can also only launch an interpreted version of it by removing this option.
It's recommended to add `2> /dev/null` to remove all the warning of integration convergences computed by ROOT inside your terminal and display only the results.

2. Compile `Contours.cpp`:
```
>> root Contours.cpp++
```
Same remark for the `++` option.
Same recommendation with adding `2> /dev/null` at the end to avoid warning display.
