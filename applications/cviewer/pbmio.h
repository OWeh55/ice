#ifndef pbm_h
#define pbm_h

#include <string>

struct EXFILE
{
  FILE *fd;
  bool process;
};

EXFILE exopen(const std::string &path, const char *mode);
bool exclose(const EXFILE &df);

// lese und schreibmodi

#ifdef WIN32
#define FWMODUS "wb"
#define FRMODUS "rb"
#else
#define FWMODUS "w"
#define FRMODUS "r"
#endif

bool ReadPBM8(const std::string &str, unsigned char *&buf, int &width, int &height);
bool ReadPBM16(const std::string &str, unsigned short *&buf, int &width, int &height);

bool ReadPBM(const std::string &str, unsigned char *buf);

bool InfPBMFile(const std::string &fname,
                int &xsize, int &ysize,
                int &maxval,
                int &nr);

#endif
