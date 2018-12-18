#include <stdio.h>
#include <image.h>
#include <fstream>
#include <vector>
#include <math.h>
#include <string>
#include <quatmatch.h>

#include "select.h"
#include "file.h"

using namespace std;
using namespace ice;

vector<Camera> Cam;
Matrix CAD;		   //CAD-Koordinaten der 4 Referenzpunkte als Zeilen

int channel1=0;
int channel2=1;

inline double EuDist(Vector3d v1, Vector3d v2)
{
  return (v1-v2).Length();
}

//liest Kalibrierdaten der Kamera und CAD-Koordinaten der 4 Ref.punkte
void GetCalibCad(const string &dir)
{
// for "live" images
  Cam.clear();
  GetCamera("calib0.txt",K[0],rad[0]);
  GetCamera("calib1.txt",K[1],rad[1]);
  GetGeo("geo.txt",CAD);
}

int InvLine(int x1, int y1, int x2, int y2, Image &img)
{
    Image tmp = NewImg(Max(x1,x2)+50,Max(y1,y2)+50,255);
    int x=0,y=0;
	
    Line(x1,y1,x2,y2,128,tmp);
    MyMarker(1,x1,y1,128,15,tmp);
    MyMarker(1,x2,y2,128,15,tmp);
	
    wloop(tmp,x,y)
	if (GetVal(tmp,x,y)==128)
	    PutVal(img,x,y,255-GetVal(img,x,y));
	
    return 0;
}

//transformiert Punkte (Zeilen) in Matrix ps gem. Parametern in double rad[4]
Matrix RemDistort(const Matrix &ps,const vector<double> &rad)
{
  Matrix res = Matrix(ps.rows(),3);
  double u = 0, v = 0, rd = 0, r = 0;
  double delta_r = 0, a = 0, delta_x = 0, delta_y = 0;

  for (uint c1=0; c1<ps.rows(); c1++)
  {
    if(ps[c1][2]==1){ u = ps[c1][0]; v = ps[c1][1];}
    else{ u = ps[c1][0]/ps[c1][2]; v = ps[c1][1]/ps[c1][2];}

    rd = sqrt(Sqr(rad[0]-u) + Sqr(rad[1]-v));

    r = rd/(1 + rad[2]*Sqr(rd) + rad[3]*Sqr(Sqr(rd)));

    delta_r = fabs(r-rd);

    a = fabs(rad[0]-u)/fabs(rad[1]-v);

    delta_y = sqrt(Sqr(delta_r)/(1+Sqr(a)));
    delta_x = a*delta_y;

    if(rad[0]<u){
      if(r>rd) res[c1][0] = u + delta_x;
      else res[c1][0] = u - delta_x;	
    }
    else{
      if(r>rd) res[c1][0] = u - delta_x;
      else res[c1][0] = u + delta_x;
    }

    if(rad[1]<v){
      if(r>rd) res[c1][1] = v + delta_y;
      else res[c1][1] = v - delta_y;
    }
    else{
      if(r>rd) res[c1][1] = v - delta_y;
      else res[c1][1] = v + delta_y;
    }
    res[c1][2] = 1;
  }

  return res;
}


//euklidischer Abstand zweier Spaltenvektoren (Typ Matrix)
double GetEuclidDist(const Matrix& v1, const Matrix& v2)
{
    double n=0;
    
    for (uint c1=0; c1<v1.rows(); c1++)
    {
	n += Sqr(v1[c1][0]-v2[c1][0]);
    }

    return sqrt(n);
}

double GetEuclidNorm(const Matrix& v)
{
    return GetEuclidDist(v,Matrix(v.rows(),1));
}

// Eulersche Winkel der Rotation von einer 
// Euklidischen Transformationsmatrix berechnen
void getEuler(const Matrix &m, double &a, double &b, double &g){

	//double sb = sqrt(1-Sqr(m[2][2]));
	//double a1,a2,b1,b2,g1,g2;
	
	/*a1 = atan(m[1][2]/m[0][2]);
	a2 = atan(-m[1][2]/m[0][2]);
	b1 = atan(sb/m[2][2]);
	b2 = atan(-sb/m[2][2]);
	g1 = atan(m[2][1]/m[2][0]);
	g2 = atan(-m[2][1]/m[2][0]);*/
	
	/*a1 = acos(m[1][2]/sb);
	a2 = acos(m[1][2]/-sb);
	g1 = asin(m[2][0]/sb);
	g2 = asin(m[2][0]/-sb);
	b1 = asin(sb);
	b2 = asin(-sb);
	
	if(b1<0){
		a = a1;
		b = b1;
		g = g1;
	}
	else{
		a = a2;
		b = b2;
		g = g2;	
	}*/
	b = asin(m[2][0]);
	g = asin(m[1][0]/-cos(b));
	a = asin(m[2][1]/-cos(b));
}

