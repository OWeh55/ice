#ifndef FC_REFERENCES_H
#define FC_REFERENCES_H

#include <string>
#include <vector>
#include <Point.h>

bool GetReferencePoints(
  const ice::Image &img, const ice::Image &mrk, const ice::Image &segmented_img,
  std::vector<ice::Point> &pointList, std::vector<ice::Point> &referenceList,
  ice::Trafo &homography, ice::Distortion &dist);

#endif
