/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2002 FSU Jena, Digital Image Processing Group
 * Contact: ice@pandora.inf.uni-jena.de
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _MSC_VER
#include <unistd.h>
#endif


#ifndef _WIN32
// not usable with mingw at the moment
#include <termios.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "defs.h"
#include "dtime.h"
#include "string"

#include "serial.h"

namespace ice
{
#define ON_BIT(flag,bits) pmode.flag|=(bits)
#define OFF_BIT(flag,bits) pmode.flag&=~(bits)

#ifndef cfsetspeed
#define cfsetspeed(x,y) { cfsetispeed(x,y); cfsetospeed(x,y); }
#endif
#ifndef cfmakeraw
#define cfmakeraw(termios_p) \
{ (termios_p)->c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);\
                   (termios_p)->c_oflag &= ~OPOST;\
                   (termios_p)->c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);\
                   (termios_p)->c_cflag &= ~(CSIZE|PARENB);\
                   (termios_p)->c_cflag |= CS8;}
#endif

#define PORTNUMBER 32
  typedef struct Port_
  {
    int fd;

    int mask;
    int open;
    int nextchar;
    int timeout;
  } PortType;


  static PortType Port[PORTNUMBER];

  static int initialized = false;

#define Initialize if (!initialized) initialize()

// initialise structures
  static void initialize()
  {
    int i;

    for (i = 0; i < PORTNUMBER; i++)
      {
        Port[i].open = false;
      }

    initialized = true;
  }

