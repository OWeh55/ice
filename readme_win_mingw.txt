Install MinGW (see also: http://www.mingw.org/wiki/Getting_Started)
  download "MinGW-get-setup" from http://www.mingw.org
  start "MinGW-get-setup"
  - License++ -> Install
  - Install-Directory C:MinGW
    x Support graphical UI
    Continue
  (Package download follows)
  - Continue
  - Package selection: 
      mark for installation
      Basic Setup: 
      * mingw developer tools
      * mingw32-base
      * mingw32-gcc-g++
      * msys-base
      -> Installation/Apply Changes

Start c:\mingw\msys\1.0\msys.bat. This opens the msys-shell, a commandline interface.
Commands, that are described in the following sections, have to be executed in the msys-shell.
Filenames and directories given with '/' as delimiter are valid from within the msys-shell. 
If backslash '\' is used as delimiter, this is a file name or directory from the viewpoint 
of Windows.

In the msys-shell execute /postinstall/pi.sh

alternatively:
     create or edit c:\mingw\msys\1.0\etc\fstab:
     enter line "C:\Mingw /mingw"

Install yasm
	get http://www.tortall.net/projects/yasm/releases/yasm-1.2.0-win32.exe
	copy as yasm.exe to c:\mingw\bin

Install pkg-config
	http://win32builder.gnome.org/packages/3.6/pkg-config_0.28-1_win32.zip 
	copy pkg-config.exe to c:\mingw\bin
	download libglib-2.0-0.dll from ? and copy it to c:\mingw\bin

For the compilation and installation of additioal libraries create the
directory /usr/local/lib:  mkdir -p /usr/local/lib. 
(This is C:\MinGW\msys\1.0\local\lib)

Install libx264  (needed (?) for libffmpeg)
	download from http://www.videolan.org/developers/x264.html
      unpack x264-snapshot-xxxxxxx.tar.bz2 in /usr/local/lib:
	cd /usr/local/lib
      tar xjvf x264-snapshot-xxxxxxx.tar.bz2
	cd x264-snapshot-xxxx
      ./configure --prefix=/mingw --enable-shared
	make
      make install

libffmpeg
      download from https://ffmpeg.org/download.html
	      (https://ffmpeg.org/releases/ffmpeg-2.2.tar.gz)
      unpack to /usr/local/lib
      cd /usr/local/lib/ffmpeg-2.2
      ./configure --prefix=/mingw --disable-iconv
      make
	make install

libjpeg
	http://ijg.org/files/jpegsr9a.zip
      unpack to /usr/local/lib
      cd /usr/local/lib/jpeg-9a
      autoheader (always necessary ?)
     ./configure --prefix=/mingw
      make
	make install

libtiff
	ftp://ftp.remotesensing.org/pub/libtiff/tiff-4.0.3.zip
      unpack to /usr/local/lib
      cd /usr/local/lib/tiff-4.0.3
     ./configure --prefix=/mingw
	find . -name codlist.c -delete
      make
	make install

fftw
	from http://www.fftw.org/download.html download http://www.fftw.org/fftw-3.3.4.tar.gz
     ./configure --prefix=/mingw
      make
	make install

zlib
	http://www.zlib.net/
	http://zlib.net/zlib-1.2.8.tar.gz
      unpack to /usr/local/lib
      cd /usr/local/lib/zlib-1.2.8
      make -f win32/Makefile.win32
	make -f win32/Makefile.win32 BINARY_PATH=/mingw/bin INCLUDE_PATH=/mingw/include LIBRARY_PATH=/mingw/lib install


libpng 
	http://www.libpng.org/pub/png/libpng.html
      unpack to /usr/local/lib
      cd /usr/local/lib/libpng-1.6.10
      ./configure --prefix=/mingw
      make
	make install

!!! 	in c:\mingw\include\commctrl.h, in c:\mingw\mingw32\include\commctrl.h,
	change from #define TV_DISPINFO __AW(NMTVDISPINFO) to #define TV_DISPINFO NMTVDISPINFO

wxWidgets
	download from http://www.wxwidgets.org/downloads/
      (https://sourceforge.net/projects/wxwindows/files/3.0.0/wxWidgets-3.0.0.zip)
      unpack to /usr/local/lib
      cd /usr/local/lib/wxWidgets
	mkdir win
      cd win
      ../configure --prefix=/mingw --with-msw
      make
