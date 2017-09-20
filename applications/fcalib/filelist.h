#ifndef FILELIST_H
#define FILELIST_H

#include <string>
#include <vector>

void FileList(const std::string &fn, std::vector<std::string> &file,
              bool recursive = false);

void FileList(int argc, char **argv,
              int optind, std::vector<std::string> &file,
              bool recursive = false);
#endif