//---------------------------------------------------------------
  int srlOpen(int port,
              int bits, int parity, int stop,
              int baud, int flow,
              int timeout)
  {
    struct termios pmode;
    char devname[30];
    int fd;
    int res;

    Initialize;

    if ((port < 0) || (port >= PORTNUMBER))
      {
        return srlWRONGPARAMETER;
      }

    if (Port[port].open)   // already open ?
      {
        srlClose(port);
      }

    sprintf(devname, "/dev/ttyS%d", port);

//  printf("Device:%s\n",devname);

    fd = open(devname, O_RDWR, 0);
    Port[port].fd = fd;

    res = tcgetattr(fd, &pmode);

    if (res != 0)
      {
        return srlSYSERROR;
      }

    cfmakeraw(&pmode);

    pmode.c_cflag &= ~CSIZE;

    switch (bits)
      {
      case 8:
        pmode.c_cflag |= CS8;
        Port[port].mask = 0xff;
        break;
      case 7:
        pmode.c_cflag |= CS7;
        Port[port].mask = 0x7f;
        break;
      case 6:
        pmode.c_cflag |= CS6;
        Port[port].mask = 0x3f;
        break;
      case 5:
        pmode.c_cflag |= CS5;
        Port[port].mask = 0x1f;
        break;
      default:
        return srlWRONGPARAMETER;
      }

    pmode.c_cflag |= CREAD | CLOCAL;

    switch (baud)
      {
      case 0:
        cfsetspeed(&pmode, B0);
        break;
      case 50:
        cfsetspeed(&pmode, B50);
        break;
      case 75:
        cfsetspeed(&pmode, B75);
        break;
      case 110:
        cfsetspeed(&pmode, B110);
        break;
      case 134:
        cfsetspeed(&pmode, B134);
        break;
      case 150:
        cfsetspeed(&pmode, B150);
        break;
      case 200:
        cfsetspeed(&pmode, B200);
        break;
      case 300:
        cfsetspeed(&pmode, B300);
        break;
      case 600:
        cfsetspeed(&pmode, B600);
        break;
      case 1200:
        cfsetspeed(&pmode, B1200);
        break;
      case 1800:
        cfsetspeed(&pmode, B1800);
        break;
      case 2400:
        cfsetspeed(&pmode, B2400);
        break;
      case 4800:
        cfsetspeed(&pmode, B4800);
        break;
      case 9600:
        cfsetspeed(&pmode, B9600);
        break;
      case 19200:
        cfsetspeed(&pmode, B19200);
        break;
      case 38400:
        cfsetspeed(&pmode, B38400);
        break;
      case 57600:
        cfsetspeed(&pmode, B57600);
        break;
      case 115200:
        cfsetspeed(&pmode, B115200);
        break;
      /*
        case 230400: cfsetspeed(&pmode,B230400); break;
      */
      default:
        return srlWRONGPARAMETER;
      }

    switch (stop)
      {
      case 1:
        OFF_BIT(c_cflag, CSTOPB);
        break;
      case 2:
        ON_BIT(c_cflag, CSTOPB);
        break;
      default:
        return srlWRONGPARAMETER;
      }

    switch (parity)
      {
      case 0: /* no parity */
        OFF_BIT(c_iflag, INPCK);
        OFF_BIT(c_cflag, PARENB);
        OFF_BIT(c_cflag, PARODD);
        break;
      case 1: /* odd parity */
        ON_BIT(c_iflag, INPCK);
        ON_BIT(c_cflag, PARENB);
        ON_BIT(c_cflag, PARODD);
        break;
      case 2: /* even parity */
        ON_BIT(c_iflag, INPCK);
        ON_BIT(c_cflag, PARENB);
        OFF_BIT(c_cflag, PARODD);
        break;
      default:
        return srlWRONGPARAMETER;
      }

    pmode.c_cc[VTIME] = timeout;
    pmode.c_cc[VMIN] = 0;

    res = tcflush(fd, TCIOFLUSH);

    if (res != 0)
      {
        return srlSYSERROR;
      }

    res = tcsetattr(fd, TCSANOW, &pmode);

    if (res != 0)
      {
        return srlSYSERROR;
      }

    Port[port].open = true;
    Port[port].nextchar = -1;
    return srlOK;
  }

  int srlWrite(int port, int byte)
  {
    if ((port < 0) || (port >= PORTNUMBER))
      {
        return srlWRONGPARAMETER;
      }

    Initialize;

    if (!Port[port].open)
      {
        return srlNOTOPEN;
      }

    if (write(Port[port].fd, &byte, 1) != 1)
      {
        return srlSYSERROR;
      }

    return srlOK;
  }

  int srlWriteString(int port, const char* s, int eol)
  {
    unsigned int i;
    int code;
    char h[STRINGLENGTH + 3];

    i = strlen(s);

    if (i > STRINGLENGTH)
      {
        return srlSTRINGTOOLONG;
      }

    strcpy(h, s);

    switch (eol)
      {
      case 0: /*nothing*/
        ;
        break;
      case 1:
        strcat(h, "\x0d");
        break;
      case 2:
        strcat(h, "\x0a");
        break;
      case 3:
        strcat(h, "\x0d\x0a");
        break;
      case 4:
        strcat(h, "\x0a\x0d");
        break;
      }

    Initialize;

    if ((port < 0) || (port >= PORTNUMBER))
      {
        return srlWRONGPARAMETER;
      }

    if (!Port[port].open)
      {
        return srlNOTOPEN;
      }

    for (i = 0; i < strlen(h); i++)
      {
        code = srlWrite(port, h[i]);

        if (code < 0)
          {
            return code;
          }
      }

    return srlOK;
  }

  int srlWriteString(int port, const string& s, int eol)
  {
    return srlWriteString(port, s.c_str(), eol);
  }

  int srlRead(int port)
  {
    int rc;
    char c;

    Initialize;

    if ((port < 0) || (port >= PORTNUMBER))
      {
        return srlWRONGPARAMETER;
      }

    if (!Port[port].open)
      {
        return srlNOTOPEN;
      }

    if (Port[port].nextchar >= 0)
      {
        c = Port[port].nextchar;
        Port[port].nextchar = -1;
        return c;
      }

    rc = read(Port[port].fd, &c, 1);

    if (rc > 0)
      {
        return Port[port].mask & c;
      }

    if (rc < 0)
      {
        return srlSYSERROR;
      }

    return srlNODATA;
  }

  int srlReadString(int port, char* s, int n, int timeout)
  {
    int i = 0;
    int time;
    int character;
    int ready = false;
    s[0] = 0;

    while ((!ready) && (i < n - 1))
      {
        time = 0;

        do
          {
            character = srlRead(port);
            time++;
          }
        while ((character == srlNODATA) && (time < timeout));

        if (character == srlNODATA)
          {
            return srlNODATA;
          }

        if (character < 0)
          {
            return character;
          }

        if ((character != '\x0d') && (character != '\x0a'))
          {
            s[i++] = character;
            s[i] = 0;
          }
        else
          {
            ready = true;
          }
      }

    return srlOK;
  }

  int srlReadString(int port, string& s, int timeout)
  {
    int rc;
    char buffer[3000];
    rc = srlReadString(port, buffer, 3000, timeout);

    if (rc == srlOK)
      {
        s = buffer;
      }

    return rc;
  }

  int srlUnRead(int port, int byte)
  {
    Initialize;

    if ((port < 0) || (port >= PORTNUMBER))
      {
        return srlWRONGPARAMETER;
      }

    if (!Port[port].open)
      {
        return srlNOTOPEN;
      }

    Port[port].nextchar = byte;
    return srlOK;
  }

  int srlClose(int port)
  {
    Initialize;

    if ((port < 0) || (port >= PORTNUMBER))
      {
        return srlWRONGPARAMETER;
      }

    if (!Port[port].open)
      {
        return srlOK;  /* ignore here */
      }

    Port[port].open = false;

    if (close(Port[port].fd) != 0)
      {
        return srlSYSERROR;
      }

    return srlOK;
  }
}
#endif
