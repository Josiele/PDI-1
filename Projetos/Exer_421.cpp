#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc, char** argv){
    Mat image;
    int width, height;
    int contadorObjetos, tonDeCinza;
  
    CvPoint p;
    image = imread(argv[1],CV_LOAD_IMAGE_GRAYSCALE);
  
    if(!image.data){
        std::cout << "imagem nao carregou corretamente\n";
        return(-1);
    }
    width=image.size().width;
    height=image.size().height;

    p.x=0;
    p.y=0;

    // busca objetos com buracos presentes
    tonDeCinza=0;
    contadorObjetos = 0;

    for(int i=0; i<height; i++){
        for(int j=0; j<width; j++){
          if(image.at<uchar>(i,j) == 255){
            // achou um objeto
            tonDeCinza++;
            contadorObjetos++;

            p.x=j;
            p.y=i;
            floodFill(image,p,tonDeCinza);

            if(tonDeCinza >= 255)
                tonDeCinza = 0;
          }
        }
    }

    cout << "Qtd. objetos: " << contadorObjetos << endl;

    imshow("image", image);
    imwrite("outroNome.png", image);
    waitKey();

    return 0;
}
