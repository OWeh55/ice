#include <image.h>

#define GFAK 100
#define TBASE 1000

void update(int &val1,int val2,int rel)
{
  int irel=TBASE-rel;
  val1=(val1*irel+val2*rel)/TBASE;
}

void putlimited(Image &img,int x,int y,int val)
{
    PutVal(img,x,y,Min(Max(val,0),img->maxval));	    
}

void putrgb(Image &r,Image &g,Image &b,int x,int y,int rv,int gv,int bv)
{
    putlimited(r,x,y,rv);
    putlimited(g,x,y,gv);
    putlimited(b,x,y,bv);
}

void usage(const string &p)
{
  cout << p << "  Version 0.7" << endl << endl;
  cout << "Usage:  " ;
  cout << p << " [<options>]" << endl << endl;
  cout << "Options:"<< endl << endl;
  cout << "  -h       (this) help" << endl;
  cout << "  -t <val> time constant for akkumulation (in frames)" << endl;
  cout << "  -c <val> scan Channel" << endl;
  cout << "  -s       switch on timing" << endl;
  cout << "  -i       don't init result" << endl;
  cout << "  -d       show diff" << endl;
  exit(1);
}

int main(int argc,char *argv[])
{
    int channels;
    int xsize, ysize;
    int maxval;
    int flags;
    string descr;
    int channel=0;
    int tc=10;
    bool timing=false;
    bool init=true;
    
// what to show
    bool diff=false;
    bool sum=true;
    bool seg=false;
    int sig=0;

    int rc;
  
    while ((rc=getopt(argc,argv,"c:t:sidm:"))>=0)
    {
	switch(rc)
	{
	    case 't': tc=atol(optarg); break;
	    case 'c': channel=atol(optarg); break;
	    case 's': timing=true; break;
	    case 'i': init=false; break;
	    case 'd': diff=true; sum=false; break;
	    case 'm': seg=true; sig=atol(optarg); break;
	    case 'h': // fall through
	    default: usage(argv[0]); break;
	}
    }

    // Wieviele kanäle gibt es?

    ScanInfo(0,xsize,ysize,maxval,channels,flags,descr);

    if (channels==0)
    {
	cerr << "Keine Bildaufnahme" << endl;
	exit(1);
    }

    if (channel>=channels)
    {
	cerr << "Aufnahmekanal nicht vorhanden" << endl;
	exit(1);
    }

    ScanInfo(0,xsize,ysize,maxval,channels,flags,descr);
    if ( (!(flags & SC_SCAN)) &&
	 (!(flags & SC_RGB)))
    {
	cerr << "Keine Farbbildaufnahme möglich" << endl;
	exit(1);
    }

    Image r,g,b;
  
    r=NewImg(xsize,ysize,maxval);
    g=NewImg(xsize,ysize,maxval);
    b=NewImg(xsize,ysize,maxval);

    Image rs,gs,bs;
    Image rd,gd,bd;
    Image re,ge,be;
  
    rs=NewImg(xsize,ysize,maxval*GFAK);
    gs=NewImg(xsize,ysize,maxval*GFAK);
    bs=NewImg(xsize,ysize,maxval*GFAK);

    if (diff)
    {
	rd=NewImg(xsize,ysize,maxval*GFAK);
	gd=NewImg(xsize,ysize,maxval*GFAK);
	bd=NewImg(xsize,ysize,maxval*GFAK);
    }

    if (seg)
    {
	re=NewImg(xsize,ysize,maxval);
	ge=NewImg(xsize,ysize,maxval);
	be=NewImg(xsize,ysize,maxval);
    }

    if (diff)
	Show(_RGB,rd,gd,bd,"Diff");

    if (sum)
	Show(_RGB,rs,gs,bs,"Summ");

    if (seg)
	Show(_RGB,re,ge,be,"Seg");

    if (init)
	ScanImg(channel,rs,gs,bs,false); //Startbelegung, damit es besser aussieht

    double tstart=TimeD();
    int frames=0;
    while (GetKey()!=27)
    {
	ScanImg(channel,r,g,b,false);
	frames++;

	Printf("Frame %d",frames);

	if (timing) 
	    Printf(", %5.2f ms/frame",(TimeD()-tstart)*1000.0/frames);

	Printf("\n");

	int x,y;
	wloop(r,x,y)
	    {
		int rv=GetVal(rs,x,y);
		int ra=GetVal(r,x,y);
		int rx=ra*GFAK;;

		int gv=GetVal(gs,x,y);
		int ga=GetVal(g,x,y);
		int gx=ga*GFAK;;

		int bv=GetVal(bs,x,y);
		int ba=GetVal(b,x,y);
		int bx=ga*GFAK;;

		update(rv,rx,tc);
		update(gv,gx,tc);
		update(bv,bx,tc);

		putrgb(rs,gs,bs,x,y,rv,gv,bv);

		if (diff)
		    putrgb(rd,gd,bd,x,y,rx-rv+maxval*GFAK/2,gx-gv+maxval*GFAK/2,bx-bv+maxval*GFAK/2);
		
		if (seg)
		{
		    int dd=abs(rx-rv)+abs(gx-gv)+abs(bx-bv);
//		    cout << dd << endl;
		    if (dd>sig)
			putrgb(re,ge,be,x,y,ra,ga,ba);	    
		    else
			putrgb(re,ge,be,x,y,255,255,127);	    
		}
	    }
    }
    if (sum) Show(OFF,rs);
    if (seg) Show(OFF,re);
    if (diff) Show(OFF,rd);

//  GetChar();
    return OK;
}
