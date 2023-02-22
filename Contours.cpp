/*
 _______________________________________________
|		     TIPP 2023	      		|
|     	     Gaël COULON - Arthur Dedieu     	|
|				     		|
|	      	   Contours.cpp			|
|Compute the contour graph of the temperature   |
|in function of the beta_TMean parameter of the |
|HYDRODYNAMIC BLASTWAVE model for protons in    |
|PbPb collisions for all centralities.   	|
|_______________________________________________|

*/

//-- Global Include --
#include "Math/Integrator.h"
#include "Math/WrappedFunction.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TMinuit.h"
#include "TStyle.h"
#include "TLatex.h"
#include "TROOT.h"
#include "TVirtualFitter.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TText.h"
#include "TFile.h"
#include "TH1.h"
#include "TF1.h"


//-- Personnal Include --
#include "ExtractFile.h"  //Extract a file
#include "Functions.h"	  //Mathematical functions to fit data

using namespace std;
using namespace TMath;


//-- Latex text function --
void myLatexDraw(TLatex *currentLatex, Float_t currentSize=0.05, Int_t currentColor=1){
	currentLatex->SetTextSize(currentSize);
	currentLatex->SetTextColor(currentColor);
	currentLatex->SetTextAngle(0);
	currentLatex->Draw();
}


//-- Global options for the display --
void myOptions(){
	gStyle->SetPalette(1,0);
	gStyle->SetPadTickY(1);
	gStyle->SetPadTickX(1);
	gStyle->SetTitleBorderSize(0);
	gStyle->SetTitleX(0.5);
	gStyle->SetTitleAlign(23);
}


//-- MAIN --
void Contours(){

	//-- Avoid any display in the terminal by ROOT -- (needed here to get faster)
	gROOT->ProcessLine("gErrorIgnoreLevel=3001");
	gROOT->ProcessLine("gPrintViaErrorHandler = kTRUE;");
	
	
	//-- Setup --
	
	myOptions();
	TVirtualFitter::SetDefaultFitter("Minuit");
	
	TString particle="proton";	// Particle (We consider only the protons)
	TString type="PbPb";		// Collision type (We consider only PbPb collisions)
	Int_t value=0;   		// 1,2,3,4: label of the collision considered
	Double_t mass=0; 		// Particle mass in Gev
	Double_t bin=0;			// Bin number
	TString title="";		// Histogram title
	TString txt1;			// Legend
	TString txt2;
	
	TH1F *h1_data;			// Data
	TH1F *h1_err_stat;		// Errors associated
	TH1F *h1_err_syst;
	TH1F *h1_err_syst2;
	TFile *file=0;			// File of extraction
	TGraph *graph [20];		// Graph for display


	//-- Parameters presets --
	
	Double_t R [10] = {10.5,8.8,8.1,7.3,6.2,5.3,4.5,3.6,3.1,2.3};				 // Radius of the fireball in fm
	Double_t n [10] = {0.735,0.736,0.739,0.771,0.828,0.908,1.052,1.262,1.678,2.423};	 // Hydrodynamic parameter
	Double_t betaTMean [10] = {0.663,0.660,0.655,0.643,0.622,0.595,0.557,0.506,0.435,0.355}; // Transverse speed to c ratio
	Double_t T [10] = {0.090,0.091,0.094,0.097,0.101,0.108,0.115,0.129,0.147,0.161};	 // Temperature in Gev
	Double_t norm [10] = {832000,861000,538000,329000,203000,90400,41600,12870,2840,1060};   // Normalisation factor 
	
	
	//-- Contours --
    	
    	for(int i=1;i<11;i++){
    	
    		// Extract data and fill the histogram
        	ExtractFile(i,particle,type,value,title,mass,h1_data,h1_err_stat, h1_err_syst,h1_err_syst2,file);
	
        	bin=h1_data->GetNbinsX();
        	
        	for(int i=0; i<bin; i++){
            		h1_data->SetBinError(i,sqrt(pow(h1_err_stat->GetBinContent(i),2) + pow(h1_err_syst->GetBinContent(i),2) + pow(h1_err_syst2->GetBinContent(i),2)));
        	}
	
        	// Blastwave fit
        	TF1* bw = new TF1("bw",Blastwave,0.3,3,6);
        	bw->FixParameter(0,0.938); 		//par[0]: mass in Gev
        	bw->SetParameter(1,T[i-1]); 		//par[1]: T in Gev
        	bw->FixParameter(2,R[i-1]); 		//par[2]: R in fm
        	bw->FixParameter(3,norm[i-1]); 		//par[3]: normalisation factor
        	bw->SetParameter(4,betaTMean[i-1]); 	//par[4]: betaTMean
        	bw->FixParameter(5,n[i-1]); 		//par[5]: n
        	TFitResultPtr bw_res = h1_data->Fit("bw","0REMIS","");
        	bw->SetNpx(1000);
		
        	//Contours
        	gMinuit->SetErrorDef(4); 	//2 sigma
        	graph[2*(i-1)] = (TGraph*)gMinuit->Contour(100,4,1);
        	gMinuit->SetErrorDef(1); 	//1 sigma
        	graph[2*i-1] = (TGraph*)gMinuit->Contour(100,4,1);
	}
    
    
    	//-- Display part --
    	TCanvas *c1 = new TCanvas("c1","contours");
    	graph[0]->SetLineColor(2);
    	graph[0]->SetTitle("");
    	graph[0]->GetXaxis()->SetTitle("#LT#beta_{T}#GT");
    	graph[0]->GetYaxis()->SetTitle("T_{Kin}  [GeV]");
   	graph[0]->Draw("al");
   	graph[0]->GetXaxis()->SetLimits(0.3,0.7);
   	graph[0]->GetYaxis()->SetRangeUser(0.08,0.165);
   	
   	graph[1]->SetLineColor(4);
    	graph[1]->Draw("l");
       
    	for(int i=2;i<11;i++){
        	graph[2*i-2]->SetLineColor(2);
        	graph[2*i-2]->Draw("l");
        	
        	graph[2*i-1]->SetLineColor(4);
        	graph[2*i-1]->Draw("l");
    	}
    
    
    	//-- Legend --
    	txt1.Form("#color[2]{-----} 1 - #sigma contour");
    	txt2.Form("#color[4]{-----} 2 - #sigma contour");
    	TLatex *FirstTxt = new TLatex(0.35,0.105,txt1);
    	myLatexDraw(FirstTxt,0.035,1);
    	TLatex *ScndTxt = new TLatex(0.35,0.11,txt2);
    	myLatexDraw(ScndTxt,0.035,1);
    
    
   	//-- Save the Contours --
    	TFile* file2 = new TFile("Contours_CR.root", "RECREATE");
    	file2->cd();
    	c1->Write();
    
}