// 2 Matrizen mit je 4 3-D-Punktereferenzen als Zeilen
// Bestimmung von Rotation und Translation
// Transl. fest, Rot. gem. kleinsten Fehlerquadraten
Matrix GetEuclidTrans(Matrix p, Matrix ps)
{
    Matrix res = Matrix(4,4);
    Matrix temp = Matrix(4,3);
    Matrix ta = Matrix(3,1), tb = Matrix(3,1);
    Matrix p_t = Matrix(4,3), ps_t = Matrix(4,3), tl = Matrix(4,3);
    Matrix X = Matrix(4,4);
    std::vector<Matrix> Xv;
    Matrix X_sum = Matrix(4,4);
    MatrixStruct XM, X_eig;
    double *eig = NULL;
    double phi=0, m[3]={0};
    Matrix b2, b3, B;
    Matrix P, R, t;
    std::vector<Matrix> resv;
    double err=0.0, err_min=10000000;
    int err_min_ind=0;
    Matrix p1 = Matrix(4,1), p2 = Matrix(4,1);

	
    temp = p(0,0,3,2);
    p = ps(0,0,3,2);
    ps = temp;

    for (uint c1=0; c1<4; c1++)
    {
	//Punkt c1 in Urpsrung verschieben, dazu von jeder Zeile Punkt c1 abziehen
	ta[0][0] = -p[c1][0];
	ta[1][0] = -p[c1][1];
	ta[2][0] = -p[c1][2];

	tl[0][0] = p[c1][0]; tl[0][1] = p[c1][1]; tl[0][2] = p[c1][2];
	tl[1][0] = p[c1][0]; tl[1][1] = p[c1][1]; tl[1][2] = p[c1][2];
	tl[2][0] = p[c1][0]; tl[2][1] = p[c1][1]; tl[2][2] = p[c1][2];
	tl[3][0] = p[c1][0]; tl[3][1] = p[c1][1]; tl[3][2] = p[c1][2];

	p_t = p - tl;

	tb[0][0] = -ps[c1][0];
	tb[1][0] = -ps[c1][1];
	tb[2][0] = -ps[c1][2];

	tl[0][0] = ps[c1][0]; tl[0][1] = ps[c1][1]; tl[0][2] = ps[c1][2];
	tl[1][0] = ps[c1][0]; tl[1][1] = ps[c1][1]; tl[1][2] = ps[c1][2];
	tl[2][0] = ps[c1][0]; tl[2][1] = ps[c1][1]; tl[2][2] = ps[c1][2];
	tl[3][0] = ps[c1][0]; tl[3][1] = ps[c1][1]; tl[3][2] = ps[c1][2];

	ps_t = ps - tl;

	//Rotationsmatrix bestimmen

	for (uint c2=0; c2<X.rows(); c2++)
	{
	    for (uint c3=0; c3<X.cols(); c3++)
	    {
		X[c2][c3] = 0;
		X_sum[c2][c3] = 0;
	    }
	}

	for (uint c4=0; c4<4; c4++)
	{
			
	    X[0][1] = p_t[c4][0]-ps_t[c4][0];  
	    X[0][2] = p_t[c4][1]-ps_t[c4][1];  
	    X[0][3] = p_t[c4][2]-ps_t[c4][2];

	    X[1][0] = ps_t[c4][0]-p_t[c4][0];
	    X[1][2] = -ps_t[c4][2]-p_t[c4][2];
	    X[1][3] = ps_t[c4][1]+p_t[c4][1];

	    X[2][0] = ps_t[c4][1]-p_t[c4][1]; 
	    X[2][1] = ps_t[c4][2]+p_t[c4][2];                                  
	    X[2][3] = -ps_t[c4][0]-p_t[c4][0];

	    X[3][0] = ps_t[c4][2]-p_t[c4][2];
	    X[3][1] = -ps_t[c4][1]-p_t[c4][1]; 
	    X[3][2] = ps_t[c4][0]+p_t[c4][0];

	    Xv.push_back(X);
	}

	for (uint c8=0; c8<4; c8++)
	{
	    X_sum = X_sum + (!Xv.at(c8))*Xv.at(c8);
	}
	X = X_sum;

	XM = NewMatrix(MAT_DOUBLE,4,4);
	for (uint c5=0; c5<X.rows(); c5++){
	    for (uint c6=0; c6<X.cols(); c6++)
		XM->data[c5][c6] = X[c5][c6];
	}

	X_eig = NewMatrix(MAT_DOUBLE,4,4);
	eig = new double[4];

	Mateigen(XM,eig,X_eig);

	delete[] eig;

	phi = acos(X_eig->data[0][3]);
		
	m[0] = X_eig->data[1][3]/sin(phi);
	m[1] = X_eig->data[2][3]/sin(phi);
	m[2] = X_eig->data[3][3]/sin(phi);

	phi = 2*phi;

	//Printf("%f\t%f\t%f\t%f\n",Degree(phi),m[0],m[1],m[2]);
		
	//GetChar();

	b2 = Matrix(3,1);

	b2[0][0] = 1;
	b2[1][0] = 1;
	b2[2][0] = (-m[0]-m[1])/m[2];

	b2 = 1/GetEuclidNorm(b2)*b2;

	b3 = Matrix(3,1);

	b3[0][0] = -(m[1]-(b2[1][0]*m[2]/b2[2][0])) / (m[0]-(b2[0][0]*m[2]/b2[2][0]));
	b3[1][0] = 1;
	b3[2][0] = (-m[0]*b3[0][0]-m[1])/m[2];

	b3 = 1/GetEuclidNorm(b3)*b3;

	//Printf("%f\t%f\n",GetEuclidNorm(b2),GetEuclidNorm(b3)); 
	//GetChar();

	B = Matrix(3,1);

	B[0][0] = m[0];
	B[1][0] = m[1];
	B[2][0] = m[2];

	B = B || b2;
	B = B || b3;

	if(fabs(Determinant(B)+1)<fabs(Determinant(B)-1)){
	    B[0][2] = -B[0][2];
	    B[1][2] = -B[1][2];
	    B[2][2] = -B[2][2];
	}

	//Printf("%f\t%f\t%f\n",Sqr(B[0][0])+Sqr(B[1][0])+Sqr(B[2][0]),Sqr(B[0][1])+Sqr(B[1][1])+Sqr(B[2][1]),Sqr(B[0][2])+Sqr(B[1][2])+Sqr(B[2][2]));
	//Printf("%f\t%f\t%f\n",(!B)[0]*(!B)[1],(!B)[1]*(!B)[2],(!B)[0]*(!B)[2]);
	//Printf("%f\n",Determinant(B));
	//GetChar();

	P = Matrix(3,3);

	P[0][0] = 1; P[0][1] = 0;        P[0][2] = 0;
	P[1][0] = 0; P[1][1] = cos(phi); P[1][2] = sin(phi);
	P[2][0] = 0; P[2][1] = -sin(phi); P[2][2] = cos(phi);

	R = B*P*!B;

	//R.Print();
	//Printf("\n");

	t = Matrix(3,1);
	t = R*tb - ta;

	Matrix m = Matrix(4,4);	

	for (uint c7=0; c7<3; c7++){
	    for (uint c8=0; c8<3; c8++){
		m[c7][c8] = R[c7][c8];
	    }
	}
	m[0][3] = t[0][0];
	m[1][3] = t[1][0];
	m[2][3] = t[2][0];
	m[3][3] = 1;
	//m.Print();
	//Printf("\n");
	//GetChar();

	resv.push_back(m);

	//Fehler bestimmen
	//fuer jede Punktkorrespondenz Fehler summieren, dann mitteln
	err = 0.0;
	for (uint c9=0; c9<4; c9++)
	{
	    p1[0][0] = ps[c9][0];
	    p1[1][0] = ps[c9][1];
	    p1[2][0] = ps[c9][2];
	    p1[3][0] = 1;
	    
	    p2 = m*p1;
	    
	    p2[0][0] /= p2[3][0];
	    p2[1][0] /= p2[3][0];
	    p2[2][0] /= p2[3][0];
	    
	    p1[0][0] = p[c9][0];
	    p1[1][0] = p[c9][1];
	    p1[2][0] = p[c9][2];

	    err += GetEuclidDist(p1,p2);

	    /*	p1.Print();
		Printf("\n");
		p2.Print();
		Printf("\n");
		Printf("%f\n",GetEuclidDist(p1,p2));
		GetChar();*/
	}
	
	err /= 4.0;
	
	if(err<err_min)
	{
	  err_min = err;
	  err_min_ind = c1;
	}
	//Printf("%f\t%d\n",err,err_min_ind);
	//GetChar();
    }

    res = resv.at(err_min_ind);

    return res;
}


