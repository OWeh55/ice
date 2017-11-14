#ifndef TV_APP_H
#define TV_APP_H

#include <string>
#include <vector>
#include <GL/glut.h>

#include "Triangle.h"
#include "surface.h"

using namespace std;

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

void DrawTextXY(float, float, float, float, char*);

void Usage(const string& cmd, bool tasten = true);
void Usage(const string& cmd, const char* error);

/* Parser */
void ReadWorld(std::vector<Triangle>& world, const string& fname, int flags);

extern std::vector<Triangle> world;

extern GLfloat globalambient[4];

/* The specifications for 2 light sources */
extern int light_number;

extern GLfloat pos0[4];
extern GLfloat dif0[4];

extern GLfloat pos1[4];
extern GLfloat dif1[4];

#endif
