#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>

#include "filelist.h"

std::string PathAndFilename(const std::string &path, const std::string &filename)
{
  std::string result = path;
  if (result[result.length() - 1] != '/')
    result += "/";
  result += filename;
  return result;
}

void FileList(const std::string &fn, std::vector<std::string> &file, bool recursive)
{
  struct stat fstat;
  if (stat(fn.c_str(), &fstat) != 0)
    return;
  if (S_ISREG(fstat.st_mode))
    {
      file.push_back(fn);
    }
  else if (S_ISDIR(fstat.st_mode))
    {
      struct dirent  *dp;
      DIR *dir;
      if ((dir = opendir(fn.c_str())) != NULL)
        {
          /* Loop through directory entries. */
          while ((dp = readdir(dir)) != NULL)
            {
              std::string fname = dp->d_name;
              std::string pname = PathAndFilename(fn, fname);
              if (stat(pname.c_str(), &fstat) == 0)
                {
                  if (S_ISDIR(fstat.st_mode))
                    {
                      if (recursive)
                        if (fname != "." && fname != "..")
                          FileList(pname, file, true);
                    }
                  else if (S_ISREG(fstat.st_mode))
                    {
                      file.push_back(pname);
                    }
                }
            }
          closedir(dir);
        }
    }
}

void FileList(int argc, char **argv,
              int optind, std::vector<std::string> &file, bool recursive)
{
  for (int i = optind; i < argc; i++)
    {
      FileList(argv[i], file, recursive);
    }
}