Matrix GetEuclidTransQuat(Matrix p, Matrix ps){

	TrafoDualQuaternion dq;
	Matrix pw1 = p(0,0,3,2);
	Matrix pw2 = ps(0,0,3,2);
	
	dq = estimateTransformation(pw1, pw2);

	return dq.getTransformationMatrix();
}



Matrix HomLinEquSys(const Matrix& A){

	MatrixStruct K=NULL, U=NULL, D=NULL, V=NULL;
	Matrix res;//u,d,v;
	double min_sv=10000000000000.0;
	int min_sv_ind=0;

	K = NewMatrix(MAT_DOUBLE,A.rows(),A.cols());

	for (uint c1=0; c1<A.rows(); c1++){
		for (uint c2=0; c2<A.cols(); c2++){
			K->data[c1][c2] = A[c1][c2];
		}
	}

	SingularValueDcmp(K,&U,&D,&V);

	//kleinsten SW suchen
	for (uint c4=0; c4<D->csize; c4++)
	{
	    if(D->data[c4][c4]<min_sv){
		min_sv = D->data[c4][c4];
		min_sv_ind = c4;
	    }
	}
	/*Printf("%f\t%d\n",min_sv,min_sv_ind);


	u = matrix(U->rsize,U->csize);
	for (uint c41=0; c41<U->rsize; c41++)
		for (uint c5=0; c5<U->csize; c5++) u[c41][c5] = U->data[c41][c5];

	d = matrix(D->rsize,D->csize);
	for (uint c6=0; c6<D->rsize; c6++)
		for (uint c7=0; c7<D->csize; c7++) d[c6][c7] = D->data[c6][c7];

	v = matrix(V->rsize,V->csize);
	for (uint c8=0; c8<V->rsize; c8++)
		for (uint c9=0; c9<V->csize; c9++) v[c8][c9] = V->data[c8][c9];

	matrix a = u*d*!v;

	u.Print(); Printf("\n");
	d.Print(); Printf("\n");
	v.Print(); Printf("\n");
	GetChar();*/


	res = Matrix(V->rsize,1);

	for (uint c3=0; c3<res.rows(); c3++) res[c3][0] = V->data[c3][min_sv_ind];

	return res;
}



//Stoerung mit Betrag r der Koordinaten des Punkts p 
Matrix RandomDistort(int p[2], double r){

	Matrix res = Matrix(2,1);
	int rnd = rand();
	double arc = rnd / double(32767) * 2 * PI;	//Richtung der St�ung
	
	res[0][0] = p[0] + sin(arc)*r;
	res[1][0] = p[1] - cos(arc)*r;


	return res;
}

int points, cams;

