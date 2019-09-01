#include <opencv2/opencv.hpp>

using namespace cv;

// https://docs.opencv.org/master/db/d64/tutorial_load_save_image.html

int main(int argc, char** argv )
{
  Mat image;
  image = imread ("jezus-redt.jpg", IMREAD_COLOR);
  
  Mat gray_image;
  cvtColor (image, gray_image, COLOR_BGR2GRAY);

  imwrite ("gray.jpg", gray_image);
  
  namedWindow ("Original", WINDOW_AUTOSIZE);
  namedWindow ("Gray", WINDOW_AUTOSIZE);

  imshow ("Original", image);
  imshow ("Gray", gray_image);
  
  waitKey (0);
  return 0;
}
