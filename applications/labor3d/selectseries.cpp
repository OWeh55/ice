#include "image.h"
#include "selectseries.h"

using namespace std;

const string SeriesName::dicommask = "img*.bmp";

SeriesName SelectSeries(const SeriesName &ser)
{
  SeriesName res = ser;
  res.series = "";

  string selection, selectionpath;
  if (res.patient.empty())
    {
      if (SelFile(res.basedir + "/*", selection, selectionpath, DIR_DIR,
                  "Patient wählen", 2, 4, 60, 20))
        res.patient = selection;
      else
        return res; // Abbruch !
    }
  if (res.study.empty())
    {
      if (SelFile(res.basedir + "/" + res.patient + "/*", selection, selectionpath, DIR_DIR,
                  "Studie wählen", 2, 4, 60, 20))
        res.study = selection;
      else
        {
          res.patient = ""; // abgebrochen ...
          return SelectSeries(res);  // eine Ebene höher
        }
    }

  if (SelFile(res.basedir + "/" + res.patient + "/" + res.study + "/*", selection, selectionpath, DIR_DIR,
              "Serie wählen", 2, 4, 60, 20))
    res.series = selection;
  else
    {
      res.study = ""; // abgebrochen ...
      return SelectSeries(res);  // eine Ebene höher
    }
  return res;
}

SeriesName SelectSeries(const string &dir)
{
  SeriesName res(dir, true);
  return SelectSeries(res);
}

SeriesName SelectDir(const std::string &dir)
{
  string selection, selectionpath;
  if (SelFile(dir + "/*", selection, selectionpath, DIR_FILE | DIR_SELPATH, "Musterdatei waehlen", 2, 4, 60, 20))
    {
      SeriesName res(selectionpath, false);

      string mask;
      unsigned int i = 0;

      while (i < selection.length())
        {
          while (i < selection.length() && !isdigit(selection[i]))
            {
              mask += selection[i];
              i++;
            }

          if (i < selection.length())
            mask += "*";

          while (i < selection.length() && isdigit(selection[i]))
            {
              i++;
            }
        }

      res.mask = mask;
      return res;
    }
  else
    return SeriesName();
}