int errFunc(const Vector &funcvar,Vector &result)
{
  // Parameter aus funcvar auslesen
  int c=0;

  // 3-D-Punkte bereitstellen
  Matrix p3d = Matrix(points,3);
  for (uint c1=0;c1<points;c1++)
  {
    p3d[c1][0] = funcvar[c++];
    p3d[c1][1] = funcvar[c++];
    p3d[c1][2] = funcvar[c++];
  }
	
  // ext. Parameter ( 1 weniger als cameras )
  Matrix ePar = Matrix(cams-1,6);
  for (uint c2=0;c2<cams-1;c2++)
  {
    ePar[c2][0] = funcvar[c++];
    ePar[c2][1] = funcvar[c++];
    ePar[c2][2] = funcvar[c++];
    ePar[c2][3] = funcvar[c++];
    ePar[c2][4] = funcvar[c++];
    ePar[c2][5] = funcvar[c++];
  }
  //cout << ePar;
	
  // int. Parameter
  Matrix iPar = Matrix(cams,5);
  for (uint c3=0;c3<cams;c3++)
  {
    iPar[c3][0] = funcvar[c++];
    iPar[c3][1] = funcvar[c++];
    iPar[c3][2] = funcvar[c++];
    iPar[c3][3] = funcvar[c++];
    iPar[c3][4] = funcvar[c++];
  }
  //cout << iPar;
	
  //Verz.parameter
  Matrix rPar = Matrix(cams,4);
  for (uint c6=0;c6<cams;c6++)
  {
    rPar[c6][0] = funcvar[c++];
    rPar[c6][1] = funcvar[c++];
    rPar[c6][2] = funcvar[c++];
    rPar[c6][3] = funcvar[c++];
  }
  //cout << rPar;
	
  //Bildpunkte
  std::vector<Matrix> p2d;
  for (uint c4=0;c4<cams;c4++)
  {
    Matrix m1 = Matrix(points,2);
    for (uint c5=0;c5<points;c5++)
    {
      m1[c5][0] = funcvar[c++];
      m1[c5][1] = funcvar[c++];
    }
    //cout << m1;
    p2d.push_back(m1);
  }
	
  //Rueckprojektionsfehler
  c = 0;
  for (uint k=0;k<cams;k++){
	
    Matrix K = Matrix(3,4);
    Matrix Rt = Matrix(4,4);
    Matrix Ra = Matrix(3,3);
    Matrix Rb = Matrix(3,3);
    Matrix Rg = Matrix(3,3);
    Matrix R = Matrix(3,3);
    Distortion1 dist(rPar[k][0],rPar[k][1],rPar[k][2],rPar[k][3]);
    K[0][2] = iPar[k][0];
    K[1][2] = iPar[k][1];
    K[0][0] = iPar[k][2];
    K[1][1] = iPar[k][3];
    K[0][1] = iPar[k][4];
    K[2][2] = 1;
    if (k==0)
    {
      Rt[0][0] = 1;
      Rt[1][1] = 1;
      Rt[2][2] = 1;
      Rt[3][3] = 1;
    }
    else{
      Ra[1][1] = cos(ePar[k-1][3]);
      Ra[1][2] = sin(ePar[k-1][3]);
      Ra[2][2] = cos(ePar[k-1][3]);
      Ra[2][1] = -sin(ePar[k-1][3]);
      Ra[0][0] = 1;
      Rb[1][1] = 1;
      Rb[2][0] = sin(ePar[k-1][4]);
      Rb[0][0] = cos(ePar[k-1][4]);
      Rb[0][2] = -sin(ePar[k-1][4]);
      Rb[2][2] = cos(ePar[k-1][4]);
      Rg[0][0] = cos(ePar[k-1][5]);
      Rg[0][1] = sin(ePar[k-1][5]);
      Rg[1][1] = cos(ePar[k-1][5]);
      Rg[1][0] = -sin(ePar[k-1][5]);
      Rg[2][2] = 1;
			
      R = Rg*Rb*Ra;
			
      for (uint c5=0;c5<3;c5++)
	for (uint c6=0;c6<3;c6++)
	  Rt[c5][c6] = R[c5][c6];
      
      Rt[0][3] = ePar[k-1][0];
      Rt[1][3] = ePar[k-1][1];
      Rt[2][3] = ePar[k-1][2];
      Rt[3][3] = 1;
      //cout << Rt;
      //GetChar();
    }

    for (uint p=0;p<points;p++)
    {
			
      Matrix X = Matrix(4,1);
      Matrix x = Matrix(2,1);
      X[0][0] = p3d[p][0];
      X[1][0] = p3d[p][1];
      X[2][0] = p3d[p][2];
      X[3][0] = 1;
      x[0][0] = p2d.at(k)[p][0];
      x[1][0] = p2d.at(k)[p][1];
      dist.Rect(x[0][0],x[1][0]);
					
      Matrix xpr = Matrix(3,1);
      xpr = K*Rt*X;
			
      result[c++] = Sqr(xpr[0][0]/xpr[2][0] - x[0][0]);
      result[c++] = Sqr(xpr[1][0]/xpr[2][0] - x[1][0]);
    }
  }

  //Abstandsmasze Referenzviereck
  result[c++] = Sqr((p3d[0]-p3d[1]).Length()-1);
  result[c++] = Sqr((p3d[1]-p3d[2]).Length()-1);
  result[c++] = Sqr((p3d[2]-p3d[3]).Length()-1);
  result[c++] = Sqr((p3d[3]-p3d[0]).Length()-1);
  result[c++] = Sqr((p3d[0]-p3d[2]).Length()-sqrt(2.0));
  result[c++] = Sqr((p3d[1]-p3d[3]).Length()-sqrt(2.0));
	
  //cout << result << endl;
  //GetChar();
	
  return 0;
}

