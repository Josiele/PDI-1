#include <iostream>
#include <cv.h>
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc, char** argv){
    Mat image;
    int width, height;
    const int tonDeCinzaPreto = 0, tonDeCinzaBranco = 255;
    const int novaCorFundo = 100, novaCorObjeto = 200;
    int contadorObjetosSemBuraco, contadorObjetosComBuraco;
  
    CvPoint p;
    image = imread(argv[1],CV_LOAD_IMAGE_GRAYSCALE);
  
    if(!image.data){
        std::cout << "imagem nao carregou corretamente\n";

        return(-1);
    }

    width = image.size().width;
    height = image.size().height;

    p.x = 0;
    p.y = 0;

    contadorObjetosSemBuraco = 0;
    contadorObjetosComBuraco = 0;

    Mat novaImagem = Mat::ones(width + 2, height + 2, CV_8U)*tonDeCinzaBranco;
    Mat(image, Rect(0, 0, width, height)).copyTo(Mat(novaImagem, Rect(1,1,width, height)));

    floodFill(novaImagem,p,tonDeCinzaPreto);

    floodFill(novaImagem,p,novaCorFundo);

    for(int i=0; i<height; i++){
        for(int j=0; j<width; j++){
            if(novaImagem.at<uchar>(i,j) == tonDeCinzaBranco){
                p.x=j;
                p.y=i;
                floodFill(novaImagem,p,novaCorObjeto);
                contadorObjetosSemBuraco++;
            }else if(novaImagem.at<uchar>(i,j) == tonDeCinzaPreto){
                p.x=j;
                p.y=i;
                floodFill(novaImagem,p,novaCorFundo);
                contadorObjetosComBuraco++;
            }
        }
    }

    p.x = 0;
    p.y = 0;
    floodFill(novaImagem,p,tonDeCinzaPreto);

    cout << "Qtd. objetos: " << contadorObjetosSemBuraco << endl;
    cout << "Qtd. objetos sem buraco: " << contadorObjetosSemBuraco - contadorObjetosComBuraco << endl;
    cout << "Qtd. objetos com buraco: " << contadorObjetosComBuraco << endl;

    imshow("bolhas.png", image);

    imwrite("novaImagem.png", novaImagem);
    imshow("Nova Imagem", novaImagem);

    waitKey();
    return 0;
}
