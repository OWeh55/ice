#include <image.h>

int main(int argc, char** argv)
{
  Image img1 = ReadImg("test_gray.jpg");
  Image img2;
  img2.create(img1);
  Image img3;
  img3.create(img1);
  Show(ON, img1, "Original");
  Show(ON, img2, "Rank");
  Show(ON, img3, "RankSub");
  for (int i = 0; i < 9; i++)
    {
      Printf("Rang: %d\n", i);
      RankImg(img1, 3, i, img2);
      SubRankImg(img1, 3, i, img3);
      GetChar();
    }
  return 0;
}