Matrix DoIt(const string &dir, bool having_calib)
{
  vector<string> images;
  Matrix res;
  int point_sel_mode = 0;
  int dist_mode = 0;
  double radius = 0;
  char bf[100];
  int p_nr = 0;
  Image img;
  Image ov;
  int pics = 0;
  int p[2] = {0,0};
  std::vector<Matrix> sel_p, sel_p_raw;
  char acc_point = 0;
  std::vector<Matrix> transCAD;
  PointList plCAD = NULL, pl_pic = NULL;
  double tr[3][3];
  std::vector<Matrix> CADplane3d;
  double n = 0;
  std::vector<Matrix> euStruct;
  Matrix Ki, x;
  std::vector<Matrix> extPar;
  std::vector<Matrix> camMatrix;
  Matrix A;

  Print("GetCalib\n");
  GetCalibCad(dir);

  Print("GetImages\n");
  Directory(images,dir+"/*.jpg");

  point_sel_mode = 0; // Input("Auswahlmodus fuer Punkte (0: anklicken; 1: automatisch mit Abfrage): ");

  p_nr = 6;//Input("Anzahl an Punkten (inkl. der 4 Referenzpunkte): ");
  Printf("\n");
      
  dist_mode = 0;//Input("Eingabe 0 - keine St�ung, 1 - alle Punkte werden gest�t, 2 - nur Ref.punkte ungest�t: ");
  if(dist_mode==1 || dist_mode==2) radius = InputD("Abstand der gest�ten Koordinaten von den gew�lten: ");
  if(dist_mode<0 || dist_mode>2 || radius==0) dist_mode = 0;

  for (unsigned int i=0;i<images.size();i++)
  {
    Printf("ReadImage %d = %s\n",i,(dir+"/"+images[i]).c_str());
    img = ReadImg(dir+"/"+images[i]);		
    ov = NewImg(img->xsize,img->ysize,255);
    ClearImg(ov);
    Show(OVERLAY,img,ov);
    
    Printf("\nBild %d ",i);
    Print(images[i]+"\n");
    //Punktliste (Matrix) fr neues Bild an Vektor v anh�gen
    Matrix m1 = Matrix(p_nr,3);

    if(!having_calib) p_nr = 4;

    for (uint c1=0; c1<p_nr; c1++)
    {

      //Punkte anklicken
      if(point_sel_mode==0) 
	if(having_calib==false){
	  MySelPoint(DEFAULT,img,ov,p);
	  if(c1<4){
	    ofstream f1(("points/"+images[i]).c_str());
	    //cout << "hier  " << bf << endl;
	    f1 << p[0] << endl << p[1] << endl;
	    f1.close();
	  }
	}
	else{
	  if(c1<4){
	    fstream f2(("points/"+images[i]).c_str(), ios::in | ios::out);
	    f2.getline(bf,100); 
	    p[0] = (int)atof(bf);
	    f2.getline(bf,100);
	    p[1] = (int)atof(bf);
	    //cout << p[0] << "  " << p[1] << endl;
	    f2.close();
	  }
	  else MySelPoint(DEFAULT,img,ov,p);
	}
      //SelPoint(DEFAULT,img,p);

      //Punkte automatisch suchen
      else {
	if(i==0){
	  //im ersten Bild Punkte anklicken
	  MySelPoint(DEFAULT,img,ov,p);
	  //SelPoint(DEFAULT,img,p);
	}
	else{
	  //FindPoint(2,img,ir,ig,ib,ov,int(sel_p.at(p_count-1)[c1][0]),int(sel_p.at(p_count-1)[c1][1]),p);
	  MyMarker(1,p[0],p[1],5,img->ysize/30,ov);
	  //Zoom(img,5,p[0],p[1]);
	  Printf("Punkt %d akzeptieren? (y/n)\n",c1+1);
	  acc_point = GetChar();
	  if(acc_point=='n' || acc_point=='N'){
	    Zoom(img,0,0,0);
	    MyMarker(1,p[0],p[1],0,img->ysize/30,ov);
	    Printf("Punkt %d anklicken\n",c1+1);
	    MySelPoint(DEFAULT,img,ov,p);
	  }
	}
      }
      if(dist_mode==0){
	m1[c1][0] = p[0];
	m1[c1][1] = p[1];
	m1[c1][2] = 1;
      }
      else{
	if(dist_mode==1){
	  Matrix pm1 = RandomDistort(p,radius);

	  m1[c1][0] = pm1[0][0];
	  m1[c1][1] = pm1[1][0];
	  m1[c1][2] = 1;
	  MyMarker(1,int(m1[c1][0]),int(m1[c1][1]),3,img->ysize/30,ov);
	}
	if(dist_mode==2){
	  if(c1<=3){
	    m1[c1][0] = p[0];
	    m1[c1][1] = p[1];
	    m1[c1][2] = 1;
	  }
	  else{
	    Matrix pm1 = RandomDistort(p,radius);

	    m1[c1][0] = pm1[0][0];
	    m1[c1][1] = pm1[1][0];
	    m1[c1][2] = 1;
	    MyMarker(1,int(m1[c1][0]),int(m1[c1][1]),3,img->ysize/30,ov);
	  }
	}
      }
				

    }
    //bei erstem Bild und automatischer Erkennung: static int r,g,b fr Tracking festlegen
    //if(point_sel_mode==1 && p_count==0) GetColVals(m1,ir,ig,ib);

    //bei automatischer Erkennung: Punkte ggf. nochmals automatisch bestimmen
    if(point_sel_mode==1){
      Printf("Punkte im aktuellen Bild nochmals automatisch bestimmen?\n");
      acc_point = GetChar();
      if(acc_point=='y' || acc_point=='Y'){
	for (uint c2=0; c2<p_nr; c2++){
	  //FindPoint(2,img,ir,ig,ib,ov,int(m1[c2][0]),int(m1[c2][1]),p);
	  MyMarker(1,p[0],p[1],5,img->ysize/30,ov);
	  //Zoom(img,5,p[0],p[1]);
	  /*Printf("Punkt %d akzeptieren? (y/n)\n",c2+1);
	    acc_point = GetChar();
	    if(acc_point=='n' || acc_point=='N'){
	    Zoom(img,0,0,0);
	    MyMarker(1,p[0],p[1],0,img->ysize/30,ov);
	    Printf("Punkt %d anklicken\n",c2+1);
	    MySelPoint(DEFAULT,img,ov,p);
	    }*/
	  m1[c2][0] = p[0];
	  m1[c2][1] = p[1];
	}
      }
    }
    Show(OFF,img);

    Printf("\n");

    if(having_calib)
    {
      Matrix m1_2 = Matrix(m1);
      sel_p_raw.push_back(m1_2);
      m1 = RemDistort(m1,rad[i]);
      sel_p.push_back(m1);

      if (i==0) 
      {
	Image urk(img);
	InvLine(int(m1[4][0]),int(m1[4][1]),int(m1[5][0]),int(m1[5][1]),urk);
	//WriteImg(urk,"urkunde/pers.jpg");
	FreeImg(urk);
      }
    }
  }

  if(!having_calib) return Matrix(1,1);

  pics = sel_p.size();

  //je Bild proj. Transformation von CAD- in Bildkoordinaten berechnen
  plCAD = NewPointList(4);
  for (uint c3=0; c3<=3; c3++){
    PutPoint(plCAD,c3,CAD[c3][0],CAD[c3][1]);
  }

  for (uint c4=0; c4<pics; c4++){
    Matrix m2 = Matrix(3,3);	//Transformationsmatrix
    pl_pic = NewPointList(4);

    for (uint c5=0; c5<=3; c5++){
      PutPoint(pl_pic,c5,sel_p.at(c4)[c5][0],sel_p.at(c4)[c5][1]);
    }

    FitProjectiveTrans(plCAD,pl_pic,tr);

    FreePointList(pl_pic);

    for (uint c6=0; c6<=2; c6++)
      for (uint c7=0; c7<=2; c7++) m2[c6][c7] = tr[c6][c7];
		
    transCAD.push_back(m2);
  }
	
  //je Bild Lage der CAD-Ebene im Raum berechnen
  //Nullpunktvektor d, Achsen e1, e2 (orthonormal)
  for (int c8=0; c8<pics; c8++){
    Matrix m3 = Matrix(3,3);	//zeilenweise Vektoren d,e1,e2 der CAD-Ebene im 3D
    Matrix KK = K[c8];

    //Vektor e1
    m3[0][2] = transCAD.at(c8)[2][0];
    m3[0][1] = (transCAD.at(c8)[1][0] - KK[1][2]*m3[0][2])/KK[1][1];
    m3[0][0] = (transCAD.at(c8)[0][0] - KK[0][1]*m3[0][1] - KK[0][2]*m3[0][2])/KK[0][0];

    //Vektor e2
    m3[1][2] = transCAD.at(c8)[2][1];
    m3[1][1] = (transCAD.at(c8)[1][1] - KK[1][2]*m3[1][2])/KK[1][1];
    m3[1][0] = (transCAD.at(c8)[0][1] - KK[0][1]*m3[1][1] - KK[0][2]*m3[1][2])/KK[0][0];

    //Vektor d
    m3[2][2] = transCAD.at(c8)[2][2];
    m3[2][1] = (transCAD.at(c8)[1][2] - KK[1][2]*m3[2][2])/KK[1][1];
    m3[2][0] = (transCAD.at(c8)[0][2] - KK[0][1]*m3[2][1] - KK[0][2]*m3[2][2])/KK[0][0];

    //Normierung
    n = 1/(sqrt(sqrt(Sqr(m3[0][0])+Sqr(m3[0][1])+Sqr(m3[0][2]))*sqrt(Sqr(m3[1][0])+Sqr(m3[1][1])+Sqr(m3[1][2]))));
    m3 = n*m3;

    //Printf("%f\n%f\n%f\n\n",sqrt(Sqr(m3[0][0])+Sqr(m3[0][1])+Sqr(m3[0][2])),sqrt(Sqr(m3[1][0])+Sqr(m3[1][1])+Sqr(m3[1][2])),m3[0][0]*m3[1][0]+m3[0][1]*m3[1][1]+m3[0][2]*m3[1][2]);
    //GetChar();

    CADplane3d.push_back(m3);
  }

  //Ki = KK.Inverse();
  x = Matrix(3,1);
  for (int c9=0; c9<pics; c9++){
    Matrix m4 = Matrix(p_nr,3);

    //4 Ref.punkte
    for (int c10=0; c10<4; c10++){
      m4[c10][0] = CAD[c10][0]*CADplane3d.at(c9)[0][0] + CAD[c10][1]*CADplane3d.at(c9)[1][0] + CADplane3d.at(c9)[2][0];
      m4[c10][1] = CAD[c10][0]*CADplane3d.at(c9)[0][1] + CAD[c10][1]*CADplane3d.at(c9)[1][1] + CADplane3d.at(c9)[2][1];
      m4[c10][2] = CAD[c10][0]*CADplane3d.at(c9)[0][2] + CAD[c10][1]*CADplane3d.at(c9)[1][2] + CADplane3d.at(c9)[2][2];
    }
    //Printf("%f\n%f\n%f\n%f\n\n",EuDist(m4[0],m4[1]),EuDist(m4[1],m4[2]),EuDist(m4[2],m4[3]),EuDist(m4[0],m4[3]));
		

    //Sehstrahlen
    /*if(p_nr>4){
      for (int c11=4; c11<p_nr; c11++){
      x[0][0] = sel_p.at(c9)[c11][0];
      x[1][0] = sel_p.at(c9)[c11][1];
      x[2][0] = sel_p.at(c9)[c11][2];

      x = Ki*x;

      m4[c11][0] = x[0][0];
      m4[c11][1] = x[1][0];
      m4[c11][2] = x[2][0];
      }
      }*/
		

    euStruct.push_back(m4);

    Printf("%f\n",EuDist(m4[0],m4[1]));
    Printf("%f\n",EuDist(m4[1],m4[2]));
    Printf("%f\n",EuDist(m4[2],m4[3]));
    Printf("%f\n",EuDist(m4[3],m4[0]));
    Printf("\n");
    //GetChar();
  }

  //externe Parameter bzgl. der ersten Kamera
  Matrix m5 = Matrix(4,4);
  for (int c12=0; c12<4; c12++) m5[c12][c12] = 1;

  extPar.push_back(m5);
  camMatrix.push_back(K[0]*m5);

  double normal[3]={0}, cam_or[3]={0}, va[3]={0}, vb[3]={0}, vc[3]={0};

  //3 Punkte va, vb, vc des Ref.vierecks, Normalenvektor normal
  va[0] = euStruct.at(0)[0][0];
  va[1] = euStruct.at(0)[0][1];
  va[2] = euStruct.at(0)[0][2];

  vb[0] = euStruct.at(0)[1][0];
  vb[1] = euStruct.at(0)[1][1];
  vb[2] = euStruct.at(0)[1][2];

  vc[0] = euStruct.at(0)[2][0];
  vc[1] = euStruct.at(0)[2][1];
  vc[2] = euStruct.at(0)[2][2];

  normal[2] = 1;
  normal[0] = -((va[2]-vb[2])-(va[2]-vc[2])*(va[1]-vb[1])/(va[1]-vc[1])) / ((va[0]-vb[0])-(va[0]-vc[0])*(va[1]-vb[1])/(va[1]-vc[1]));
  normal[1] = (-normal[0]*(va[0]-vb[0])-va[2]+vb[2]) / (va[1]-vb[1]);


  Printf("\nOrientierungen:\n");
  for (int c13=0; c13<pics; c13++){
		
    if(c13==0){
      cam_or[0] = 0;
      cam_or[1] = 0;
      cam_or[2] = 1;

      //Winkel zur Normalen des Ref.vierecks
      Printf("Kamera 1 zu Normale - Referenzviereck: %f\n", 90-Degree(acos((normal[0]*cam_or[0]+normal[1]*cam_or[1]+normal[2]*cam_or[2])/(sqrt(Sqr(normal[0])+Sqr(normal[1])+Sqr(normal[2]))*sqrt(Sqr(cam_or[0])+Sqr(cam_or[1])+Sqr(cam_or[2]))))));
    }
		
    if(c13>0){
      //Rot. u. Transl. bzgl. der ersten Kamera bestimmen
      Matrix m6 = GetEuclidTrans(euStruct.at(0),euStruct.at(c13));
      Matrix test = GetEuclidTransQuat(euStruct.at(0),euStruct.at(c13));
		
      cout << "--------------------------------" << endl;
      cout << "meins:\n" << m6 << endl;
      cout << "quat:\n" << test << endl;
      cout << "Ref1:\n" << euStruct.at(0) << endl;
      cout << "Ref2:\n" << euStruct.at(c13) << endl << "--------------------------------" << endl;
      m6 = test;

      extPar.push_back(m6);
      camMatrix.push_back(K[1]*m6);

      cam_or[0] = camMatrix.at(c13)[2][0];
      cam_or[1] = camMatrix.at(c13)[2][1];
      cam_or[2] = camMatrix.at(c13)[2][2];

      //Kamearorientierungen
      Printf("Kamera %d zu Kamera 1: %f\n", c13+1, Degree(acos(cam_or[2]/sqrt(double(Sqr(cam_or[0])+Sqr(cam_or[1])+Sqr(cam_or[2]))))));

      //Winkel zur Normalen des Ref.vierecks
      Printf("Kamera %d zur Ebene des Referenzvierecks: %f\n", c13+1, 90-Degree(acos((normal[0]*cam_or[0]+normal[1]*cam_or[1]+normal[2]*cam_or[2])/(sqrt(Sqr(normal[0])+Sqr(normal[1])+Sqr(normal[2]))*sqrt(Sqr(cam_or[0])+Sqr(cam_or[1])+Sqr(cam_or[2]))))));
    }
  }
  Printf("\n");
  //GetChar();



  //Triangulation
  //je Punkt c14
  res = Matrix(p_nr+pics,3);

  //Referenzpunkte
  for (int c16=0; c16<4; c16++){
    res[c16][0] = euStruct.at(0)[c16][0];
    res[c16][1] = euStruct.at(0)[c16][1];
    res[c16][2] = euStruct.at(0)[c16][2];
  }


  //weitere Punkte
  for (int c14=4; c14<p_nr; c14++){
		
    A = Matrix(2*pics,4);

    //je Bild c15
    for (int c15=0; c15<pics; c15++){

      A[2*c15][0]   = sel_p.at(c15)[c14][1]*camMatrix.at(c15)[2][0] - camMatrix.at(c15)[1][0];
      A[2*c15+1][0] = camMatrix.at(c15)[0][0] - sel_p.at(c15)[c14][0]*camMatrix.at(c15)[2][0];

      A[2*c15][1]   = sel_p.at(c15)[c14][1]*camMatrix.at(c15)[2][1] - camMatrix.at(c15)[1][1];
      A[2*c15+1][1] = camMatrix.at(c15)[0][1] - sel_p.at(c15)[c14][0]*camMatrix.at(c15)[2][1];
			
      A[2*c15][2]   = sel_p.at(c15)[c14][1]*camMatrix.at(c15)[2][2] - camMatrix.at(c15)[1][2];
      A[2*c15+1][2] = camMatrix.at(c15)[0][2] - sel_p.at(c15)[c14][0]*camMatrix.at(c15)[2][2];

      A[2*c15][3]   = sel_p.at(c15)[c14][1]*camMatrix.at(c15)[2][3] - camMatrix.at(c15)[1][3];
      A[2*c15+1][3] = camMatrix.at(c15)[0][3] - sel_p.at(c15)[c14][0]*camMatrix.at(c15)[2][3];
    }

    x = HomLinEquSys(A);

    res[c14][0] = x[0][0]/x[3][0];
    res[c14][1] = x[1][0]/x[3][0];
    res[c14][2] = x[2][0]/x[3][0];

    /*ofstream of ("matrixa.txt");
      of << A.rows() << "\n";
      of << A.cols() << "\n";
      of << "\n";
      for (int c16=0; c16<A.cols();c16++){
      for (int c17=0; c17<A.rows();c17++){
      of << A[c17][c16] << "\n";
      }
      of << "\n";
      }
      of.close();

      A.Print();
      Printf("\n");
      x.Print();
      Printf("\n");
      GetChar();*/
  }

  //Kamerazentren
  for (int c17=p_nr; c17<p_nr+pics; c17++){
		
    res[c17][0] = extPar.at(c17-p_nr)[0][3];
    res[c17][1] = extPar.at(c17-p_nr)[1][3];
    res[c17][2] = extPar.at(c17-p_nr)[2][3];
  }

  //------------
  //LM-Verfahren
  //____________
	
  //3-D-Punkte,externe Par., interne Par., Verz.par., Bildpunkte
  Vector var = Vector(3*p_nr + 6*(pics-1) + 5*pics + 4*pics + 2*pics*p_nr);
  points = p_nr;
  cams = pics;
	
  //3-D-Punkte eintragen
  int c=0;
  for (int c18=0;c18<p_nr;c18++){
    var[c++] = res[c18][0];
    var[c++] = res[c18][1];
    var[c++] = res[c18][2];
  }
	
  double al,be,ga;
  getEuler(extPar.at(1),al,be,ga);
  //cout << "Eulersche Winkel: " << al << "  " << be << "  " << ga << endl;
	
  //ext. Parameter eintragen (ausser 1. Kamera)
  for (int c19=1;c19<pics;c19++){
    getEuler(extPar.at(c19),al,be,ga);
    var[c++] = extPar.at(c19)[0][3];
    var[c++] = extPar.at(c19)[1][3];
    var[c++] = extPar.at(c19)[2][3];
    var[c++] = al;
    var[c++] = be;
    var[c++] = ga;
  }
  //cout << extPar.at(1);
	
  //int. Parameter eintragen
  var[c++] = K[0][0][2];
  var[c++] = K[0][1][2];
  var[c++] = K[0][0][0];
  var[c++] = K[0][1][1];
  var[c++] = K[0][0][1];
	
  var[c++] = K[1][0][2];
  var[c++] = K[1][1][2];
  var[c++] = K[1][0][0];
  var[c++] = K[1][1][1];
  var[c++] = K[1][0][1];
	
  //Verz.parameter eintragen
  var[c++] = rad[0][0];
  var[c++] = rad[0][1];
  var[c++] = rad[0][2];
  var[c++] = rad[0][3];
	
  var[c++] = rad[1][0];
  var[c++] = rad[1][1];
  var[c++] = rad[1][2];
  var[c++] = rad[1][3];
	
  //Bildkoordinaten eintragen
  for (int c20=0;c20<pics;c20++){
    for (int c21=0;c21<p_nr;c21++){
      var[c++] = sel_p_raw.at(c20)[c21][0];
      var[c++] = sel_p_raw.at(c20)[c21][1];
    }
  }
	
  //cout << var << endl << endl;
  LMDif(var,3*p_nr + 6*(pics-1),errFunc,2*pics*p_nr + 6);
  cout << "LM fertig.\n";
	
  c=0;
  Matrix resLM = Matrix(p_nr,3);
  for (int c21=0;c21<p_nr;c21++){
    resLM[c21][0] = var[c++];
    resLM[c21][1] = var[c++];
    resLM[c21][2] = var[c++];
  }
  cout << (resLM[0]-resLM[1]).Length() << endl;
  cout << (resLM[1]-resLM[2]).Length() << endl;
  cout << (resLM[2]-resLM[3]).Length() << endl;
  cout << (resLM[3]-resLM[0]).Length() << endl;
  cout << "Ergebnis mit LM: " << (resLM[4]-resLM[5]).Length() << endl;
	
  //res = resLM;
	
  //Show(OFF,img);
  //FreeImg(img);
  //FreeImg(ov);
  return res;
}

