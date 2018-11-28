#include "ggraph.h"
#include "gcontur.h"
#include "gregion.h"

#include "f_draw.h"

void Draw::get_data()
{
  const GImage* src1 = getInputImage(0);
  const GData* src2 = getInputData(1);

  int valuer = getInputInt(2, src1->maxVal());
  int valueg = getInputInt(3, valuer);
  int valueb = getInputInt(4, valuer);

  calcSize(src1);

  GImage* tresult = cloneGImage(src1);
  result[0] = tresult;
  addImageParameters(result, 0);

  if (src2->getType().type == DType::point)
    {
      const GPoint* tsrc2 = dynamic_cast<const GPoint*>(src2);
      for (int i = 0; i < tsrc2->getSize(); ++i)
        {
          const Point& p = (*tsrc2)[i];
          (*tresult)[0].setPixelClipped(p.x, p.y, valuer);
          if (src1->getSize() > 1)
            (*tresult)[1].setPixelClipped(p.x, p.y, valueg);
          if (src1->getSize() > 2)
            (*tresult)[2].setPixelClipped(p.x, p.y, valueb);
        }
    }

  else if (src2->getType().type == DType::graph)
    {
      const GGraph* tsrc2 = dynamic_cast<const GGraph*>(src2);
      Point p1;
      Point p2;
      for (int i = 0; i < tsrc2->nEdges(); ++i)
        {
          tsrc2->getEdge(i, p1, p2);
          Line(p1, p2, valuer, (*tresult)[0]);
          if (src1->getSize() > 1)
            Line(p1, p2, valueg, (*tresult)[1]);
          if (src1->getSize() > 2)
            Line(p1, p2, valueb, (*tresult)[2]);
        }
    }

  else if (src2->getType().type == DType::contur)
    {
      const GContur* tsrc2 = dynamic_cast<const GContur*>(src2);
      for (int i = 0; i < tsrc2->getSize(); ++i)
        {
          MarkContur((*tsrc2)[i], valuer, (*tresult)[0]);

          if (src1->getSize() > 1)
            MarkContur((*tsrc2)[i], valueg, (*tresult)[1]);

          if (src1->getSize() > 2)
            MarkContur((*tsrc2)[i], valueb, (*tresult)[2]);
        }
    }

  else if (src2->getType().type == DType::region)
    {
      const GRegion* tsrc2 = dynamic_cast<const GRegion*>(src2);
      for (int i = 0; i < tsrc2->getSize(); ++i)
        {
          const Region& r = (*tsrc2)[i];
          r.draw((*tresult)[0], valuer);

          if (src1->getSize() > 1)
            r.draw((*tresult)[1], valueg);

          if (src1->getSize() > 2)
            r.draw((*tresult)[2], valueb);
        }
    }
  else if (src2->getType().type == DType::image)
    {
      const GImage* tsrc2 = dynamic_cast<const GImage*>(src2);
      if (tsrc2->getSize() != 1)
        throw SdpException(getName(), "expected grayvalue image");

      const Image marker = (*tsrc2)[0];

      WindowWalker ww((*tresult)[0]);

      for (ww.init(); !ww.ready(); ww.next())
        if (marker.getPixel(ww))
          (*tresult)[0].setPixel(ww, valuer);

      if (src1->getSize() > 1)
        {
          for (ww.init(); !ww.ready(); ww.next())
            if (marker.getPixel(ww))
              (*tresult)[1].setPixel(ww, valueg);
        }

      if (src1->getSize() > 2)
        {
          for (ww.init(); !ww.ready(); ww.next())
            if (marker.getPixel(ww))
              (*tresult)[2].setPixel(ww, valueb);
        }
    }
  else if (src2->getType().type == DType::integer)
    {
      // we assume, that the given data vector<int> is a histogram
      const GInteger* tsrc2 = dynamic_cast<const GInteger*>(src2);
      int nClasses = src2->getSize();
      Histogram h(nClasses);
      for (int i = 0; i < nClasses; ++i)
        h.addValue(i, (*tsrc2)[i]);

      h.draw((*tresult)[0], valuer);

      if (src1->getSize() > 1)
        h.draw((*tresult)[1], valueg);

      if (src1->getSize() > 2)
        h.draw((*tresult)[2], valueb);
    }
}
