ICE V6.x  (c) 1992 - 2011 Friedrich-Schiller-Universitaet Jena

ICE - C - Bibliothek fuer die Bildverarbeitung/Bildanalyse
**********************************************************

(Hinweise fuer Nutzer frueherer Versionen am Ende)

ICE ist eine C/C++ Bibliothek fuer die Bildverarbeitung, die von der Bildverar-
beitungs-Gruppe an der Uni Jena entwickelt wurde und sich im taeglichen Ge-
brauch in Forschung und Lehre an der Uni Jena bewaehrt hat.  Die Bibliothek
wurde zur Bearbeitung von Forschungsthemen auf dem Gebiet der robot vision/
3D-Rekonstruktion,  der Mustererkennung und  weiteren Bildverarbeitungspro-
blemen erfolgreich eingesetzt.

Die Bibliothek wird der Allgemeinheit zur freien Verwendung entsprechend der
LGPL (GNU Lesser General Public License) zur Verfuegung gestellt.
Die LGPL ist als Datei lgpl.txt im Paket zu finden.

Voraussetzungen:
****************

Betriebssystem:
	Linux mit X11
oder	WindowsXX

C++ - Compiler

wxWidgets - Bibliothek
JPEG - Bibliothek

Sehr empfohlen, aber eventuell verzichtbar:
 fftw3-devel
 libraw1394-devel (nur linux)
 libdc1394-devel (nur linux)

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
	Tar-Files erfolgte)

    Optionale Variablen

    - ICEVISUALISATION=800,600,100
	Zu verwendende Groesse der Dartstellung 800*600, 
        Update der Darstellung alle 100ms

    - ICESCANDEVICE0=program,scan /tmp/bild.bmp,/tmp/bild.bmp
      ICESCANDEVICE1=file,/pandora/data/temp/bild.bmp
      ICESCANDEVICE2=fw,0,MODE_640x480_YUV422,30
	
      Die Variablen ICESCANDEVICEx legen die fuer die Bildeingabe
	(Scan) zu verwendenden Geraete bzw. Treiber fest. 
        Die angefuehrten Beispiele bedeuten:

	ICESCANDEVICE0=program,scan /tmp/bild.bmp,/tmp/bild.bmp
	Es ist ein externes Programm aufzurufen (hier: scan),
	welches ein Bild in einer Datei ablegt. Das Ergebnis
	ist die Datei /tmp/bild.bmp. Die Ergebnis-Datei mu� in
	einem von ICE unterstuetzten Format vorliegen.
	
	ICESCANDEVICE1=file,/data/bild.bmp
	Das gescannte Bild liegt direkt als Datei vor.

	ICESCANDEVICE2=fw,0,MODE_640x480_YUV422,30
	Es wird die Firewire-Schnittstelle zum Scannen
        verwendet.

 3. Bilden der Bibliothek

    Wechsel in das Verzeichnis $ICEDIR/src, Bilden der Bibliothek mit

	make

 4. Bilden von eigenen Anwendungen
    Sinnvoller Ausgangspunkt zur Verwaltung eigener Anwendungen ist
    das im Verzeichnis $ICEDIR/examples befindliche Makefile. Dies sollte
    in das eigene Programm-Verzeichnis kopiert werden.

    Wenn meinprogramm.cpp eine kleine eigene Anwendung aus einem Modul
    (ein *.cpp-File) ist, so kann ohne Editieren des Makefile meinprogramm
    gebildet werden mit:

      make dep  ; //	muss nur einmal aufgerufen werden, wenn sich
			Abhaengigkeiten nicht aendern
      make meinprogramm

    Wenn das eigene Projekt mehrere Quelltext-Dateien umfa�t, mu� das 
    Makefile entsprechend angepa�t werden.

Soll ICE ohne die verzichtbaren Bibliotheken verwendet werden, so koennen
entsprechende Environment-Variablen gesetzt werden:

Bibliothek            deaktivieren        Einschraenkung
-----------------------------------------------------------------
fftw3-devel           export NOFFTW=1     langsamere Fouriertrafo
-----------------------------------------------------------------
libraw1394-devel+     export NOSCANFW=1    keine Bildaufnahme mit 
libdc1394-devel                            Firewire-Kameras
-----------------------------------------------------------------


Installation Windowsxx
**********************

Zur Installation unter Windows wird die Verwendung von des 
MinGW-Paketes empfohlen. Fuer die Visualisierung wird die Bibliothek
wxWidgets benoetigt. Eine Beschreibung der Installation in dieser
Umgebung steht in der Datei readme_win32.txt
Der Einsatz anderer Compiler ist ohne weiteres moeglich, aber ungetestet.

Dokumentation und Kontakte
**************************

Die Beschreibung von ICE liegt als Postscript-File den Paketen bei. Die 
Dokumentation ist auch online unter

http://www.inf-cv.uni-jena.de/ice/icedoc_html/icedoc_html.html

lesbar.  Beachten Sie bitte,  dass die online-Dokumentation stets den ak-
tuellen Weiterentwicklungen angepasst wird und deshalb nicht vollstaendig
mit dieser Distribution konform gehen muss.

Die Universitaet Jena uebernimmt mit dieser Veroeffentlichung keine 
weitergehenden Verpflichtungen zu Beratung und Support fuer diese 
ICE-Distribution. Trotzdem wollen wir nach Moeglichkeit helfen, Probleme 
beim Einsatz von ICE zu loesen.  Schicken Sie uns dazu eine kurze Mail 
mit einer moeglichst genauen Problem-Beschreibung an

ice@inf.uni-jena.de

Diese Adresse sollten Sie ebenfalls benutzen, wenn Sie eigene ICE-Programme 
oder Funktionen geschrieben haben,  die Sie gerne auch anderen zur
Verfuegung stellen wollen. Wir werden dann diese Information  in unseren 
Server aufnehmen.

Bemerkungen zum Verzeichnis Examples:

Die Programme in diesem Verzeichnis  sind keine echten Lehrbeispiele, die
meisten entstanden waehrend  der Erprobung neu implementierter Funktionen
als Testprogramm.  Dem entsprechend koennen diese Programme auch zum Tes-
ten der korekten Installation benutzt werden  (z.B. testet vis2 zahlrei-
che Funktionen zur Bildschirmdarstellung). 
Ein Blick in  den Quelltext  dieser Programme kann aber auch helfen,  die
Benutzung einzelner Funktionen besser zu verstehen.

Upgrade von frueheren Versionen:

Gegenueber Version 5 wurde die Verzeichnisstruktur geaendert. Alte Projekte
(Makefiles) erfordern eine Anpassung. Bei nicht zu umfangreichen Projekten
kann das Makefile in ice/examples als Ausgangspunkt dienen.
Alle Funktionen, die mit der Visualisierung verbunden sind, befinden sich im
Unterverzeichnis src/visual. Explizite Includes der Headerfiles in diesem
Verzeichnis muessen deshalb mit Verzeichnis erfolgen, also zum Beispiel

#include <visual/xio.h>

Bei Verwendung von "#include <image.h>" ist keine Aenderung erforderlich. 

Der NULL-Pointer kann nicht mehr als Bild (Typ Image) uebergeben werden. Wo
dies unbedingt erforderlich ist, muss stattdessen "Image()" verwendet werden. 

Mit der Version 6.3.0 ist die Moeglichkeit entfallen, ein Fenster im Bild 
zu setzen (SetWindowImg...). Eine bessere Moeglichkeit ist die Verwendung 
von Subimages.