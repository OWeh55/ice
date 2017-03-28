#include<stdio.h>
#include<math.h>
#include<image.h>

int main(int argc, char* argv[])
{
  int dimx, dimy, farbe;
  int koord[2];
  int p_in[3][2];
  double circle_par[3];
  double c_seg_par1[2], c_seg_par2[2];
  double moment[21], s[2];
  double t_p[4][2];
  double sh;
  double x0, y0, radius, kreis[3], ell_par[5];
  double seg_par1[2], seg_par2[2];
  double sup_c1, sup_c2, sup_tr1[3][3], sup_tr2[3][3];
  double sup_f1, sup_f2;
  int x1, y1, x2, y2, x3, y3, x4, y4;

  Image pic1;
  Image mark1;
  Contur cont1, cont2, cont1_sh;
  int ient, zyklus;
  TextList men;
  Segment Seg1;
  PointList pl;
  /************************************************************/
  OpenAlpha("MomentFit");
  SetAttribute(0, 7, 0, 0);
  ClearAlpha();
  Printf("Fitting momentenbasiert  !!!!!!!\n");
  Printf("\n\n");
  Printf("Nachdem Sie y für yes  eingeben werden, öffnet sich ein Grafikfenster.\n");
  Printf("Zeichnen Sie dann mit der linken Maustaste ein geschlossenes Objekt\n");
  Printf("Drücken Sie die rechte Maustaste, dann wird das Objekt geschlossen\n");
  Printf("Anschließend können Sie in einem Menü die Fittingmethode auswählen\n");
  Printf("\n\n");
  Printf("Nun drücken Sie auf der Tastatur auf  y\n");
  GetChar();
  dimx = 512;
  dimy = 512;
  pic1 = NewImg(dimx, dimy, 255);
  mark1 = NewImg(dimx, dimy, 255);
  Display(ON);
  Show(OVERLAY, pic1, mark1);
  /***********************************************************/

  //  ReadImg("fit_ell.tif",pic1);
  cont1 = SelContur(pic1, TRUE);

  for (zyklus = 0; zyklus < 1; ++zyklus)
    {
      if (zyklus == 0)
        {
          MomentRegion(cont1, moment, s);
          FillRegion(cont1, 80, pic1);
        }
      else
        {
          cont1 = SelContur(pic1);
          MomentRegion(cont1, moment, s);
          FillRegion(cont1, 80, pic1);

          //  SelPoint(DEFAULT,mark1,koord);
          //  InitTrans(sh_tr);
          //  sh_x=(double)koord[0]-s[0];
          //  sh_y=(double)koord[1]-s[1];
          //  ShiftTrans(sh_x,sh_y,sh_tr);
          //  cont1_sh=TransContur(cont1,sh_tr);
          //  MomentRegion(cont1_sh,moment,s_sh);
          //  FillRegion(cont1_sh,31,pic1);
          //  cont1=cont1_sh;
        }

      /**********************************************/
      OpenAlpha("Test");
      SetAttribute(7, 1, 0, 0);
      ClearAlpha();
      SetAlphaCursor(10, 0);
      Printf("Momentbased Fitting\n");

      men.push_back("Fitting of a triangle");
      men.push_back("Fitting of a parallelogram");
      men.push_back("Fitting of a rectangle");
      men.push_back("Fitting of a circle");
      men.push_back("Fitting of a circular segment");
      men.push_back("Fitting of a ellipse");
      men.push_back("Fitting of a elliptical segment");
      men.push_back("Fitting of a super quadric");
      men.push_back("Segmentation of the contour (by Paul Rosin)");
      men.push_back("Segmentation of the contour (by Klaus Voss)");

      ient = Menu(men, 5, 2, 60, 20) + 1;

      SetAlphaCursor(0, 20);

      /*************************************************/
      /*************************************************/

      if (ient == 2)
        {
          FitParallelogramMoments(moment, t_p);
          x1 = (int)t_p[0][0];
          y1 = (int)t_p[0][1];
          x2 = (int)t_p[1][0];
          y2 = (int)t_p[1][1];
          x3 = (int)t_p[2][0];
          y3 = (int)t_p[2][1];
          x4 = (int)t_p[3][0];
          y4 = (int)t_p[3][1];
          Line(x1, y1, x2, y2, 255, DEFAULT, pic1);
          Line(x2, y2, x3, y3, 255, DEFAULT, pic1);
          Line(x3, y3, x4, y4, 255, DEFAULT, pic1);
          Line(x4, y4, x1, y1, 255, DEFAULT, pic1);
        }

      if (ient == 1)
        {

          FitTriangleMoments(moment, t_p);
          x1 = p_in[0][0] = (int)t_p[0][0];
          y1 = p_in[0][1] = (int)t_p[0][1];
          x2 = p_in[1][0] = (int)t_p[1][0];
          y2 = p_in[1][1] = (int)t_p[1][1];
          x3 = p_in[2][0] = (int)t_p[2][0];
          y3 = p_in[2][1] = (int)t_p[2][1];
          Line(x1, y1, x2, y2, 255, DEFAULT, pic1);
          Line(x2, y2, x3, y3, 255, DEFAULT, pic1);
          Line(x3, y3, x1, y1, 255, DEFAULT, pic1);
        }

      if (ient == 3)
        {
          FitRectangleMoments(moment, t_p);
          x1 = (int)t_p[0][0];
          y1 = (int)t_p[0][1];
          x2 = (int)t_p[1][0];
          y2 = (int)t_p[1][1];
          x3 = (int)t_p[2][0];
          y3 = (int)t_p[2][1];
          x4 = (int)t_p[3][0];
          y4 = (int)t_p[3][1];
          Line(x1, y1, x2, y2, 255, DEFAULT, pic1);
          Line(x2, y2, x3, y3, 255, DEFAULT, pic1);
          Line(x3, y3, x4, y4, 255, DEFAULT, pic1);
          Line(x4, y4, x1, y1, 255, DEFAULT, pic1);
        }

      if (ient == 4)
        {
          FitCircleMoments(moment, x0, y0, radius);
          kreis[0] = x0;
          kreis[1] = y0;
          kreis[2] = radius;
          DrawCircle(kreis, 255, 255, NOFILL, pic1);
        }

      if (ient == 5)
        {
          FitCircularSegmentMoments(moment, circle_par, c_seg_par1, c_seg_par2);
          DrawCircle(circle_par, 255, 255, NOFILL, pic1);
          Line((int)c_seg_par1[0], (int)c_seg_par1[1], (int)c_seg_par2[0], (int)c_seg_par2[1], 255, DEFAULT, pic1);
        }

      if (ient == 6)
        {
          FitEllipseMoments(moment, ell_par);
          DrawEllipse(ell_par, 255, 255, NOFILL, pic1);
        }

      if (ient == 7)
        {
          sh = FitEllipticalSegmentMoments(moment, ell_par, seg_par1, seg_par2);
          DrawEllipse(ell_par, 255, 255, NOFILL, pic1);
          Line((int)seg_par1[0], (int)seg_par1[1], (int)seg_par2[0], (int)seg_par2[1], 255, DEFAULT, pic1);

        }

      if (ient == 8)
        {
          FitSuperEllipseMoments(moment, sup_c1, sup_f1, sup_tr1, sup_c2, sup_f2, sup_tr2);

          //Printf("Superquadrik c1 f1 %f %f \n",sup_c1,sup_f1);
          //Printf("Superquadrik c2 f2 %f %f \n",sup_c2,sup_f2);
          if (zyklus == 0)
            {
              DrawSuperEllipse(sup_c1, sup_tr1, 255, pic1);
            }

          if (zyklus == 1)
            {
              DrawSuperEllipse(sup_c2, sup_tr2, 255, pic1);
            }
        }

      if (ient == 9)
        {
          Printf("Fitting by the algorithm of Paul Rosin\n");
          farbe = 1;
          pl = ConturPointList(cont1, 1);
          Seg1 = SegmentPointList(pl, SPL_ALL);

          while (Seg1->prev) Seg1 = Seg1->prev;

          if (Seg1 == NULL) Printf("Rosin nicht möglich\n");

          while (Seg1 != NULL)
            {
              if (Seg1->typ == 1)
                {
                  x1 = (int)Seg1->p0[0];
                  y1 = (int)Seg1->p0[1];
                  x2 = (int)Seg1->p1[0];
                  y2 = (int)Seg1->p1[1];
                  Line(x1, y1, x2, y2, farbe, DEFAULT, mark1);
                }

              if (Seg1->typ == 2)
                {
                  CircleSegment(Seg1->par, farbe, farbe, NOFILL, mark1);
                }

              if (Seg1->typ == 3)
                {
                  EllipseSegment(Seg1->par, farbe, farbe, NOFILL, mark1);
                }

              Seg1 = Seg1->next;
              farbe = (farbe % 3) + 1;
            }
        }

      if (ient == 10)
        {
          Printf("Fitting by the algorithm of Klaus Voss\n");
          farbe = 1;
          pl = ConturPointList(cont1, 1);
          Seg1 = SegmentPointList(pl, SPL_ALL | SPL_BIDIRECT, 3.0);

          if (Seg1 == NULL) Printf("Segment. Voss nicht möglich\n");

          while (Seg1->prev) Seg1 = Seg1->prev;

          while (Seg1 != NULL)
            {
              if (Seg1->typ == 1)
                {
                  x1 = (int)Seg1->p0[0];
                  y1 = (int)Seg1->p0[1];
                  x2 = (int)Seg1->p1[0];
                  y2 = (int)Seg1->p1[1];
                  Line(x1, y1, x2, y2, farbe, DEFAULT, mark1);
                }

              if (Seg1->typ == 2)
                {
                  CircleSegment(Seg1->par, farbe, farbe, NOFILL, mark1);
                }

              if (Seg1->typ == 3)
                {
                  EllipseSegment(Seg1->par, farbe, farbe, NOFILL, mark1);
                }

              Seg1 = Seg1->next;
              farbe = (farbe % 3) + 1;
            }
        }
    }

  SelPoint(DEFAULT, mark1, koord);
  //printf("Bild ausgeben???   (y/n)");
  //scanf("%s",zk);
  //if (strcmp(zk,"y")==0)
  //  {
  //    WriteImg(pic1,"fit_ell.tif");printf("Ausgabe des Bildes erfolgt \n");
  //  }
  Display(OFF);

  return 0;
}






























