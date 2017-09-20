#include "gcontur.h"

#include "f_contur.h"

void getConturs::get_data()
{
  const Image &src = getInput<Image, GImage, DType::image>();

  int threshold = getInputInt(1, 1);
  Image mark = NewImg(src);
  clearImg(mark);
  GContur *tresult = new GContur;
  result[0] = tresult;

  IPoint ps(0, 0);

  while (SearchStart(src, mark, NULL, threshold, 1, ps, HORZ) == OK)
    {
      Contur c = CalcContur(src, mark, NULL, threshold, ps, 0, isunknown, isunknown);
      MarkContur(c, 2, mark);
      tresult->push_back(c);
    }
}
