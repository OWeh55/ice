#ifndef EXIFSIG_H
#define EXIFSIG_H

#include <string>

bool getExifSig(const std::string &file,
                std::string &exif, double &focallength);

bool getExifSig(const std::string &file, const std::string &modifier,
                std::string &exif, double &focallength);

#endif
