#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

double alfa;
int alfa_slider = 0;
int alfa_slider_max = 100;

int top_slider = 0;
int top_slider_max = 100;
//image1 é a imagem borada e image2 é a imagem original
Mat image1, image2, blended;
Mat imageTop;

char TrackbarName[50];

void on_trackbar_blend(int, void*){
    //calcula o alpha de decaimento
    alfa = (double) alfa_slider/alfa_slider_max ;
    addWeighted( imageTop, alfa, image1, 1-alfa, 0.0, blended);
    imshow("TiltShift", blended);
}
//compõe as duas imagens, deixando a parte do meio em foco
void on_trackbar_line(int, void*){
    image1.copyTo(imageTop);
    int limit = top_slider;
    if(limit > 0){
        Mat tmp = image2(Rect(0, image2.rows/2-limit, 256, 2*limit));
        tmp.copyTo(imageTop(Rect(0, imageTop.rows/2-limit, 256, 2*limit)));
    }
    on_trackbar_blend(alfa_slider,0);
}

int main(int argvc, char** argv){
    image1 = imread("blend1.jpg");
    image2 = imread("blend2.jpg");
    image2.copyTo(imageTop);
    namedWindow("TiltShift", 1);
    top_slider_max = imageTop.rows/2;
    
    sprintf( TrackbarName, "Alpha x %d", alfa_slider_max );
    createTrackbar( TrackbarName, "TiltShift",
                   &alfa_slider,
                   alfa_slider_max,
                   on_trackbar_blend );
    on_trackbar_blend(alfa_slider, 0 );
    
    sprintf( TrackbarName, "Scanline x %d", top_slider_max );
    createTrackbar( TrackbarName, "TiltShift",
                   &top_slider,
                   top_slider_max,
                   on_trackbar_line );
    on_trackbar_line(top_slider, 0 );
    
    waitKey(0);
    return 0;
}
