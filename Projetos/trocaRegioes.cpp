#include <iostream>
#include <cv.h>
#include "opencv2/highgui/highgui.hpp"
#define NSUBIMAGES 4

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

using namespace cv;
using namespace std;

int main(int, char** argv){
    Mat img;
    Mat newImg;
    
    img = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
    
    if (!img.data) {
        cout << "Image " << argv[1] << " could not be opened";
    }
    
    namedWindow("mainWindow", WINDOW_AUTOSIZE);
    
    imshow("mainWindow", img);
    
    int imgRows = img.rows;
    int imgCols = img.cols;
    int subImageRows = imgRows/(NSUBIMAGES/2);
    int subImageCols = imgCols/(NSUBIMAGES/2);
    
    vector<int> vert;
    vector<Mat> subImgs;
    vector<int> vertPuzzle;
    
    cout << "image size: " << imgRows << "x" << imgCols << endl;
    
    cout << "sub image size: " << subImageRows << "x" << subImageCols << endl;
    
    //cria vetor com os pontos iniciais de cada sub imagem
    for (int i =0 ; i < NSUBIMAGES/2; i++) {
        for (int j = 0; j < NSUBIMAGES/2; j++) {
            vert.push_back(i*subImageRows);
            vert.push_back(j*subImageCols);
            cout << "vertice: (" << i*subImageRows << "," << j*subImageCols << ")" << endl;
        }
    }
    
    //preenche vetor com sub imagens da imagem inicial, usando construtor de ROI(region on interest)
    for (int i = 0; i < NSUBIMAGES; i++) {
        subImgs.push_back(Mat(img, Rect(vert.at(2*i+1), vert.at(2*i), subImageCols, subImageRows)));
        cout << "Sub Image created !" << endl;
    }
    /*
    namedWindow("subImage1", WINDOW_AUTOSIZE);
    namedWindow("subImage2", WINDOW_AUTOSIZE);
    namedWindow("subImage3", WINDOW_AUTOSIZE);
    namedWindow("subImage4", WINDOW_AUTOSIZE);
    imshow("subImage1", subImgs.at(0));
    imshow("subImage2", subImgs.at(1));
    imshow("subImage3", subImgs.at(2));
    imshow("subImage4", subImgs.at(3));
    */
    //cria vetor aleatorio para reconstrução das subimagens
    //vertPuzzle.push_back(rand() % NSUBIMAGES);
    while (vertPuzzle.size()<NSUBIMAGES) {
        int temp = rand() % NSUBIMAGES;
        int flag = -1;
        for (int i = 0; i < vertPuzzle.size() ; i++) {
            if (temp == vertPuzzle.at(i)) {
                flag = 1;
            }
        }
        if (flag == -1) {
            vertPuzzle.push_back(temp);
        }
    }
    
    cout << "vetor de reordenção: ";
    for (int i=0; i< vertPuzzle.size(); i++) {
        cout << vertPuzzle.at(i) << ", ";
    }
    cout << endl;;
    
    
    newImg = img.clone();
    //rotina para copiar subimage para RIO na imagem clone
    for (int i = 0; i < NSUBIMAGES; i++) {
        int x = vert.at(2*vertPuzzle.at(i)+1);
        int y = vert.at(2*vertPuzzle.at(i));
        Mat(subImgs.at(i), Rect(0, 0, subImageCols, subImageRows)).copyTo(Mat(newImg, Rect(x, y, subImageCols,subImageRows)));
    }
    
    namedWindow("PuzzleWindow", WINDOW_AUTOSIZE);
    imshow("PuzzleWindow", newImg);
     
    waitKey();
    return 0;
}
