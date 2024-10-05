/**********************************************************/
/*  Programm zur Umbenennung von Funktionen, Strukturen.. */
/**********************************************************/

#include <stdlib.h>
#include <unistd.h>
#include <image.h>
#include <stdio.h>

using namespace ice;

int getword(char**, char []);
int sonder(char c);

FILE* textfile;
TextEntry Dirliste = NULL;
TextEntry konvert = NULL;

#define SEEK_SET 0
#define SEEK_END 2

int filesize(FILE*);

int main(int argc, char* argv[])
{
  char wort[80];
  char q[40], z[40], * buffer, * buffer1;
  char ch[5] = "-/|\\";
  TextEntry akt;
  int fsize;
  int i;

  setbuf(stdout, NULL);
  setbuf(stdin, NULL);
  textfile = fopen("/ice/konvert.dat", "rt");

  if (textfile == NULL)
    {
      printf("Kein Konverterfile \n");
      exit(255);
    }

  while (fscanf(textfile, "%s %s", q, z) == 2)
    {
      printf(" %s -> %s \n", q, z);
      akt = AddTextEntry(&konvert, q);
      strcpy(&(akt->Text[40]), z);
    }

  fclose(textfile);

  Directory(&Dirliste, "*.c");
  Directory(&Dirliste, "*.h");

  while (Dirliste != NULL)
    {
      printf("File: %s ", Dirliste->Text);
      textfile = fopen(Dirliste->Text, "rt");
      fsize = filesize(textfile);
      printf(" %d Bytes ", fsize);
      buffer = (char*) malloc(fsize);

      if (buffer == NULL) exit(222);

      if ((fsize = fread(buffer, 1, fsize, textfile)) == 0) exit(111);

      *(buffer + fsize) = 0;
      fclose(textfile);
      textfile = fopen("temp.$$$", "wt");
      i = 0;
      buffer1 = buffer;

      while (*buffer1 != 0)
        {
          if (getword(&buffer1, wort))
            {
              for (akt = konvert;
                   (akt != NULL) && (strcmp(akt->Text, wort) != 0);
                   akt = akt->next);

              if (akt != NULL) strcpy(wort, akt->Text + 40);
            }

          fprintf(textfile, "%s", wort);

          if ((i & 0x3f) == 0) printf("%c\b", ch[(i >> 6) & 3]);

          i++;
        }

      printf("\n");
      fclose(textfile);
      free(buffer);
      unlink(Dirliste->Text);
      rename("temp.$$$", Dirliste->Text);

      SubTextEntry(&Dirliste, Dirliste->Text);
    }

  return 0;
}

int getword(char** Line, char w[111])
{
  int iz;
  char* buffer;
  buffer = *Line;

  w[0] = *buffer;
  w[1] = 0;
  buffer++;

  if (sonder(w[0]))
    {
      *Line = buffer;
      return FALSE;
    }

  iz = 1;

  while ((*buffer != 0) && (!sonder(*buffer)))
    {
      w[iz] = *buffer;
      buffer++;
      iz++;
    }

  w[iz] = 0;
  *Line = buffer;
  return TRUE;
}

int sonder(char c)
{
  if (c == '_') return FALSE;

  if (c < '0') return TRUE;

  if (c <= '9') return FALSE;

  if (c < '@') return TRUE;

  if (c <= 'Z') return FALSE;

  if (c < 'a') return TRUE;

  if (c <= 'z') return FALSE;

  return TRUE;
}

int filesize(FILE* stream)
{
  int altpos, laenge;

  /* speichert die aktuelle Position
     des Dateizeigers */
  altpos = ftell(stream);

  /* positioniert ans Ende der Datei */
  fseek(stream, 0, SEEK_END);

  /* speichert die Gr”áe der Datei */
  laenge = ftell(stream);

  /* setzt den Dateizeiger auf die
     ursprngliche Position */
  fseek(stream, altpos, SEEK_SET);

  return laenge;
}
