#include <stdlib.h>
#include <stdio.h>

#include <string>
#include <iostream>
#include "pbmio.h"


using namespace std;

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

int is_white(int c)
{
  static int is_comment = false;
  if (is_comment)
    {
      if (c == 0x0a) is_comment = false;
      return true;
    }
  if (c == '#')
    {
      is_comment = true;
      return true;
    }
  return (c == '\t') || (c == 10) || (c == 13) || (c == ' ');
}

int is_number(int c)
{
  return (c >= '0') && (c <= '9');
}

int read_number(FILE *fd)
{
#define MAXNUMBERLEN 10
  int c;
  int res = 0;
  do
    {
      c = fgetc(fd);
    }
  while (is_white(c));
  while (is_number(c))
    {
      res = res * 10 + c - '0';
      c = getc(fd);
    }
  return res;
}

EXFILE pbm_open_read(const string &fname, int &xsize, int &ysize, int &maxval, int &ch)
{
  EXFILE fd;
  int c;

  fd = exopen(fname, FRMODUS);
  if (fd.fd == NULL) return fd;

  c = fgetc(fd.fd);
  if (c != 'P')
    {
      exclose(fd);
      fd.fd = NULL;
      return fd;
    }

  c = fgetc(fd.fd);
  switch (c)
    {
    case '5':
      ch = 1;
      break;
    case '6':
      ch = 3;
      break;
    default:
    {
      exclose(fd);
      fd.fd = NULL;
      return fd;
    }
    }

  xsize = read_number(fd.fd);
  ysize = read_number(fd.fd);
  maxval = read_number(fd.fd);

  return fd;
}

bool pbmreadbuffer8(const string &fname, unsigned char *data)
{
  EXFILE fd;
  int xsize, ysize, maxval, ch;
  fd = pbm_open_read(fname, xsize, ysize, maxval, ch);
  if ((fd.fd == NULL) || (maxval > 255))
    {
      return false;
    }

  fread(data, ch, xsize * ysize, fd.fd);
  exclose(fd);
  return true;
}

void swapb(unsigned short *p)
{
  unsigned short h = *(unsigned char *)p * 256U + *(unsigned char *)(p + 1);
  *p = h;
}

bool pbmreadbuffer16(const string &fname, unsigned short *data)
{
  EXFILE fd;
  int xsize, ysize, maxval, ch;
  fd = pbm_open_read(fname, xsize, ysize, maxval, ch);
  if ((fd.fd == NULL) || (maxval < 256))
    {
      return false;
    }

  fread(data, ch * 2, xsize * ysize, fd.fd);
  exclose(fd);
  for (unsigned short *p = data; p < data + xsize * ysize; p++)
    swapb(p);
  return true;
}

bool ReadPBM(const string &fname, unsigned char *data)
{
  return pbmreadbuffer8(fname, data);
}

bool ReadPBM8(const string &str, unsigned char *&buf, int &width, int &height)
{
  int nr;
  int maxval;
  if (!InfPBMFile(str, width, height, maxval, nr))
    return false;
  //  cout << maxval << " " << nr << endl;
  if (maxval > 255 || nr > 1)
    return false;
  buf = new unsigned char[width * height];
  return pbmreadbuffer8(str, buf);
}

bool ReadPBM16(const string &str, unsigned short *&buf, int &width, int &height)
{
  int nr;
  int maxval;
  if (!InfPBMFile(str, width, height, maxval, nr))
    return false;
  //  cout << maxval << " " << nr << endl;
  if (maxval < 256 || nr > 1)
    return false;
  buf = new unsigned short[width * height];
  for (int i = 0; i < width * height; i++)
    buf[i] = 0;
  return pbmreadbuffer16(str, buf);
}

bool InfPBMFile(const string &fname,
                int &xsize, int &ysize, int &maxval, int &nr)
{
  EXFILE fd;
  fd = pbm_open_read(fname, xsize, ysize, maxval, nr);
  if (fd.fd == NULL) return false;
  exclose(fd);
  return true;
}

EXFILE exopen(const string &fn, const char *mode)
{
  EXFILE rc;
  rc.process = fn[0] == '|';
  if (!rc.process) rc.fd = fopen(fn.c_str(), mode);
  else rc.fd = popen(fn.c_str() + 1, mode);
  return rc;
}

bool exclose(const EXFILE &fd)
{
  if (fd.process) return pclose(fd.fd);
  else return fclose(fd.fd);
}
