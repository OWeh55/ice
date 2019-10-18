download mingw-w64-install.exe; msys2; jpeg-6b; fftw-3.3.8; tiff-4.0.4; wxWigdets3.1.2; ffmpeg4.1.3; zlib-1.2.11 
Install mingw64:
C:\mingw64\x86_64-8.1.0-posix-seh-rt_v6-rev0\mingw64
Download msys
Extract to C:\msys
configure to fstab in C:\msys\etc
if fstab.conf don't exist create one
add fstab.conf:
C:/mingw64/x86_64-8.1.0-posix-seh-rt_v6-rev0/mingw64     /mingw
add to fstab:
C:/mingw64/x86_64-8.1.0-posix-seh-rt_v6-rev0/mingw64 /mingw

for more information lock at this pages:
http://rperki.blogspot.com/2015/05/installing-mingw-w64-msys-on-win-81-x64.html
https://github.com/mmgen/mmgen/wiki/Install-MinGW-64-and-MSYS-on-Microsoft-Windows

open PowerShell
C:\msys\bin\bash.exe --login

unpack archiv to "C:\msys\lib\" creating folder jpeg-6b
cd /lib/jpeg-6b/ 
./configure --prefix=/mingw --disable-shared
make
make install-lib

unpack archiv to "C:\msys\lib\" creating folder fftw-3.3.8
cd ../fftw-3.3.8/ 
./configure --prefix=/mingw --with-our-malloc16 --with-windows-f77-mangling --enable-shared --disable- static --enable-threads --with-combined-threads --enable-portable-binary --enable-sse2 --with-incoming-s tack-boundary=2   
make
make install

unpack archiv to "C:\msys\lib\" creating folder tiff-4.0.4
cd ../tiff-4.0.4/ 
./configure --prefix=/mingw --disable-shared 
make
make install

unpack archiv to "C:\msys\lib\" creating folder wx
mkdir msw
cd msw
export "CXXFLAGS=-fpermissive" (helps with build wsWigets, maybe)
cd ../wx/msw/
../configure --prefix=/mingw -with-msw --disable-shared 
make
make install

unpack archiv to "C:\msys\lib\" creating folder ffmpeg
cd ../../ffmpeg (version 4.1)
./configure --prefix=/mingw --enable-shared --disable-ffprobe --disable-ffplay --disable-x86asm( if you don't habe the yasm.exe in /mingw/bin)
make
make install

unpack archiv to "C:\msys\lib\" creating folder zlib-1.2.11
cd ../zlib-1.2.11/ 
make -f win32/Makefile.gcc 
cp zlib.h zconf.h /mingw/include/ 
cp libz.a /mingw/lib/


   ICE (from github branch windows)
             - Umgebungsvariablen von msys einstellen:
                    export ICEDIR=/lib/ice
               Wurde auf die Installation der ffmpeg-Bibliothek verzichtet, 
               ist zusätzlich
                    export NOVIDEOIO=1
               einzutragen
             - diese Zeilen in die Datei
                    "C:\msys\etc\profile"
               eintragen & speichern
             - msys-Konsole neustarten
                      
             - Ordner
                    "C:\msys\lib\ice\"
               anlegen
             - Archiv entpacken und Inhalt nach
                    "C:\msys\lib\ice\"
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
