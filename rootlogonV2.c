// From Yuri, modified to work on laptop (Bo,2002)
// add config for aliroot (Bo,2004)
// add config for thermus package (Bo,2005)
// remove thermus config for undergrads (Bo,2005)
// adaptation to aliBuild + ROOT6 (antonin, 2018)

 
{
   TString sProgName = gProgName;
// TString Str_Prompt = gROOT->GetApplication()->Argv(0);
   TString Str_Prompt = gProgName;

   Str_Prompt += " [%03d] ";    
   //((TRint*)gROOT->GetApplication())->SetPrompt( Str_Prompt.Data()); 
  
   gSystem->Exec("echo Welcome, user $USER for a local session ");  
   {
     TDatime start;
     
     int idate=start.GetDate();
     int itime=start.GetTime();
     
     int year=idate/10000;
     int month=(idate%10000)/100;
     int day=idate%100;
     int hh=itime/10000;
     int mm=(itime%10000)/100;
     int ss=itime%100;
     
     TString cmonth[12];
     cmonth[ 0] = "Jan";
     cmonth[ 1] = "Feb";
     cmonth[ 2] = "Mar";
     cmonth[ 3] = "Apr";
     cmonth[ 4] = "May";
     cmonth[ 5] = "Jun";
     cmonth[ 6] = "Jul";
     cmonth[ 7] = "Aug";
     cmonth[ 8] = "Sep";
     cmonth[ 9] = "Oct";
     cmonth[10] = "Nov";
     cmonth[11] = "Dec";
    
     
     // cout << " *** Start at Date : " << day << "-" << c[month-1] << "-" << year
     //      << " Time : " << hh << ":" << mm << ":" << ss << " ***" << endl;
     // cout << endl;
     Printf(" *** Start at Date : %i-%s-%i / Time : %i:%i:%i",day, cmonth[month-1].Data(), year, hh, mm, ss);
   }
   
   
   gROOT->SetStyle("Plain");// Default white background for all plots
     
   gStyle->SetCanvasColor(10);
   gStyle->SetCanvasDefH(410);
   gStyle->SetCanvasDefW(540);
   gStyle->SetStatColor(10);
   gStyle->SetPadColor(10);
   gStyle->SetGridColor(kGray);
   gStyle->SetTitleFillColor(kWhite);
   
   // Settings for statistics information
   gStyle->SetOptFit(10111);
   gStyle->SetStatFormat("12.7g");
   gStyle->SetOptStat("iouRMen");
   gStyle->SetOptDate(0);
//   gStyle->SetStatX(0.97);
//   gStyle->SetStatY(0.98);
//   gStyle->SetStatH(0.2);
//   gStyle->SetStatW(0.3);
   
   // SetPaperSize wants width & height in cm: A4 is 20,26 & US is 20,24
   gStyle->SetPaperSize(20,26); 
   Style_t font = 42; 
   //gStyle->SetDrawBorder(0);
  
   gStyle->GetAttDate()->SetTextFont( font );
   gStyle->SetTitleFontSize(0.04);
   gStyle->SetTextSize(0.04);
   //gStyle->SetCanvasBorderMode(0);
   //gStyle->SetPadBorderMode(0);
   //gStyle->SetPadLeftMargin(0.15);
   //gStyle->SetPadTopMargin(0.08);
   //gStyle->SetPadRightMargin(0.04);
   //gStyle->SetPadBottomMargin(0.15);
//   gStyle->SetTitleSize(0.04,"xyz");
//   gStyle->SetLabelSize(0.04,"xyz");
   gStyle->SetLabelOffset(0.005,"xyz");
   gStyle->SetTitleOffset(1.1,"x");
   gStyle->SetTitleOffset(1.5,"yz");
   gStyle->SetTitleFont(font,"xyz");
   gStyle->SetLabelFont(font,"xyz");
   gStyle->SetTextFont(  font);
//    gStyle->SetTitleFont(font,"y");
//    gStyle->SetTitleFont(font,"z");
   gStyle->SetStatFont(  font);
   gStyle->SetStatFontSize(0.02);
   gStyle->SetMarkerSize(1.2);
   gStyle->SetPalette(1,0); 
   // gStyle->SetPalette(53); : 
        // NOTE :
        //      Alternative : dark body radiator, https://root.cern.ch/drupal/content/rainbow-color-map
        //      https://root.cern.ch/root/html/THistPainter.html#HP23, 51 to 56
        //     if ncolors = 51 and colors=0, a Deep Sea palette is used.
        //     if ncolors = 52 and colors=0, a Grey Scale palette is used.
        //     if ncolors = 53 and colors=0, a Dark Body Radiator palette is used.
        //     if ncolors = 54 and colors=0, a two-color hue palette palette is used.(dark blue through neutral gray to bright yellow)
        //     if ncolors = 55 and colors=0, a Rain Bow palette is used.
        //     if ncolors = 56 and colors=0, an inverted Dark Body Radiator palette is used.
   gStyle->SetEndErrorSize(2);  
   
   
   
    //      Print version
   TString ROOTSYS("$ROOTSYS");
   gSystem->ExpandPathName(ROOTSYS);
   
   // load base root libraries
        //    gSystem->Load("libCore"); 
        //    gSystem->Load("libTree");
        //    gSystem->Load("libGeom");
        //    gSystem->Load("libVMC");
        //    gSystem->Load("libPhysics");
        //    gSystem->Load("libMinuit2"); // NOTE : change, 7 Oct 2015 libMinuit -> libMinuit2
        //    
        //    // Load analysis framework libraries
        //    gSystem->Load("libSTEERBase");
        //    gSystem->Load("libESD");
        //    gSystem->Load("libAOD");
        //    gSystem->Load("libANALYSIS");
        //    gSystem->Load("libOADB");
        //    gSystem->Load("libANALYSISalice");
        //    gSystem->Load("libCORRFW");
        //    gSystem->Load("libPWGTools");
   
   Printf(" *** QAInfo: You are using machine node :   %s \t",gSystem->HostName());
   Printf(" *** QAInfo: with ROOTSYS :     %s", ROOTSYS.Data());
   
   // 1 - Start of config for ROOT	

   if(sProgName.EqualTo("root.exe")){
       
        //gSystem->SetIncludePath("-I$ALICE_ROOT/include -I$ALICE_PHYSICS/include");
            // NOTE : gSystem->SetIncludePath("-I$ROOTSYS/include");
            // -I$ROOTSYS/include seems anyway included by root itself
       
        Printf(" *** QAInfo:      with include paths : %s", gSystem->GetIncludePath() );
    }
    
    // 2 - Start of Config aliroot

   if(sProgName.EqualTo("aliroot")){

    //AliKalmanTrack::SetConvConst(100/0.299792458/0.2);
    TString ALICE_ROOT("$ALICE_ROOT");
    TString ROOTSYS("$ROOTSYS");
    TString GEANT3_ROOT("$GEANT3_ROOT");
    	gSystem->ExpandPathName(ALICE_ROOT);

        gSystem->AddIncludePath("-I$ALICE_ROOT/include -I$ALICE_PHYSICS/include");
            // NOTE : -I$ROOTSYS/include seems included by default root launch
        
        gSystem->Load("$(ALICE_ROOT)/lib/libEVGEN");
        gSystem->Load("$(ALICE_ROOT)/lib/libANALYSIS");
        //gSystem->Load("$(ROOTSYS)/lib/libEGPythia6");
        //gSystem->Load("$(ALICE_ROOT)/lib/libpythia6_4_21");
        //gSystem->Load("$(ALICE_ROOT)/lib/libAliPythia6");
        //gSystem->Load("$(ALICE_ROOT)/lib/libAliPythia8");
        

        Printf(" *** QAInfo: AliRoot setup:  %s",ALICE_ROOT.Data());
        Printf(" *** QAInfo:      with 'updated' include paths : %s", gSystem->GetIncludePath() );
        
   }
   
   // 4 -  Start of Config Thermus
  Bool_t SwitchThermus = 0;

  // Start of Config aliroot
  if(SwitchThermus && ( (sProgName.EqualTo("root.exe") )|| (sProgName.EqualTo("root"))) ){
        Printf(" ");
        
//         gSystem->Setenv("THERMUS", "/home/amaire/AliSoft/Atelier-THERMUS/Thermus-Install"");    
        Printf(" *** Info: Thermus setup: \t THERMUS is included, with path : %s ***", gSystem->Getenv("THERMUS"));
    
    gSystem->AddIncludePath("-I${THERMUS}/particles  -I${THERMUS}/main");
    Printf(" *** QAInfo:      with 'updated' include paths : %s", gSystem->GetIncludePath() );
    
    gSystem->Load("${THERMUS}/build/lib/libFunctions.so");
    gSystem->Load("${THERMUS}/build/lib/libTHERMUS.so"); 
    
    
   }
   if(!SwitchThermus && ((sProgName.EqualTo("root.exe"))||(sProgName.EqualTo("root"))) ){
        Printf(" *** Info: Thermus setup: \t THERMUS is NOT included \t ***");
   }
   Printf(" ");
  // End of Config THERMUS
 
}
