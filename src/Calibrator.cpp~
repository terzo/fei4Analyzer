#include "Calibrator.h"

Calibrator::Calibrator(void )
{
  int i,j;

  //TH1F* dummy = new TH1F("dummy", "dummy", 10, 0, 1);

  char infileName2[] = "./data/TOT_TEST_002108/analysis.root";
  TFile *infile2 = new TFile(infileName2,"READ");
  if(!infile2->IsOpen())
  { 
    std::cout << "ERROR: no file " << infileName2 << " found" << std::endl;
    exit(0);
  } 
  else 
  {
   std::cout << "file : " << infileName2 << " loaded" <<std::endl;
  }

  //infile2->ls();
  std::string histoName2 = "ToT_Mean_Mod_1";
  TH2* h2 = (TH2*)infile2->FindObjectAny(histoName2.c_str());
  if(h2==NULL)
  {
    std::cout << "ERROR: no histogram " << histoName2 << " found" << std::endl;
    exit(0);
  } 
  
  for (i=1; i<81; i++)
  { 
   	for (j=1; j<337; j++) 
   	{
    		 totmap_[i] [j] = h2->GetCellContent (i,j);
   	}
  }
  std::cout<<"totmap feddich" << std::endl;

  infile2->Close();
  delete  infile2;

}
//=====================================================================
double Calibrator::calib(EventMaker::hitDef hit)
{
   double charge = 0;
   int col = hit.col;
   int row = hit.row;
   if (totmap_[col][row]>0) 
   {
     charge = hit.tot * 6. / totmap_ [col] [row];    
   }

   return charge;
}
