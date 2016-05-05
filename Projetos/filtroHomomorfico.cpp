#include <iostream>
#include <math.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

// troca os quadrantes da imagem da DFT
void deslocaDFT(Mat& image ){
    Mat tmp, A, B, C, D;
    
    // se a imagem tiver tamanho impar, recorta a regiao para
    // evitar cópias de tamanho desigual
    image = image(Rect(0, 0, image.cols & -2, image.rows & -2));
    int cx = image.cols/2;
    int cy = image.rows/2;
    
    // reorganiza os quadrantes da transformada
    // A B   ->  D C
    // C D       B A
    A = image(Rect(0, 0, cx, cy));
    B = image(Rect(cx, 0, cx, cy));
    C = image(Rect(0, cy, cx, cy));
    D = image(Rect(cx, cy, cx, cy));
    
    // A <-> D
    A.copyTo(tmp);  D.copyTo(A);  tmp.copyTo(D);
    
    // C <-> B
    C.copyTo(tmp);  B.copyTo(C);  tmp.copyTo(B);
}

void preparaDFT(Mat img, Mat& complexImg, bool print = false){
    //calcula tamanho ideal para o calcula da DFT
    int M = getOptimalDFTSize(img.rows);
    int N = getOptimalDFTSize(img.cols);
    
    Mat paddedImg;
    Mat_<float> realInput, zeros;
    //aumenta a imagem para o tamanho ideal
    copyMakeBorder(img, paddedImg, 0, M - img.rows, 0, N - img.cols, BORDER_CONSTANT, Scalar::all(0));
    //imprime a imagem redimensionada caso solicitado
    if(print)
        imshow("Imagem Redimensionada", paddedImg);
    //inicializa uma matriz de zeros (parte imaginaria)
    zeros = Mat_<float>::zeros(paddedImg.size());
    complexImg = Mat(paddedImg.size(), CV_32FC2, Scalar(0));
    //converte a parte real da imagem para float
    realInput = Mat_<float>(paddedImg);
    //junta parte real e imaginaria em uma matriz de dois canais
    vector<Mat> planos;
    planos.push_back(realInput);
    planos.push_back(zeros);
    merge(planos, complexImg);
}

void calculaDFT(Mat complexImg, bool print = false){
    //calcula o ln de cada elemento da matriz de dois canais
    log(complexImg+1,complexImg);
    
    //calcula o dft da imagem
    dft(complexImg, complexImg);
    //reorganiza o espectro
    deslocaDFT(complexImg);
    //imprime o espectro caso solicitado
    if(print){
        vector<Mat> planos;
        split(complexImg, planos);
        //normalize(planos[0], planos[0], 0, 1, CV_MINMAX);
        imshow("Espectro da Imagem", planos[1]);
    }
}

void geraFiltroHomomorfico(Mat complexImg, Mat& filtro, float gamaH, float gamaL, float raio, float var, bool print = false){
    vector<Mat> planos;
    //separa os canais da matriz da imagem complexa
    split(complexImg, planos);
    
    Mat img = planos[0];
    //cria matriz temporaria de mesmo tamanho da matriz da imagem
    Mat tmp;
    tmp = Mat(img.size(), CV_32F, Scalar(0));
    
    int M = tmp.rows;
    int N = tmp.cols;
    //preenche a matriz temporaria com o filtro
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            float Duv = (i - M/2)*(i - M/2)+(j - N/2)*(j - N/2);
            Duv = sqrt(Duv);
            float pot = -var*((Duv*Duv)/(raio*raio));
            tmp.at<float>(i,j) = (gamaH - gamaL)*exp(pot) + gamaL;
        }
    }
    
    filtro = Mat(img.size(), CV_32FC2, Scalar(0));
    //cria uma matriz de filtro com dois canais iguais
    planos.clear();
    planos.push_back(tmp);
    planos.push_back(tmp);
    merge(planos, filtro);
    //imprime o filtro caso solicitado
    if(print)
        imshow("filtro homomorfico", tmp);
}

void calculaDFTInverso(Mat complexImg, Mat& imgFiltrada, Mat imgOriginal){
    //reorganiza o espectro
    deslocaDFT(complexImg);
    //calcula DFT inversa
    idft(complexImg, complexImg);
    //separa os canais
    vector<Mat> planos;
    split(complexImg, planos);
    
    //calcula exponencial da parte real da imagem complexa
    //exp(planos[0], planos[0]);
    
    //normaliza a imagem para valores float entre 0 e 1
    normalize(planos[0], planos[0], 0, 1, CV_MINMAX);
    imgFiltrada = planos[0];
    /*
    //redimensiona a imagem para o tamanho da imagem original
    if(imgOriginal.cols == planos[0].cols && imgOriginal.rows == planos[0].rows)
        imgFiltrada = planos[0];
    else
        Mat(planos[0], Rect(0,0,imgOriginal.rows-1, imgOriginal.cols-1)).copyTo(imgFiltrada);
    */
}

int main(int, char** argv){
    Mat img, complexImage, filtro, imgFiltrada;
    
    img = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
    if(!img.data)
        cout << "erro ao tentar abrir a imagem " << argv[1] << endl;
    
    imshow("Imagem Original", img);
    
    preparaDFT(img, complexImage);
    
    calculaDFT(complexImage);
    
    geraFiltroHomomorfico(complexImage, filtro, 1.1, 0.9, 20, 1);
    
    // aplica o filtro frequencial
    mulSpectrums(complexImage, filtro, complexImage, 0);
    
    calculaDFTInverso(complexImage, imgFiltrada, img);
    
    imshow("Imagem Filtrada", imgFiltrada);
    
    waitKey();
    return 0;
}
