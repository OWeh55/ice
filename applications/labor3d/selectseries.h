#ifndef SELECTSERIES_H
#define SELECTSERIES_H

#include <string>

class SeriesName
{
//private:
public:
  static const string dicommask;
  bool dicom;
  std::string mask;
  std::string basedir;
  std::string patient;
  std::string study;
  std::string series;
public:
  SeriesName(): dicom(true), mask(dicommask) {}
  SeriesName(const string& basedir, bool dicom = false): dicom(dicom), basedir(basedir)
  {
    if (dicom)
      mask = dicommask;
    else
      mask = "*.jpg";
  }

  std::string Filename() const
  {
    if (dicom)
      return basedir + "/" + patient + "/" + study + "/" + series;
    else
      return basedir;
  }

  std::string FileMask() const
  {
    if (dicom)
      return basedir + "/" + patient + "/" + study + "/" + series + "/" + mask;
    else
      return basedir + "/" + mask;
  }
  bool isValid() const
  {
    return dicom ? (!series.empty()) : (!basedir.empty());
  }
};

SeriesName SelectSeries(const SeriesName& ser);
SeriesName SelectSeries(const std::string& dir);
SeriesName SelectDir(const std::string& dir = ".");

#endif
