#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc, char** argv){
  Mat image;
  Mat equalizedImage;
  Mat imageGray;
  int width, height;
  //objeto de captura de frames
  VideoCapture cap;
  //vetor contendo as porcoes r g b da imagem
  vector<Mat> planes;
  //histogramas
  Mat histR, histG, histB;
  //tamanho do vetor histograma
  int nbins = 64;
  //parametros para calculo do histograma
  float range[] = {0, 256};
  const float *histrange = { range };
  bool uniform = true;
  bool acummulate = false;

  cap.open(0);//inicia objeto para captura de imagens

  //testa abertura
  if(!cap.isOpened()){
    cout << "cameras indisponiveis";
    return -1;
  }

  //le altura e largura do frame sendo capturado
  width  = cap.get(CV_CAP_PROP_FRAME_WIDTH);
  height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);

  //mostra altura e largura na tela 
  cout << "largura = " << width << endl;
  cout << "altura  = " << height << endl;

  //define largura e altura do histograma
  int histw = nbins, histh = nbins/2;
  //cria elemento para desenhar os histogramas
  Mat histImgR(histh, histw, CV_8UC3, Scalar(0,0,0));
  Mat histImgG(histh, histw, CV_8UC3, Scalar(0,0,0));
  Mat histImgB(histh, histw, CV_8UC3, Scalar(0,0,0));

  while(1){
    cap >> image;//transfere o frama capturado para o elemento tipo Mat
    split (image, planes);//divide os valores r,g e b em elementos diferentes
    //calcula histograma de cada elemento de cor
    calcHist(&planes[0], 1, 0, Mat(), histR, 1,
             &nbins, &histrange,
             uniform, acummulate);
    calcHist(&planes[1], 1, 0, Mat(), histG, 1,
             &nbins, &histrange,
             uniform, acummulate);
    calcHist(&planes[2], 1, 0, Mat(), histB, 1,
             &nbins, &histrange,
             uniform, acummulate);
    //normaliza os histogramas
    normalize(histR, histR, 0, histImgR.rows, NORM_MINMAX, -1, Mat());
    normalize(histG, histB, 0, histImgR.rows, NORM_MINMAX, -1, Mat());
    normalize(histB, histB, 0, histImgR.rows, NORM_MINMAX, -1, Mat());

    //pinta os elementos dos histogramas todo de preto
    histImgR.setTo(Scalar(0));
    histImgG.setTo(Scalar(0));
    histImgB.setTo(Scalar(0));

    //desenha histograma
    for(int i=0; i<nbins; i++){
      line(histImgR, Point(i, histh),
           Point(i, cvRound(histR.at<float>(i))),
           Scalar(0, 0, 255), 1, 8, 0);
      line(histImgG, Point(i, histh),
           Point(i, cvRound(histG.at<float>(i))),
           Scalar(0, 255, 0), 1, 8, 0);
      line(histImgB, Point(i, histh),
           Point(i, cvRound(histB.at<float>(i))),
           Scalar(255, 0, 0), 1, 8, 0);
    }
    histImgR.copyTo(image(Rect(0, 0       ,nbins, histh)));
    histImgG.copyTo(image(Rect(0, histh   ,nbins, histh)));
    histImgB.copyTo(image(Rect(0, 2*histh ,nbins, histh)));
    imshow("image", image);

    //converte imagem para tons de cinza
    cvtColor(image, imageGray, CV_BGR2GRAY);
    //equaliza a imagem em tons de cinza
    equalizeHist(imageGray, equalizedImage);
    
    imshow("Equalized Gray Image", equalizedImage);

    if(waitKey(27) >= 0) break;
  }
  return 0;
}



