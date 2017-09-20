#ifndef stringtools_h
#define stringtools_h

#include <string>

using namespace std;

string Trim(const string& s);
string Split(string& s, const string& ss);

bool iswhite(char c);
bool numberc(char c);
bool alpha(char c);

string ToString(float val, unsigned int width = 0, unsigned int prec = 0);

#endif
