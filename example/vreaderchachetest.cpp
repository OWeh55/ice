#include <image.h>

int main(int argc, char** argv)
{
  string fn = "/home/noo/extra/film/Home.mp4";

  if (argc > 1)
    fn = argv[1];

  VideoFileCached vi(fn, 100);
  int xs, ys, mv, fps;
  vi.getPara(xs, ys, mv, fps);
  Image ri = NewImg(xs, ys, mv);
  Image gi = NewImg(xs, ys, mv);
  Image bi = NewImg(xs, ys, mv);
  Show(_RGB, ri, gi, bi);
  vi.Read(ri, gi, bi, 10);
  Printf("Frame: %d , Error: %d\n", vi.FrameNumber(), vi.getError());
  int c;

  while ((c = GetChar()) != 'x')
    {
      switch (c)
        {
        case 'n':
          vi.Read(ri, gi, bi, vi.next);
          break;
        case 'p':
          vi.Read(ri, gi, bi, vi.prev);
          break;
        }

      Printf("Frame: %d , Error: %d\n", vi.FrameNumber(), vi.getError());
    }
}
