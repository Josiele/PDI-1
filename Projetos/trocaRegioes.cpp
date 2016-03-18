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
    
    int imgRows = img.rows;
    int imgCols = img.cols;
    int subImageRows = imgRows/(NSUBIMAGES/2);
    int subImageCols = imgCols/(NSUBIMAGES/2);
    
    vector<int> vert;
    //vector<Mat> subImgs;
    vector<int> vertPuzzle;
    
    cout << "image size: " << imgRows << "x" << imgCols << endl;
    
    cout << "sub image size: " << subImageRows << "x" << subImageCols << endl;
    
    //cria vetor com os pontos de inicio de cada sub imagem
    for (int i =0 ; i < NSUBIMAGES/2; i++) {
        for (int j = 0; j < NSUBIMAGES/2; j++) {
            vert.push_back(i*subImageRows);
            vert.push_back(j*subImageCols);
            cout << "vertice: (" << i*subImageRows << "," << j*subImageCols << ")" << endl;
        }
    }
    
    //cria vetor aleatorio para criacao do quebra cabeca
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
    
    //imprime o vetor de reordenacao na tela
    cout << "vetor de reordenção: ";
    for (int i=0; i< vertPuzzle.size(); i++) {
        cout << vertPuzzle.at(i) << ", ";
    }
    cout << endl;;
    
    
    newImg = img.clone();
    //rotina para copiar uma RIO da imagem original para outra posicao no quebra cabeca
    for (int i = 0; i < NSUBIMAGES; i++) {
        int x = vert.at(2*vertPuzzle.at(i)+1);
        int y = vert.at(2*vertPuzzle.at(i));
	int x0 = vert.at(2*i+1);
        int y0 = vert.at(2*i);
        Mat(img, Rect(x0, y0, subImageCols, subImageRows)).copyTo(Mat(newImg, Rect(x, y, subImageCols,subImageRows)));
    }
    
    namedWindow("PuzzleImage", WINDOW_AUTOSIZE);
    imshow("PuzzleImage", newImg);
    namedWindow("Image", WINDOW_AUTOSIZE);
    imshow("Image", img);
     
    waitKey();
    return 0;
}
