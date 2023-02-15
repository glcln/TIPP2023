/*
 _______________________________________________
|		     TIPP 2023	      		|
|     	     Gaël COULON - Arthur Dedieu     	|
|				     		|
|	      	   MAIN FUNCTION		|
|Get the data histogram, fit it depending on the|
|collision caracteristics (particles: protons or|
|phi, type: pp or PbPb)	and estimate the total  | 
|yield.						|
|_______________________________________________|

*/

//-- Global Include --
#include "Math/Integrator.h"
#include "Math/WrappedFunction.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TLatex.h"
#include "TROOT.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TText.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
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
	gStyle->SetTickLength(-0.02,"Y");
	gStyle->SetPadTickX(1);
	gStyle->SetTitleBorderSize(0);
	gStyle->SetTitleX(0.5);
	gStyle->SetTitleAlign(23);
	gStyle->SetTitleW(1);
	gStyle->SetTitleSize(0.030,"Y");
	gStyle->SetTitleSize(0.035,"X");
}


//-- MAIN --
void Yield(TString particle, TString type){

	//-- Avoid any display in the terminal by ROOT -- (uncomment if wanted)
	/*
	gROOT->ProcessLine("gErrorIgnoreLevel=3001");
	gROOT->ProcessLine("gPrintViaErrorHandler = kTRUE;");
  	*/
  	
  	myOptions();
  	
  	
  	//-- Log scale -- 1:ON & 0:OFF
  	Int_t log=1;
	
	
  	
	//-- Global variables --
	
	Int_t value=0;   		// 1,2,3,4: label of the collision considered
	Double_t mass=0; 		// Particle mass in Gev
	Double_t b_min=0;		// MAXWELL-BOLTZMANN model range
	Double_t b_max=0;
	Double_t p_min=0;		// POWER LAW model range
	Double_t p_max=0;
	Double_t l_min=0;		// LEVY-TSALLIS model range
	Double_t l_max=0;
	Double_t Vmax=0;		// Max of the data
	Double_t bin=0;			// Bin number
	Double_t ExtrapolIntegral=0;	// Extrapolated integral
	Double_t ExtrapolError=0;	
	Double_t BinIntegralError=0;	// Bin integral
	Double_t BinIntegral=0;		
	TString title="";		// Histogram title
	TString txt1;			// Legend
	TString txt2;
	TString txt3;
	TString txt4;
	TString txt5;
	TString txt6;
	TString txt7;
	
	
	
	//-- File extraction --
	
	TH2F *empty=0;			// Empty histogram to display the [0,first bin] part
	TH1F *h1_data;			// Data
	TH1F *h1_err_stat;		// Errors associated
	TH1F *h1_err_syst;
	TH1F *h1_err_syst2;
	TFile *file=0;			// File of extraction
	
	//i=1: Only 0-5% centrality collisions in this code
	ExtractFile(1,particle,type,value,title,mass,h1_data,h1_err_stat, h1_err_syst,h1_err_syst2,file);
	
	bin=h1_data->GetNbinsX();
	
	if (value==3){ 			// phi in pp collision
		//Reconstructing data that were automatically matched by ROOT before
		h1_data->SetBinContent(5,0.0167252);
		h1_data->SetBinContent(6,0.015271);
	}
	for(int i=0; i<bin+1; i++){		
		if(value==0){ 		// protonss in PbPb collisions
			//Fill error bins (syst. uncorr+corr & stat.)
			h1_data->SetBinError(i,Sqrt(Power(h1_err_stat->GetBinContent(i),2) + Power(h1_err_syst->GetBinContent(i),2) + Power(h1_err_syst2->GetBinContent(i),2)));
		}
		else{ 	 		// Other collisions
			//Fill error bins (syst. & stat.)
			h1_data->SetBinError(i,Sqrt(Power(h1_err_stat->GetBinContent(i),2) + Power(h1_err_syst->GetBinContent(i),2)));
		}
	}
		
		
		
	//-- Display data --
	
	Vmax=h1_data->GetBinContent(h1_data->GetMaximumBin());
	
	if(log==1){
		//Need to adapt the empty histogram to the range of the data
		if(value%2==0) empty=new TH2F("empty","empty",20,0,20,20,1E-5,10*Vmax);
		else if(value%2==1) empty=new TH2F("empty","empty",20,0,21,20,5E-8,10*Vmax);
	}
	else if(log==0) empty=new TH2F("empty","empty",20,0,20,20,1E-5,1.1*Vmax);
	
	TCanvas *c1=new TCanvas("c1");
	c1->cd();
	
	empty->SetXTitle("p_{T} [Gev/c]");
	empty->SetYTitle("#frac{1}{N_{ev}} #frac{d^{2}N}{dydp_{T}} [Gev/c]^{-1}");
	empty->SetTitle(title);
	empty->SetStats(0);
	empty->Draw();
	
	h1_data->SetLineColor(1);
	h1_data->Draw("SAME");
	h1_data->SetXTitle("p_{T} [Gev/c]");
	h1_data->SetYTitle("#frac{1}{N_{ev}} #frac{d^{2}N}{dydp_{T}} [Gev/c]^{-1}");
	h1_data->SetTitle(title);
	h1_data->SetStats(0);



	//-- Bin integral --
	BinIntegral=h1_data->IntegralAndError(1,bin,BinIntegralError,"width");
	
	
	
	//-- Main part --
	
	if(value%2==0){		// PbPb collisions 
	
		//-- Fit part --
       	
       		// Set range of fits
       		b_min=h1_data->GetBinLowEdge(1);
       		b_max=2;
		if(value==0){
			p_min=8;
			p_max=p_max=h1_data->GetBinLowEdge(bin+1);
		}
		else if(value==2){
			p_min=5;
			p_max=h1_data->GetBinLowEdge(bin+1);
		}
		
		// MAXWELL-BOLTZMANN model
       		TF1* boltz = new TF1("boltz",Boltzmann,b_min,b_max,3);
		// Parameters: norm (SET), temperature in Gev (SET), mass in Gev(FIXED)
		if(value==0) boltz->SetParameters(400,0.520,1);
		else if(value==2) boltz->SetParameters(90,0.504,1);
		boltz->FixParameter(2,mass);
		boltz->SetLineColor(2);
		boltz->SetLineStyle(1);
		TFitResultPtr boltz_res=h1_data->Fit("boltz","0REMIS","");
		boltz->SetNpx(1000);
		boltz->Draw("SAME");
	
		// POWER LAW model
		TF1* powerlaw = new TF1("powerlaw",PowerLaw,p_min,p_max,3);
		// Parameters: coef0 (SET), coef1 (SET), exponent value (SET)
		powerlaw->SetParameters(-0.4,641,-4.3);
		powerlaw->SetLineColor(4);
		powerlaw->SetLineStyle(1);
		TFitResultPtr powerlaw_res=h1_data->Fit("powerlaw","0REMIS","");
		powerlaw->SetNpx(1000);
		powerlaw->Draw("SAME");
       	
       		
       		if(value==0){	//protons in PbPb collisions	
			
			// HYDRODYNAMIC BLASTWAVE model
			
			// Parameters
       			Double_t R=10.5; 	  // Fireball radius in fm: FIXED
       			Double_t T=0.090; 	  // Temperature in Gev: SET
       			Double_t norm=832000; 	  // Normalisation factor: FIXED
       			Double_t betaTMean=0.663; // Transverse speed to c ratio: SET
       			Double_t n=0.735;	  // Hydrodynamic parameter: FIXED

			// Fit
       			TF1* blastwave= new TF1("blastwave",Blastwave,b_min,3,6);
       			blastwave->FixParameter(0,mass);
			blastwave->SetParameter(1,T);
			blastwave->FixParameter(2,R);
			blastwave->FixParameter(3,norm);
			blastwave->SetParameter(4,betaTMean);
			blastwave->FixParameter(5,n);
			TFitResultPtr blastwave_res = h1_data->Fit("blastwave","0REMIS","");
			blastwave->SetLineColor(kGreen+3);
			blastwave->SetLineStyle(9);
			blastwave->Draw("SAME");
			
			
			//-- Yield extrapolation between 0 and the first bin using HYDRODYNAMIC BLASTWAVE model --
			TF1* bw_extrapol=new TF1("bw_extrapol",Blastwave,0,b_min,6);
			bw_extrapol->FixParameter(0,mass);
			bw_extrapol->FixParameter(1,blastwave->GetParameter(1));
			bw_extrapol->FixParameter(2,R);
			bw_extrapol->FixParameter(3,blastwave->GetParameter(3));
			bw_extrapol->FixParameter(4,blastwave->GetParameter(4));
			bw_extrapol->FixParameter(5,n);
			bw_extrapol->SetLineColor(kGreen-5);
			bw_extrapol->SetLineStyle(2);
			bw_extrapol->SetFillColor(kGreen+3);
  			bw_extrapol->SetFillStyle(3001);
  			bw_extrapol->SetNpx(1000);
  			bw_extrapol->Draw("SAME FC");
			
			ExtrapolIntegral=blastwave->Integral(0,b_min);
  			ExtrapolError=blastwave->IntegralError(0,b_min,blastwave_res->GetParams(),blastwave_res->GetCovarianceMatrix().GetMatrixArray());
  			txt2.Form("#color[209]{#bullet} Yield extrapolation in [0,%2.1f]: %3.2f #pm %3.2f", b_min,ExtrapolIntegral,ExtrapolError);
	
			
			//-- HYDRODYNAMIC BLASTWAVE model: Chi2/NDF --
			Double_t bw_chi2 = blastwave->GetChisquare();
			Double_t bw_NDF = blastwave->GetNDF();
			txt5.Form("#color[209]{----} BlastWave fit - #chi^{2}/NDF = %4.2f/%1.0f", bw_chi2, bw_NDF);
		}
		
		
		else if(value==2){	//phi in PbPb collisions
			
			//-- Yield extrapolation between 0 and the first bin using MAXWELL-BOLTZMANN model --
			TF1* boltz2 = new TF1("boltz2",Boltzmann,0,b_min,3);
			boltz2->FixParameter(0,boltz->GetParameter(0));
			boltz2->FixParameter(1,boltz->GetParameter(1));
			boltz2->FixParameter(2,mass);
			boltz2->SetLineColor(kOrange+7);
			boltz2->SetLineStyle(2);
			boltz2->SetFillColor(kOrange+7);
  			boltz2->SetFillStyle(3001);
  			boltz2->Draw("SAME FC");
			
			ExtrapolIntegral=boltz->Integral(0,b_min);
  			ExtrapolError=boltz->IntegralError(0,b_min,boltz_res->GetParams(),boltz_res->GetCovarianceMatrix().GetMatrixArray());
  			txt2.Form("#color[95]{#bullet} Yield extrapolation in [0,%2.1f]: %3.2f #pm %3.2f", b_min,ExtrapolIntegral,ExtrapolError);
		}
		
		
		//-- MAXWELL-BOLTZMANN model: Chi2/NDF --
		Double_t boltz_chi2 = boltz->GetChisquare();
		Double_t boltz_NDF = boltz->GetNDF();
		txt3.Form("#color[2]{#topbar} Boltzmann fit - #chi^{2}/NDF = %4.2f/%2.0f", boltz_chi2, boltz_NDF);
  		
  		
  		//-- POWER LAW model: Chi2/NDF --
  		Double_t powerlaw_chi2 = powerlaw->GetChisquare();
		Double_t powerlaw_NDF = powerlaw->GetNDF();
		txt4.Form("#color[4]{#topbar} Power law fit - #chi^{2}/NDF = %4.2f/%1.0f", powerlaw_chi2, powerlaw_NDF);
		
		
		//-- Bin integral legend --
		txt1.Form("Bin Integral: %4.2f #pm %3.2f", BinIntegral,BinIntegralError);	
  	}
  	
  	
 	else if (value%2==1){		// pp collisions 
  	
		//-- Fit part --
       	
       		// Set range of fits
		l_min=h1_data->GetBinLowEdge(1);
		l_max=h1_data->GetXaxis()->GetBinUpEdge(bin);
		
		// LEVY-TSALLIS model
		TF1* levy = new TF1("levy",LevyTsallis,l_min,l_max,4);
		// Parameters: temperature in Gev (SET), integral (SET), mass in Gev (FIXED), exponent value (SET)
		if(value==1) levy->SetParameters(0.2,0.2,mass,7); 
		else if(value==3) levy->SetParameters(0.3,0.03,mass,7);
		levy->FixParameter(2,mass);
		levy->SetLineColor(2);
		levy->SetLineStyle(1);
		TFitResultPtr levy_res = h1_data->Fit("levy","0RMIS","");
		levy->SetNpx(1000);
		levy->Draw("SAME");
		
		
		//-- Yield extrapolation between 0 and the first bin using LEVY-TSALLIS model --
		TF1* levy2 = new TF1("levy2",LevyTsallis,0,l_min,4);
		levy2->FixParameter(0,levy->GetParameter(0)); 
		levy2->FixParameter(1,levy->GetParameter(1));
		levy2->FixParameter(2,levy->GetParameter(2));
		levy2->FixParameter(3,levy->GetParameter(3));
		levy2->SetLineColor(kOrange+7);
		levy2->SetLineStyle(2);
		levy2->SetFillColor(kOrange+7);
  		levy2->SetFillStyle(3001);
  		levy2->SetNpx(1000);
  		levy2->Draw("SAME FC");
		
		ExtrapolIntegral=levy->Integral(0,h1_data->GetBinLowEdge(1));
		ExtrapolError=levy->IntegralError(0,h1_data->GetBinLowEdge(1),levy_res->GetParams(),levy_res->GetCovarianceMatrix().GetMatrixArray());
		txt2.Form("#color[95]{#bullet} Yield extrapolation in [0,%2.1f]: %5.4f #pm %5.4f", h1_data->GetBinLowEdge(1),ExtrapolIntegral,ExtrapolError);


       		//-- LEVY-TSALLIS model: Chi2/NDF --
       		Double_t levy_chi2 = levy->GetChisquare();
		Double_t levy_NDF = levy->GetNDF();	
  		txt6.Form("#color[2]{#topbar} Levy-Tsallis - #chi^{2}/NDF = %4.2f/%2.0f", levy_chi2, levy_NDF);
  		
  		
  		//-- Bin integral legend --
  		txt1.Form("Bin Integral: %5.4f #pm %5.4f", BinIntegral,BinIntegralError);	
  	}

  	
  	//-- Global legend display --
    	
    	if(log==1){		//Log scale: ON
  		c1->SetLogy(1);
  		
  		// Bin integral
  		TLatex *FirstTxt = new TLatex(7,0.3*Vmax,txt1);
  		myLatexDraw(FirstTxt,0.035,1);
  		
  		// Yield extrapolation
		TLatex *SecondTxt = new TLatex(7,0.15*Vmax,txt2);
  		myLatexDraw(SecondTxt,0.035,1);
  		
  		// Fits
  		if(value%2==0){
  			TLatex *ThirdTxt = new TLatex(7,0.04*Vmax,txt3);
  			myLatexDraw(ThirdTxt,0.035,1);
  			TLatex *FourTxt = new TLatex(7,0.015*Vmax,txt4);
  			myLatexDraw(FourTxt,0.035,1);
  			TLatex *FiveTxt = new TLatex(7,0.006*Vmax,txt5);
  			myLatexDraw(FiveTxt,0.035,1);
  		}
  		else{
  			TLatex *SixTxt = new TLatex(7,0.04*Vmax,txt6);
  			myLatexDraw(SixTxt,0.035,1);
  		}
  	}
  	else{			//Log scale: OFF
  		// Bin integral
  		TLatex *FirstTxt = new TLatex(8,Vmax,txt1);
  		myLatexDraw(FirstTxt,0.035,1);
  		
  		// Yield extrapolation
		TLatex *SecondTxt = new TLatex(8,0.9*Vmax,txt2);
  		myLatexDraw(SecondTxt,0.035,1);
  		
  		// Fits
  		if(value%2==0){
  			TLatex *ThirdTxt = new TLatex(8,0.8*Vmax,txt3);
  			myLatexDraw(ThirdTxt,0.035,1);
  			TLatex *FourTxt = new TLatex(8,0.7*Vmax,txt4);
  			myLatexDraw(FourTxt,0.035,1);
  			if(value==0){
  				TLatex *FiveTxt = new TLatex(8,0.6*Vmax,txt5);
  				myLatexDraw(FiveTxt,0.035,1);
  			}
  		}
  		else{
  			TLatex *SixTxt = new TLatex(8,0.8*Vmax,txt6);
  			myLatexDraw(SixTxt,0.035,1);
  		}
  	}
  
	
}	








