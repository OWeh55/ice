Install ICE for Windows 7 with mingw and msys
=============================================
(other compilers untested but probably possible)

0) Legend:
   ":>" ... console input in mingw shell (msys)

1) Neccessary programs and libraries:
   i)   MinGW + MSYS
   Download latest version of mingw-get-setup.exe from 
   http://sourceforge.net/projects/mingw/files/Installer

   ii)  JPEG Bibliothek
   http://www.ijg.org/files/
   testet with jpegsr6b.zip

   iii) libtiff (http://www.libtiff.org/)
   Download from ftp://ftp.remotesensing.org/pub/libtiff/
   testet with ftp://ftp.remotesensing.org/pub/libtiff/tiff-4.0.3.zip

   iv) fftw
   http://www.fftw.org/download.html
   Source packages are needed, not only the precompiled dlls
   testet with http://www.fftw.org/fftw-3.3.3.tar.gz

   v)  wxWidgets (http://www.wxwidgets.org/downloads/)
   Download sources for Windows
   https://sourceforge.net/projects/wxwindows/files/2.9.5/wxWidgets-2.9.5.zip/download

   vi)   ffmpeg (optional, used for reading and writing of video files)
   Look for releases at http://www.ffmpeg.org/download.html#releases
   (http://www.ffmpeg.org/releases/ffmpeg-2.0.1.tar.gz)

   vii)  zLib (only necessary with ffmpeg)
   http://zlib.net/zlib128.zip

   viii)   ICE
   http://www.inf-cv.uni-jena.de/ice/download.html (Version 6.30)

Installation of all packages requires 1,7 GB free disk space.
                        
