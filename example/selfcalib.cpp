#include <iostream>

#include <image.h>
#include <selfcalib.h> // ICE

/**
 * Output.
 * @param camera the camera object
 * @param xsize image xsize
 * @param ysize image ysize
 */

void printInterpretedIntrinsic(Camera& camera)
{
  Vector v = camera.MakeVector();

//   cout << "Intrinsic parameters:" << endl;
//   cout << endl;
//   cout << "Camera.toString():" << endl;
//   cout << camera.toString() << endl;

  cout << "Intrinsic calibration result, non-normalized:" << endl;
  cout << "fx: " << v[0] << endl;
  cout << "fy: " << -v[1] * v[0] << endl;
  cout << "s: " << v[2] * v[0] << endl;
  cout << "ox: " << v[3] << endl;
  cout << "oy: " << v[4] << endl;
  cout << endl;

//   const double factor = std::max(double(xsize), double(double(ysize))) / 2.0;
//   cout << "Intrinsic calibration result, normalized:" << endl;
//   cout << "fx: " << v[0] / factor << endl;
//   cout << "fy: " << -v[1] * v[0] / factor << endl;
//   cout << "s: " << v[2] * v[0] / factor << endl;
//   cout << "ox: " << (v[3] - double(xsize) / 2.0) / factor << endl;
//   cout << "oy: " << (v[4] - double(ysize) / 2.0) / factor << endl;
//   cout << endl;
}

/**
 * Output.
 * @param camera the camera object
 * @param xsize image xsize
 * @param ysize image ysize
 */
void printInterpretedExtrinsic(Camera& camera, int index, int xsize, int ysize)
{
  //const double factor = std::max(double(xsize), double(double(ysize))) / 2.0;

  Vector v = camera.MakeVector();
  cout << "Extrinsic parameters camera " << index << ":" << endl;
  cout << "t: " << v[5] << ", " << v[6] << ", " << v[7] << endl;
  cout << "angles: " << v[8] << ", " << v[9] << ", " << v[10] << endl;
  cout << endl;
}

int main(int argc, char** argv)
{
  cout << "Parameters: <image files>." << endl;

  if (argc < 4)
    {
      cerr << "Too few parameters. At least 3 images required." << endl;
    }

  vector<Image> imagesOriginal;
  imagesOriginal.reserve(argc);

  for (int i = 1; i < argc; ++i)
    {
      cout << "Reading: " << argv[i] << endl;
      Image image = ReadImg(argv[i]);
      imagesOriginal.push_back(image);
      //Show(ON, image);
    }

//   Camera camera;
//   Calibrate(images, camera, false);
//   cout << "Calibration result as reported by ICE:" << endl;
//   cout << camera.toString() << endl;

  vector<Camera> cameras(imagesOriginal.size());
  {
    cout << "Performing calibration..." << endl << endl;
    vector<Image> images(imagesOriginal.size());
    Image d = NewImg(imagesOriginal[0]);
    Image m = NewImg(imagesOriginal[0]);
    Show(OVERLAY, d, m);

    for (unsigned int i = 0; i < imagesOriginal.size(); ++i)
      {
        images[i] = NewImg(imagesOriginal[i], true);
      }

    Calibrate(images, cameras, false, d, m);

//     printInterpretedIntrinsic(cameras[0], images[0]->xsize, images[0]->ysize);
    for (unsigned int i = 0; i < cameras.size(); ++i)
      {
        printInterpretedExtrinsic(cameras[i], i,
                                  images[0]->xsize, images[0]->ysize);
      }
  }

  return 0;
}
