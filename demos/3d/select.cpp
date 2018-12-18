#include "select.h"

int MyMarker(int mode, int x, int y, int col, int size, const Image &img)
{
  for (int i=x-1;i<=x+1;i++)
    for (int j=y-1;j<=y+1;j++)
      if (img.inside(i,j)) 
	Marker(mode,i,j,col,size,img);
  return 0;
}

static void limits(int val,int &valmin,int &valmax,int size)
{
  valmin=val-SELWINDOWSIZE;
  if (valmin<0) valmin=0;
  valmax=valmin+2*SELWINDOWSIZE;
  if (valmax>size) valmax=size-1;
  valmin=valmax-2*SELWINDOWSIZE;
}
  

int MySelPoint(int mode,const Image &img, Image &ov,int p[2])
{
  //PointList pl = NewPointList(4);
  IPoint pt;
  int rc;
  pt=SelPoint(mode,img,rc);
  
  while (rc<0)
  {
    int xmin,ymin,xmax,ymax;
    limits(pt.X(),xmin,xmax,img->xsize);
    limits(pt.Y(),ymin,ymax,img->ysize);
    Window w(xmin,ymin,xmax,ymax);
    
    Image i1;
    i1.create(ZOOMSIZE,ZOOMSIZE,255);
    renormImg(img(w),i1);
    
    Image m1;
    m1.create(ZOOMSIZE,ZOOMSIZE,255);
    renormImg(ov(w),m1);
    
    Show(OVERLAY,i1,m1);
    pt=SelPoint(mode,i1,rc);
    pt.x=pt.x/ZOOMFAK+xmin;
    pt.y=pt.y/ZOOMFAK+ymin;
  }
  
  p[0]=pt.X();
  p[1]=pt.Y();
  
  MyMarker(1,p[0],p[1],4,img->ysize/30,ov);
  
  return 0;
}

