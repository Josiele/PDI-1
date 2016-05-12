#include <iostream>
#include <math.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

float gammaH;
int gammaH_slider = 60;
int gammaH_slider_max = 100;

float gammaL;
int gammaL_slider = 20;
int gammaL_slider_max = 100;

float c;
int c_slider = 1;
int c_slider_max = 100;

float D0;
int D0_slider = 20;
int D0_slider_max = 100;

char TrackbarName[50];

void on_trackbar_gammaH(int, void*){
    gammaH = (float) gammaH_slider;
}

void on_trackbar_gammaL(int, void*){
    gammaL = (float) gammaL_slider;
}

void on_trackbar_c(int, void*){
    c = (float) c_slider;
}

void on_trackbar_D0(int, void*){
    D0 = (float) D0_slider;
}

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
            tmp.at<float>(i,j) = (gamaH - gamaL)*(1-exp(pot)) + gamaL;
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
    char key;
    
    img = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
    if(!img.data)
        cout << "erro ao tentar abrir a imagem " << argv[1] << endl;
    
    namedWindow("Imagem Original", 1);
    
    sprintf( TrackbarName, "Gamma H: ");
    createTrackbar( TrackbarName, "Imagem Original",
                   &gammaH_slider,
                   gammaH_slider_max,
                   on_trackbar_gammaH );
    on_trackbar_gammaH(gammaH_slider, 0 );
    
    sprintf( TrackbarName, "Gamma L: ");
    createTrackbar( TrackbarName, "Imagem Original",
                   &gammaL_slider,
                   gammaL_slider_max,
                   on_trackbar_gammaL );
    on_trackbar_gammaL(gammaL_slider, 0 );
    
    sprintf( TrackbarName, "C: ");
    createTrackbar( TrackbarName, "Imagem Original",
                   &c_slider,
                   c_slider_max,
                   on_trackbar_c );
    on_trackbar_c(c_slider, 0 );
    
    sprintf( TrackbarName, "D0: ");
    createTrackbar( TrackbarName, "Imagem Original",
                   &D0_slider,
                   D0_slider_max,
                   on_trackbar_D0 );
    on_trackbar_D0(D0_slider, 0 );
    
    imshow("Imagem Original", img);
    
    for (;;) {
    
        preparaDFT(img, complexImage);
        
        log(complexImage+1, complexImage);
    
        calculaDFT(complexImage);
    
        geraFiltroHomomorfico(complexImage, filtro, gammaH, gammaL, D0, c, true);
    
        // aplica o filtro frequencial
        mulSpectrums(complexImage, filtro, complexImage, 0);
    
        calculaDFTInverso(complexImage, imgFiltrada, img);
        
        exp(imgFiltrada, imgFiltrada);
        normalize(imgFiltrada, imgFiltrada, 0, 1, CV_MINMAX);
    
        imshow("Imagem Filtrada", imgFiltrada);
        
        key = (char) waitKey(10);
        if (key == 27) {
            break;
        }
    }

    return 0;
}