2) Installation:
        
   2.1) MinGW and MSYS
      - start mingw-get-setup.exe
      - choose install
      - proof settings:
        - Location 
          Use only directories that contain no spaces in their name
          if you change this you must substitute C:\MinGW in this
          file with your location
        - enable graphical user interface
      - continue...
        Graphical User Interface starts
      - select packages:
          Basic Setup:
            mingw-developer-toolkit
            mingw-gcc-g++
            msys-base
          MSYS Base System
            msys-tiny
            ...?
      - use menu "Installation/apply changes" to install

      - create a file c:\MinGW\msys\1.0\etc\fstab containing a line
           "c:/mingw	/mingw"
        look at c:\MinGW\msys\1.0\etc\fstab.sample for an example

      - if you want to use mingw tools outside msys, modify the 
        win environment variable PATH to include
            "C:\MinGW\bin"
            "C:\MinGW\msys\1.0\bin"

      - as a shortcut to start msys (bash) put a link to 
           C:\MinGW\msys\1.0\msys.bat
        on the desktop ("send to desktop")

    In the following explanation :> means, that you have to write that 
    command in the bash window, that opens when you start msys
    (C:\MinGW\msys\1.0\msys.bat)

   2.2) JPEG library
      - Unpack archiv to "C:\MinGW\msys\1.0\lib\" creating
        an folder jpeg-6b here

      - open msys 
          :> cd /lib/jpeg-6b
          :> ./configure --prefix=/mingw --disable-shared
          :> make
          :> make install-lib
                
   2.3) fftw
      - Unpack fftw to "C:\MinGW\msys\1.0\lib\"
        creating "C:\MinGW\msys\1.0\lib\fftw-3.3.3\"
          
      - open msys:
          :> cd /lib/fftw-3.3.3
          :> ./configure --prefix=/mingw --disable-alloca --with-our-malloc
          :> make
           if you get errors like "codlist.c: Permission denied",
           then erase the file[s] codlist.c with 
                :> find . -name codlist.c -delete
          :> make install

   2.4) libtiff (http://www.libtiff.org/)
      - Unpack to "C:\MinGW\msys\1.0\lib\" creating  "C:\MinGW\msys\1.0\lib\tiff-4.0.3"

      - open msys:
          :> cd /lib/tiff-4.0.3
          :> ./configure --prefix=/mingw --disable-shared
          :> make
          :> make install
                                            
   2.5) wxWidgets (Version: 2.9.5, wxWidgets-2.9.5.zip)
      - create a directory "C:\MinGW\msys\1.0\lib\wx\"
      - unpack zip-File into this directory

      - open msys
          :> cd /lib/wx
          :> mkdir msw
          :> cd msw

          :> ../configure --prefix=/mingw --with-msw --disable-shared --disable-treectrl --disable-dirpicker --disable-filepicker --disable-dirdlg
        disable-treectrl and following was necessary because of a bug in this version
          :> make
          :> make install

  2.6) ffmpeg (Version: 0.6.1, ffmpeg-0.6.1.tar.bz2)
     - Anlegen eines Ordners "C:\MinGW\msys\1.0\lib\ffmpeg\"
            - Archiv in diesen Ordner entpacken
            - in der MinGW-Shell in den Ordner wechseln
                 :> cd /lib/ffmpeg
            - dort folgendes ausfuehren
                 :> ./configure --prefix=/mingw --enable-shared --enable-memalign-hack --disable-ffprobe --disable-ffserver --disable-ffplay
                 :> make
                 :> make install

  2.7) zLib (http://zlib.net/zlib125.zip)
            - Archiv zlib125.zip auspacken nach /mingw/msys/1.0/lib/
	    - in der MinGW-Shell in das Verzeichnis /lib/zlib-1.2.5 wechseln
                 :> cd /lib/zlib-1.2.5
            - dort ausfuehren
                 :> make -f win32/Makefile.gcc
                 :> cp zlib.h zconf.h /mingw/include
                 :> cp libz.a /mingw/lib

  2.8) ICE (Version: 6.3, ice_630.zip)
             - Umgebungsvariablen von msys einstellen:
                    export ICEDIR=/lib/ice
               Wurde auf die Installation der ffmpeg-Bibliothek verzichtet, 
               ist zusätzlich
                    export NOVIDEOIO=1
               einzutragen
             - diese Zeilen in die Datei
                    "C:\MinGW\msys\1.0\etc\profile"
               eintragen & speichern
             - msys-Konsole neustarten
                      
             - Ordner
                    "C:\MinGW\msys\1.0\lib\ice\"
               anlegen
             - Archiv entpacken und Inhalt nach
                    "C:\MinGW\msys\1.0\lib\ice\"
               kopieren
             - in den Ordner wechseln
                    :> cd /lib/ice/src/
             - dort folgendes Ausfuehren
                    :> make

	Zum Testen der Installation sollte ein Beispiel-Programm gebildet 
	werden:
		- in Verzeichnis examples wechseln
                        :> cd /lib/ice/example
                - Bilden und Ausfuehren von vis2
                        :> make
                        :> vis2

Hinweise zum Bilden eigener Programme

Ausgangspunkt fuer ein eigenes Makefile sollte das Makefile im Verzeichnis examples 
sein. Dieses sollten Sie in ein eigenes Arbeitsverzeichnis kopieren. 

Sie koennen nun:

- eigene kleine Programme bilden, die aus einer Quelldatei (myprog.cpp) 
  bestehen, indem Sie
    - die Abhaengigkeiten bilden
        :> make dep
    - das eigentliche Programm bilden
        :> make myprog

- eigene Programme bilden, die aus einem Hauptprogramm (myprog.cpp) 
  und mehreren weiteren Quelldateien (obj1.cpp, obj2.cpp, obj3.cpp) 
  bestehen, indem Sie 
    - in dem Makefile die folgenden Zeilen editieren

        MAIN=myprog
        OBJECTS=$(MAIN).o obj1.o onj2.o obj3.o

    - die Abhaengigkeiten bilden
        :> make dep
    - das eigentliche Programm bilden
        :> make
