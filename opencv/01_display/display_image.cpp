#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs/legacy/constants_c.h>

using namespace cv;

// https://docs.opencv.org/master/db/df5/tutorial_linux_gcc_cmake.html

int main(int argc, char** argv )
{
  Mat image;
  image = imread ("jezus-redt.jpg", CV_LOAD_IMAGE_COLOR);
  namedWindow ("Display Image", WINDOW_AUTOSIZE);
  imshow ("Display Image", image);
  waitKey (0);
  return 0;
}
