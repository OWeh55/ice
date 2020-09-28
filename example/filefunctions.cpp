#include "filefunctions.h"

bool fexist(const string& fn)
{
  struct stat buf;
  int rc = stat(fn.c_str(), &buf);
  return rc == 0;
}

string filenameFromMask(const string& mask, int nr)
{
  char buf[1000];
  snprintf(buf, 1000, mask.c_str(), nr);
  return buf;
}

bool isMask(const string& mask)
{
  return filenameFromMask(mask, 1) != filenameFromMask(mask, 2);
}

string findFilename(const string& mask)
{
  string fileMask = mask;
  static int FileNr = 0;
  string filename = mask;
  if (!isMask(mask)) // filename without %d
    {
      if (!fexist(filename))
        return mask;
      string path, basename, ext;
      SplitFilename(mask, path, basename, ext);
      if (path.empty())
        fileMask = basename + "%04d." + ext;
      else
        fileMask = path + "/" + basename + "%04d." + ext;
    }
  filename = filenameFromMask(fileMask, FileNr++);
  while (fexist(filename))
    {
      filename = filenameFromMask(fileMask, FileNr++);
    }
  return filename;
}
