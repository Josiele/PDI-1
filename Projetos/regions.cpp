#include <iostream>
#include <cv.h>
#include "opencv2/highgui/highgui.hpp" 

/*
    Quatro parametros são entrada para esse programa, Xi, Xf, Yi e Yf, que são
    usado para determinar a região que será negativada.
 
*/


using namespace cv;
using namespace std;

int main(int, char** argv){
    Mat img;
    int P1X, P1Y, P2X, P2Y;

    img= imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
    if(!img.data)
       cout << "error - image can't open" << endl;
    
    int xMax = img.rows;
    int yMax = img.cols;
    
    cout << "size (" << xMax << ", " << yMax << ")" << endl;
    
    cout << "Xi = ";
    cin >> P1X;
    cout << "Xf = ";
    cin >> P2X;
    
    cout << "Yi = ";
    cin >> P1Y;
    cout << "Yf = ";
    cin >> P2Y;
    
    for (int i = P1X; i < P2X; i++) {
        for (int j = P1Y; j < P2Y; j++) {
            img.at<uchar>(i,j) = 255 - img.at<uchar>(i,j); // Operação de negativo pixel = 255 - pixel
        }
    }

    namedWindow("window",WINDOW_AUTOSIZE);
    imshow("window", img);
    waitKey();
    return 0;
}
