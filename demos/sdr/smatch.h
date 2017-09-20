#ifndef SMATCH_H
#define SMATCH_H

using namespace ice;

#define MAXIMAGES 2000

void Peak(Image imgt, int mode, double& xmax, double& ymax, double& eval, int mdist);

// Peak - mode
// MIN
#define P_MIN 1
#define P_MAX 2

// POSITIVE, NEGATIVE
#define P_POSITIVE 4
#define P_NEGATIVE 8

#define P_INTERPOL 16
#define P_EVAL 32

#define DT_NEARLY_SHIFT 1
#define DT_REFINE 16
int DetectTrafo1(Image img1, Image img2,
                 Trafo& tr,
                 double beta = 0.1, int iter = 5, int mode = DT_NEARLY_SHIFT);

int DetectTrafo1(Image img1, Image img2, Image himg,
                 Image mark,
                 Trafo& tr,
                 double beta = 0.1, int iter = 5, int mode = DT_NEARLY_SHIFT);

extern bool cross;
extern char waitc;
void Wait(int level);

extern bool ignoreglobal;

#define GLOBALSHIFTSIZE 1024
extern Image Iglobalshift;

#define LOCALSHIFTSIZE 128
extern Image Ilocalshift;

#endif
