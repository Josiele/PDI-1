#include <math.h>
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

Mat generateHF(Mat toBeFiltered, double Dzero, int Order){
   int M = toBeFiltered.rows;
   int N = toBeFiltered.cols;
   Point centre = Point(M/2,N/2);
   Mat filter = Mat(toBeFiltered.size(), toBeFiltered.type());
   for(int u = 0; u < M; u++){
      for(int v = 0; v < N; v++){
         double Duv = (double) sqrt(pow((double)(u-centre.x),2.0)+pow((double)(v-centre.y),2.0));
         filter.at<double>(u,v) = 1/(1+pow((double)(Dzero/Duv), (double)(2*Order)));   
      }
   }
   return filter;
}

int main(int argc, char** argv){
   Mat img;
   img = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
   if(!img.data)
      cout << "erro loading image " << argv[1];
   Mat filter = generateHF(img, 0.25, 1);
   imshow("image", img);
   imshow("Filter", filter);
   waitKey();
   return 0;
}

