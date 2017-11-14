#include <image.h>

#define SIZEX 640
#define SIZEY 480

void usage(const string &p)
{
  cout << p << "  Version 0.7" << endl << endl;
  cout << "Usage:  " ;
  cout << p << " [<options>]" << endl << endl;
  cout << "Options:"<< endl << endl;
  cout << "  -h       (this) help" << endl;
  cout << "  -n <val> Neigbourhood" << endl;
  cout << "  -t <val> Threshold" << endl;
  cout << "  -T <val> High threshold (for bilevel method)" << endl;
  cout << "  -c <val> scan Channel" << endl;
  cout << "  -l <val> Line distance for Contur"<<endl;
  cout << "  -s       switch on timing" << endl;
  cout << "  -a <val> number of cycles to run (default:-1 = endless)" << endl;
  exit(1);
}

int main(int argc,char *argv[])
{
  int rc;
  int neighbour=11;
  int threshold=25;
  int thresholdh=0;
  int channel=0;
  bool timing=false;
  int counter=-1;
  int dist=3;

  while ((rc=getopt(argc,argv,"a:hT:n:t:c:sl:"))>=0)
  {
    switch(rc)
    {
      case 'a': counter=atol(optarg); break;
      case 'n': neighbour=atol(optarg) | 1; break;
      case 't': threshold=atol(optarg); break;
      case 'T': thresholdh=atol(optarg); break;
      case 'c': channel=atol(optarg); break;
      case 'l': dist=atol(optarg); break;
      case 's': timing=true; break;
      case 'h':
      default:
	usage(argv[0]);
	break;
    }
  }

  Image img1(SIZEX,SIZEY,255);
  ClearImg(img1);
  Show(ON,img1);

  Image img2(SIZEX,SIZEY,255);
  ClearImg(img2);
  Show(OVERLAY,img2);

  Image img3(SIZEX,SIZEY,255);
  ClearImg(img3);
  Show(OVERLAY,img3);
  
  Image mrk(SIZEX,SIZEY,255);

//  ScanImg(channel,img1,false);

  double start=TimeD();
  int i=0;
  while (counter!=0)
  {
    int x,y;
    Printf("Cycle %d ",i);

    ScanImg(channel,img1,false);

//    DoBImg(img1,img1,1,15,SMD_POSITIVE);

    if (thresholdh>threshold)
      LocalSeg(img1,img2,neighbour,thresholdh,threshold);
    else
      LocalSeg(img1,img2,neighbour,threshold);

    IPoint ps(0,0);
    
    ClearImg(mrk);

    if (thresholdh>threshold)
    {
      while (SearchStart(img2,mrk,LocalSegObjHigh,15,10,ps)==OK)
      {
	Contur c=CalcContur(img2,mrk,LocalSegObj,15,ps,isunknown,isunknown);
	if (c.isClosed())
	{
	  if (c.isHole()) FillRegion(c,2,mrk);
	  else FillRegion(c,3,mrk);
      }
	else
	  PutVal(mrk,ps.X(),ps.Y(),2);
      }
    }
    else
    {
      while (SearchStart(img2,mrk,LocalSegObj,15,10,ps)==OK)
      {
	Contur c=CalcContur(img2,mrk,LocalSegObj,15,ps);
	if (c.isClosed() )
	{
	    if (c.isHole()) MarkContur(c,2,mrk);
	    else FillRegion(c,3,mrk);
	    }
	else
	  PutVal(mrk,ps.X(),ps.Y(),1);
      }
    }
    wloop(mrk,x,y)
      {
	int m=GetVal(mrk,x,y);
	if ((m==3)||(m==2)) PutVal(img3,x,y,m);
	else PutVal(img3,x,y,0);
      }
    if (timing)
      Printf(" %5.2f ms/cycle\n",(TimeD()-start)*1000 / (i+1));
    else
      Printf("\n");

    if (counter>0) counter--;
    i++;
  }

  Printf("\nReady.\nPress <ENTER>");
  GetChar();
  return OK;
}
