#include <fstream>
#include "file.h"

void GetCamera(const string &fn,Matrix &m,vector<double> &rad)
{
  // read initrinsic parameter of camera
  double val;

  // initialise camera data
  m = Matrix(3,4);
  rad = vector<double>(4);
  m[2][2] = 1;

  // open file 
  ifstream f(fn.c_str());
  int c=0;
		
  // read all values
  while (f>>val) 
  {
    if (c==0) m[0][0] = val;
    if (c==1) m[1][1] = val;
    if (c==2) m[0][1] = val;
    if (c==3) m[0][2] = val;
    if (c==4) m[1][2] = val;

    if (c==5) rad[0] = val;
    if (c==6) rad[1] = val;
    if (c==7) rad[2] = val;
    if (c==8) rad[3] = val;
		
    c++;
  }
}

void GetGeo(const string &fn,Matrix &m)
{
  int c=0;
  double val=0;
	
  m = Matrix(4,2);
  
  ifstream f2(fn.c_str());

  while(f2 >> val){
    if(c==0) m[0][0] = val;
    if(c==1) m[0][1] = val;
    if(c==2) m[1][0] = val;
    if(c==3) m[1][1] = val;
    if(c==4) m[2][0] = val;
    if(c==5) m[2][1] = val;
    if(c==6) m[3][0] = val;
    if(c==7) m[3][1] = val;

    c++;
  }
}

