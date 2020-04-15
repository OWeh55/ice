ICE V6.x  (c) 1992 - 2018 Friedrich-Schiller-Universitaet Jena
    (c) 2019 - 2020 Wolfgang Ortmann
 
ICE - C - Bibliothek fuer die Bildverarbeitung/Bildanalyse
**********************************************************

(Hinweise fuer Nutzer frueherer Versionen am Ende)

ICE ist eine C/C++ Bibliothek fuer die Bildverarbeitung, die von der
Bildverarbeitungs-Gruppe an der Uni Jena entwickelt wurde und sich im
taeglichen Gebrauch in Forschung und Lehre an der Uni Jena bewaehrt hat.
Die Bibliothek wurde zur Bearbeitung von Forschungsthemen auf dem Gebiet
der robot vision, 3D-Rekonstruktion,  der Mustererkennung und weiteren
Bildverarbeitungsproblemen erfolgreich eingesetzt.

Die Bibliothek wird der Allgemeinheit zur freien Verwendung entsprechend der
LGPL (GNU Lesser General Public License) zur Verfuegung gestellt.
Die LGPL ist als Datei lgpl.txt im Paket zu finden.

Voraussetzungen:
****************

Betriebssystem:
	Linux
oder	Windows

C++ - Compiler

wxWidgets - Bibliothek

JPEG - Bibliothek
fftw3-devel
ffmpeg

Installation Linux:
*******************

 1. Installation des Source-Codes
    Ein Verzeichnis fuer ice anlegen (mkdir ice), in das Verzeichnis 
    wechseln (cd ice) und dort das gepackte tar-Archiv auspacken mit

	zcat ice_6xx.tgz | tar 

    bzw. auch

      tar xzvf ice_6xx.tar
       
 2. Einstellung der Umgebung
    Folgende Umgebungsvariablen sind fuer die Uebersetzung bzw. den Lauf
    von ICE-Programmen erforderlich:

    -	ICEDIR=/home/foo/ice
	Verzeichnis, in dem ICE installiert ist. (Wo das Auspacken des
	tar-Files erfolgte)

    Optionale Variablen

    - ICEVISUALISATION=800,600,100
	Zu verwendende initiale Groesse der Darstellung 800*600, 
        Update der Darstellung alle 100ms

 3. Bilden der Bibliothek

    Wechsel in das Verzeichnis $ICEDIR/src, Bilden der Bibliothek mit

	make

 4. Bilden von eigenen Anwendungen
    Sinnvoller Ausgangspunkt zur Verwaltung eigener Anwendungen ist
    das im Verzeichnis $ICEDIR/examples befindliche Makefile.
    Dieses sollte in das eigene Programm-Verzeichnis kopiert werden.

    Wenn meinprogramm.cpp eine kleine eigene Anwendung aus einem Modul
    (ein *.cpp-File) ist, so kann ohne Editieren des Makefile meinprogramm
    gebildet werden mit:

      make dep       #	muss nur einmal aufgerufen werden, wenn sich
		     #	Abhaengigkeiten nicht aendern
      make meinprogramm

    Wenn das eigene Projekt mehrere Quelltext-Dateien umfasst, muss das 
    Makefile entsprechend angepasst werden.

Soll ICE ohne die verzichtbaren Bibliotheken verwendet werden, so koennen
entsprechende Environment-Variablen gesetzt werden:

Bibliothek            deaktivieren        Einschraenkung
-----------------------------------------------------------------
fftw3-devel           export NOFFTW=1     langsamere Fouriertrafo

Installation Windows
********************

Zur Installation unter Windows wird die Verwendung von des 
MinGW-Paketes empfohlen. Fuer die Visualisierung wird die Bibliothek
wxWidgets benoetigt. Eine Beschreibung der Installation in dieser
Umgebung steht in der Datei readme_win32.txt
Der Einsatz anderer Compiler ist ohne weiteres moeglich, aber ungetestet.

Dokumentation und Kontakte
**************************

Die Beschreibung von ICE liegt als Postscript-File den Paketen bei. 

Wolfgang Ortmann und die Universität Jena uebernehmen mit dieser
Veröffentlichung keine weitergehenden Verpflichtungen zu Beratung und
Support fuer diese ICE-Distribution.

Bemerkungen zum Verzeichnis Examples:

Die Programme in diesem Verzeichnis  sind keine echten Lehrbeispiele, die
meisten entstanden waehrend  der Erprobung neu implementierter Funktionen
als Testprogramm.  Dementsprechend koennen diese Programme auch zum Tes-
ten der korrekten Installation benutzt werden  (z.B. testet vis2 zahlrei-
che Funktionen zur Bildschirmdarstellung). 
Ein Blick in  den Quelltext  dieser Programme kann aber auch helfen,  die
Benutzung einzelner Funktionen besser zu verstehen.

Upgrade von frueheren Versionen:

Gegenueber Version 5 wurde die Verzeichnisstruktur geaendert. Alte Projekte
(Makefiles) erfordern eine Anpassung. Bei nicht zu umfangreichen Projekten
kann das Makefile in ice/examples als Ausgangspunkt dienen.
Alle Funktionen, die mit der Visualisierung verbunden sind, befinden sich im
Unterverzeichnis src/visual. Explizite Includes der Headerfiles in diesem
Verzeichnis muessen deshalb mit Verzeichnisangabe erfolgen, also zum Beispiel

#include <visual/xio.h>

Bei Verwendung von "#include <image.h>" ist keine Aenderung erforderlich. 

Mit der Version 6.3.0 ist die Moeglichkeit entfallen, ein Fenster im Bild 
zu setzen (SetWindowImg...). Eine bessere Moeglichkeit ist die Verwendung 
von Subimages.
