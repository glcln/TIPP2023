/*
 _______________________________________________
|		     TIPP 2023	      		|
|     	     Gaël COULON - Arthur Dedieu     	|
|				     		|
|	      	   Functions.h 			|
|Mathematical expression of the different 	|
|functions used to fit the data of the project.	| 
|_______________________________________________|

*/

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

//-- Global includes --
#include "TMath.h"
#include "TF1.h"

using namespace TMath;


//-- LEVY-TSALLIS --
Double_t LevyTsallis(Double_t *x, Double_t *par){
	Double_t T=par[0];
	Double_t coef=par[1];
	Double_t m=par[2];
	Double_t n=par[3];
 
	Float_t p_T=x[0];
	Double_t m_T=Sqrt(p_T*p_T+m*m);
	Double_t prefactor=(n-1)*(n-2)/(n*T*(n*T+m*(n-2)));

	return prefactor*coef*p_T*Power((1+(m_T-m)/(n*T)),-n);
}


//-- MAXWELL-BOLTZMANN --
Double_t Boltzmann(Double_t *x, Double_t *par){
	Double_t coef=par[0];
	Double_t T=par[1];
	Double_t m=par[2];
	
	Float_t p_T=x[0];
	Double_t m_T=Sqrt(p_T*p_T+m*m);
   
	return coef*p_T*m_T*Exp(-m_T/T);
}


//-- POWER LAW --
Double_t PowerLaw(Double_t *x, Double_t *par){
	Float_t p_T=x[0];
	
	return par[0]+par[1]*Power(p_T,par[2]);
}


//-- HYDRODYNAMIC BLASTWAVE --

// Integral part
Double_t calcR(Double_t *x, Double_t *par){
	Double_t m=par[0];
	Double_t T=par[1];
	Double_t R=par[2];
	Double_t p_T=par[3];
	Double_t betaTMean=par[4];
	Double_t n=par[5];
	
	Float_t r=x[0];
	Double_t m_T=Sqrt(p_T*p_T+m*m);
	
	Double_t rho=ATanH((n+2)/2*betaTMean*Power(r/R,n));
	Double_t K1=BesselK1(m_T*CosH(rho)/T);
	Double_t I0=BesselI0(p_T*SinH(rho)/T);

	return m_T*I0*K1*r;
}


// Blastwave part
Double_t Blastwave(Double_t *x,Double_t *par){
	Double_t m=par[0];
	Double_t T=par[1];
	Double_t R=par[2];
	Double_t coef=par[3];
	Double_t betaTMean=par[4];
	Double_t n=par[5];
		
	Float_t p_T=x[0];
	
	TF1* f_inter = new TF1("f_inter",calcR,0,R,6);
	f_inter->FixParameter(0,m);
	f_inter->FixParameter(1,T);
	f_inter->FixParameter(2,R);
	f_inter->FixParameter(3,p_T);
	f_inter->FixParameter(4,betaTMean);
	f_inter->FixParameter(5,n);
	
	Double_t result=f_inter->Integral(0,R);
	
	return coef*p_T*result;
}


#endif
