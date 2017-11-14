#ifndef CVIEWER_H
#define CVIEWER_H

#include <string>
#include <GL/glut.h>

#include "arith.h"
#include "surface.h"

using namespace std;

#include "object.h"

void Display(void);
void CreateEnvironment(void);

void MakeGeometry(void);
void MakeLighting(void);
void MakeCamera();
void Home();

void HandleKeyboard(unsigned char key, int x, int y);
void HandleSpecialKeyboard(int key, int x, int y);
void HandleMouse(int, int, int, int);
void HandleReshape(int, int);
void HandleMainMenu(int);
void HandleAngleMenu(int);
void HandleSpeedMenu(int);
void HandleVisibility(int vis);
void HandleIdle(void);

void DrawTextXY(float, float, float, float, char *);

void Usage(const string &cmd, bool tasten = true);
void Usage(const string &cmd, const char *error);

/* Parser */
void ReadWorld(const string &fname);

extern objekt *world;

int yylex(void);
extern int yydebug;

extern GLfloat globalambient[4];

/* The specifications for 2 light sources */
extern int light_number;

extern GLfloat pos0[4];
extern GLfloat dif0[4];

extern GLfloat pos1[4];
extern GLfloat dif1[4];

extern texture *text;
#endif
