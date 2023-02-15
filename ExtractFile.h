/*
 _______________________________________________
|		     TIPP 2023	      		|
|     	     Gaël COULON - Arthur Dedieu     	|
|				     		|
|	      	   ExtractFile.h		|
|Root macro to extract data in different 	|
|centrality collisions, useful for Contours.cpp	| 
|_______________________________________________|

*/

#ifndef EXTRACTFILE_H
#define EXTRACTFILE_H

//-- Global includes --
#include "TROOT.h"
#include "TH1F.h"
#include "TFile.h"
#include <string.h>
#include <iostream>

using namespace std;

/*
----- Parameters -----
	i: from 1 (most central collisions) to 10 (lowest ones), data are separed thanks to this index  
 particle: "proton" or "phi"
     type: "pp" or "PbPb"
    value: give back the label of the collision studied (0,1,2,3)
    title: give back the appropriate title for the histogram
     mass: give back the particle mass in Gev
     data: data histogram
 err_stat: statistic errors histogram
 err_syst: systematic errors histogram
err_syst2: uncorrelated systematic errors histogram (in one case)

*/
void ExtractFile(Int_t i, TString particle, TString type, Int_t &value, TString &title, Double_t &mass, TH1F *&data, TH1F *&err_stat, TH1F *&err_syst, TH1F *&err_syst2, TFile *file){

if(particle=="proton"){ 	//i in range [1,10]
	mass=0.938;
	file=TFile::Open("HEPData-1569102768-v1-root.root");
	TDirectoryFile* dirfile1 = (TDirectoryFile*) file->FindObjectAny("Table 6");
	TDirectoryFile* dirfile2 = (TDirectoryFile*) file->FindObjectAny("Table 5");
	if (type=="PbPb"){
		value=0;
		
		data=new TH1F(*((TH1F*)dirfile2->FindObjectAny(Form("Hist1D_y%d",i))));
		err_stat=new TH1F(*((TH1F*)dirfile2->FindObjectAny(Form("Hist1D_y%d_e1",i))));
		err_syst=new TH1F(*((TH1F*)dirfile2->FindObjectAny(Form("Hist1D_y%d_e2",i))));
		err_syst2=new TH1F(*((TH1F*)dirfile2->FindObjectAny(Form("Hist1D_y%d_e3",i))));
		
		Double_t centr1,centr2;
		if(i<3){
			centr1=5*(i-1);
			centr2=5*i;
		}
		else{
			centr1=10*(i-2);
			centr2=10*(i-1);
		}
		title=Form("p_{T} distribution of p+#bar{p} measured in Pb-Pb collisions at #sqrt{s_{NN}}=5.02 TeV with %2.0f-%2.0f %% centrality", centr1, centr2);
	}
	else if(type=="pp"){ //i=1, centrality: 0-5% fixed
		value=1;
		//Need to attribute an argument to the pointer even if it's only used for protons production in PbPb
		
		data=new TH1F(*((TH1F*)dirfile1->FindObjectAny(Form("Hist1D_y%d",i))));
		err_stat=new TH1F(*((TH1F*)dirfile1->FindObjectAny(Form("Hist1D_y%d_e1",i))));
		err_syst=new TH1F(*((TH1F*)dirfile1->FindObjectAny(Form("Hist1D_y%d_e2",i))));
		err_syst2=new TH1F(*((TH1F*)dirfile2->FindObjectAny(Form("Hist1D_y1_e3"))));
		
		title=Form("p_{T} distribution of p+#bar{p} measured in pp collisions at #sqrt{s_{NN}}=5.02 TeV with 0-5 %% centrality");
	}
}

else if (particle=="phi"){	   //i in range [1,8]
	mass=1.020;
	file=TFile::Open("HEPData-ins1762368-v1-root.root");
	//Need to attribute an argument to the pointer even if it's only used for protons production in PbPb
	TDirectoryFile* dirfile1 = (TDirectoryFile*) file->FindObjectAny("Table 5");
	err_syst2=new TH1F(*((TH1F*)dirfile1->FindObjectAny(Form("Hist1D_y1_e3"))));
	if (type=="PbPb"){
		value=2;
		TDirectoryFile* dirfile = (TDirectoryFile*) file->FindObjectAny("Table 3");
		
		data=new TH1F(*((TH1F*)dirfile->FindObjectAny(Form("Hist1D_y%d",i))));
		err_stat=new TH1F(*((TH1F*)dirfile->FindObjectAny(Form("Hist1D_y%d_e1",i))));
		err_syst=new TH1F(*((TH1F*)dirfile->FindObjectAny(Form("Hist1D_y%d_e2",i))));
		
		Double_t centr1=10*(i-1);
		Double_t centr2=10*i;
		title=Form("p_{T} distribution of #phi mesons measured in Pb-Pb collisions at #sqrt{s_{NN}}=5.02 TeV with %2.0f-%2.0f %% centrality", centr1, centr2);
	}
	else if (type=="pp"){  //i=1, centrality: 0-5% fixed
		value=3;
		TDirectoryFile* dirfile = (TDirectoryFile*) file->FindObjectAny("Table 4");
		
		data=new TH1F(*((TH1F*)dirfile->FindObjectAny(Form("Hist1D_y%d",i))));
		err_stat=new TH1F(*((TH1F*)dirfile->FindObjectAny(Form("Hist1D_y%d_e1",i))));
		err_syst=new TH1F(*((TH1F*)dirfile->FindObjectAny(Form("Hist1D_y%d_e2",i))));
		
		title=Form("p_{T} distribution of #phi mesons measured in pp collisions at #sqrt{s_{NN}}=5.02 TeV with 0-5 %% centrality");
	}
}

};
#endif
