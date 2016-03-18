#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc, char** argv){
  //matriz para armazenar o quadro capturado
  Mat image;
  int width, height;
  //elemento de captura
  VideoCapture cap;
  //matriz para armezar o histograma do quadro atual e do quadro anterior
  Mat hist, oldHist;
  //entradas para a funcao de calculo do histograma
  int nbins = 64;
  float range[] = {0, 256};
  const float *histrange = { range };
  bool uniform = true;
  bool acummulate = false;

  //abre elemento de captura
  cap.open(0);

  //verifica abertura do elemento de captura
  if(!cap.isOpened()){
    cout << "cameras indisponiveis";
    return -1;
  }
  
  //ler o tamanho do frama sendo capturado
  width  = cap.get(CV_CAP_PROP_FRAME_WIDTH);
  height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);

  cout << "largura = " << width << endl;
  cout << "altura  = " << height << endl;

  int histw = nbins, histh = nbins/2;//calcula largura e altura da imagem do histograma
  Mat histImg(histh, histw, CV_8UC1, Scalar(0));//imagem do histograma, inicialmente toda preta
  int count = 0; //conta o numero da iteracao
  double histDiff = 0; //armezena a differenca entre histogramas

  while(1){   
    //captura a imagem usando a webcam e converte o frame de RGB para GRAYSCALE
    cap >> image;
    cvtColor(image, image, CV_BGR2GRAY);

    //calcula e normaliza o histograma da imagem capturada
    calcHist(&image, 1, 0, Mat(), hist, 1,
             &nbins, &histrange,
             uniform, acummulate);
    normalize(hist, hist, 0, histImg.rows, NORM_MINMAX, -1, Mat());

    //pinta a matriz da imagem do histograma toda de preto
    histImg.setTo(Scalar(0));

    //desenha o histograma 
    for(int i=0; i<nbins; i++){
      line(histImg, Point(i, histh),
           Point(i, cvRound(hist.at<float>(i))),
           Scalar(255), 1, 8, 0);
   }
    
    //adiciona a imagem do histograma a imagem a ser mostrada
    histImg.copyTo(image(Rect(0, 0       ,nbins, histh)));
    imshow("image", image);
    
    //so calcula a diferenca entre os histogramas apartir da segunda captura de quadro
    if(count >=1)
       histDiff = compareHist(hist, oldHist, 0);
    //se houver diferenca de quadro maior que 0.2 escreve na tela o valor retornado pela funcao compareHist()
    if( histDiff < 0.98)
       cout << "motion detected, histDiff = " << histDiff << endl;
    //salva o histograma atual na variavel de histograma antigo para calcular diferencao
    oldHist = hist.clone();
    count++;
    if(waitKey(30) >= 0) break;
  }
  return 0;
}


