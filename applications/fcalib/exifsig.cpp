#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <string>

#include "exifsig.h"

using namespace std;

string exiftool(const string& fn, const string para)
{
  string cmd = "exiftool " + para + " " + fn;
  FILE* p = popen(cmd.c_str(), "r");
  char res[3000];
  if (fgets(res, 2999, p) != NULL)
    {
      int n = strlen(res);
      if (res[n - 1] == '\n')
        res[n - 1] = 0;
      if (res[0] == '\'')
        {
          res[0] = 0;
          res[n - 2] = 0;
          return res + 1;
        }
      else
        return res;
    }
  else
    return "";
}

bool getExifSig(const string& file, const string& modifier,
                string& exif, double& focallength)
{
  string FocalLength = exiftool(file, "-m -p '$focalLength'");
//  cout << exif.FocalLength << endl;
  if (!FocalLength.empty())
    {
      focallength = atof(FocalLength.c_str());
    }
  else
    focallength = 0.0;
  string Model = exiftool(file, "-m -p '$Model'");
//  cout << exif.Model << endl;
  string Lens = exiftool(file, "-m -p '$Lens'");
  if (Lens.empty())
    Lens = exiftool(file, "-m -p '$LensModel'");
  if (Lens.empty())
    Lens = exiftool(file, "-m -p '$LensType'");
//  cout << exif.Lens << endl;
  string Maker = exiftool(file, "-m -p '$Make'");
//  cout << exif.Maker << endl;
// brennweite wird extra behandelt, um interpolieren zu können
  exif = Model + "#" + Lens + "#" + Maker;

  if (!modifier.empty())
    exif += "#" + modifier;

  for (unsigned int i = 0; i < exif.length(); i++)
    if (exif[i] == ' ')
      exif[i] = '_';

  return !(FocalLength.empty() && Model.empty() && Lens.empty() && Maker.empty());
}

bool getExifSig(const string& file,
                string& exif, double& focallength)
{
  return getExifSig(file, exif, focallength);
}