void ShowMenu(bool status=false)
{
	ClearAlpha();
	Print("                   === 3D - Messung ===\n\n");
	Print("    p - Scan new pair of images\n\n");
	Print("    c - Select four points per image for calibration\n\n");
	Print("    m - Perform measurement\n\n");
	Print("    x - Exit program\n\n");
}

void usage(const string &pname)
{
  cout << endl; // separate message from previous text
  cout << "Usage: " << endl;
  cout << "    " << pname << " <options>" << endl << endl;
  cout << "options:" << endl;
  cout << "  -c <nr>   use channel nr to scan first image" << endl;
  cout << "  -C <nr>   use channel nr to scan second image" << endl;
  cout << "  -i <name> use name as folder containing images" << endl;
  exit(1);
}

int main(int argc,char *argv[])
{

    Matrix P;
    string picfolder = "pics";

    int rc;
    
    while ((rc=getopt(argc,argv,"c:C:i:"))>=0)
    {
      switch(rc)
      {
	case 'c': channel1=atol(optarg); break;
	case 'C': channel2=atol(optarg); break;
	case 'i': picfolder=optarg; break;
	case 'h':
	default:
	  usage(argv[0]); break;
      }
    }


    //Bilder anlegen
    Image i0(640,480,255), i1(640,480,255);

    char choice = 'o';

    Alpha(ON);
    ClearAlpha();
		
    while (choice != 'x')
    {
	ShowMenu();
	
	choice = tolower(GetChar());
	ClearAlpha();

#if 0
	Printf("%c",choice);
	Delay(1.0);
#endif

	if (choice=='p')
	{
	    Show(ON,i0,"Picture 1");
	    Show(ON,i1,"Picture 2");

	    ScanImg(channel1,i0,false);
	    WriteImg(i0,picfolder+"/00.jpg");

	    ScanImg(channel2,i1,false);
	    WriteImg(i1,picfolder+"/01.jpg");

	    Show(OFF,i0);
	    Show(OFF,i1);
	}

	else if(choice=='c')
	{
	    P = DoIt(picfolder,false);

	}
	else if(choice=='m')
	{
	    P = DoIt(picfolder,true);

	    Printf("\n");
	    if (P.rows()>5)
	    {
		Printf("%f\n",EuDist(P[4],P[5]));
		/*fstream f("urkunde/groesse");
		  f << Round(100*EuDist(P[4],P[5]))/100.0 << endl;
		  f.seekg(0);
		  f.getline(bf,100);
		  string s(bf);
		  int pos = s.find(".",0);
		  s.erase(pos,1);
		  s.insert(pos,",");
		  f.close();
		  fstream f1("urkunde/groesse");
		  f1 << s << endl;
		  f1.close();*/
	    }
	    GetChar();
	}
    }

    Alpha(OFF);
    return 0;
}
