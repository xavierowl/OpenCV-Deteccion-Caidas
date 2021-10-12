
// Librerías que contienen funciones estándar de C++
#include <iostream>
#include <cstdlib>
#include <unistd.h>

// Librería que contiene funciones matemáticas
#include <cmath>

// Librerías de OpenCV

// Se pueden cargar todas las Librerías incluyendo
//#include <opencv2/opencv.hpp>

// Contiene las definiciones fundamentales de las matrices e imágenes 
#include <opencv2/core/core.hpp> 
// Procesamiento de imágenes
#include <opencv2/imgproc/imgproc.hpp>
// Códecs de imágenes
#include <opencv2/imgcodecs/imgcodecs.hpp>
// Manejo de ventanas y eventos de ratón, teclado, etc.
#include <opencv2/highgui/highgui.hpp>
// Lectura de video
#include <opencv2/video/video.hpp>
// Escritura de video
#include <opencv2/videoio/videoio.hpp>

#include <time.h>

using namespace std;
using namespace cv;

//Declaración de la estructura FIFO para la generación del contenido multimedia
struct nodo{
  Mat dato;
  struct nodo  *sig;       
       
}*inicio=NULL,*ult=NULL,*aux;

void  agregarElemento(Mat imagen){
    aux= new nodo;  
    aux->dato=imagen;
    if(inicio==NULL){
     inicio=ult=aux; 
     aux->sig=NULL;                                
    }else{
    aux->sig=NULL;
    ult->sig=aux;
    ult=aux;     
    }       
}

void eliminarElemento(){
    aux = inicio;
    if (inicio != NULL) {
        if (aux->sig != NULL) {
            aux = aux->sig;            
            free(inicio);
            inicio = aux;
        } else {
            free(inicio);
            inicio = NULL;
            ult = NULL;
        }
    }    
}

int contarElementos(){
    aux=inicio;
    int elementos = 0;
    while(aux!=NULL){        
        aux=aux->sig;                  
        elementos += 1;
        }
    return elementos;
}

void generarVideo(){
    //Variables para la escritura de video
    Size fs(640, 480);
    int fps = 20;
    VideoWriter oVideoWriter("video.mp4", 0x7634706d,
    fps, fs, true);
    aux=inicio;
    while(aux!=NULL){
        oVideoWriter.write(aux->dato);
        aux=aux->sig;
    }
}

int main(int argc, char *argv[]){    
    Ptr<BackgroundSubtractor> pBackSub = createBackgroundSubtractorKNN(1000, 500, true);
    Mat elementoEstructurante = getStructuringElement(MORPH_CROSS, Size(3,3)) * 255;    
    
    //Se crean las ventanas
    namedWindow("Video", WINDOW_AUTOSIZE);
    moveWindow("Video", 100, 73);
    
    //VideoCapture video(0);
    VideoCapture video("videos/no-caidas/17.mp4");
    
    Mat fondo;
    Mat objeto;
    Mat bordes;
    int contornoPrincipal =  0;
    vector<vector<Point> > contornos;
    vector<Vec4i> jerarquia;
    Rect area;
    
    Moments momentos;
    double cx;
    double cy;
    
    bool existeContorno = false;
    
    int ancho;
    int alto;
    int caidas = 0;
    double segundos = 0.0;
    time_t tiempoInicial = time(0);
    
    if(video.isOpened()){
        while(3==3){
            cout<<"Detectando"<<endl;
            //Validacion de 40 frames para la construccion del video
            if(contarElementos()>39){
                eliminarElemento();
                }
            
            //Reinicio del contador del tiempo
            if(difftime( time(0), tiempoInicial) > 6){
                cout << "10 s. sin caídas. S:" << difftime( time(0), tiempoInicial) << endl;
                caidas = 0;
                tiempoInicial= time(0);
            }
            
            video >> fondo;
            
            agregarElemento(fondo);
            
            resize(fondo, fondo, Size(), 0.7,0.7);
            
            existeContorno = false;
            contornoPrincipal = 0;
    
            GaussianBlur(fondo, fondo, Size(3, 3), 2, 2);
            pBackSub->apply(fondo, objeto);
            
            erode(objeto, objeto, elementoEstructurante);
            dilate(objeto, objeto, elementoEstructurante);
            
            // Se detectan los bordes del objeto
            Laplacian(objeto, bordes, CV_16S, 3);
            convertScaleAbs(bordes, bordes);
            
            // Se encuentran los contornos del objeto
            findContours(bordes,contornos,jerarquia,RETR_TREE,CHAIN_APPROX_SIMPLE);
            
            //Obtención del contorno más grande
            for(int i = 0; i < contornos.size(); i++){
                if(contornos[contornoPrincipal].size() < contornos[i].size()){
                    contornoPrincipal = i;
                    existeContorno = true;
                }
            }
            
            if(existeContorno){                
                area = boundingRect(contornos[contornoPrincipal]);
                
                ancho = area.br().x - area.tl().x;
                alto = area.br().y - area.tl().y;
                if((ancho>=alto || ancho == alto) && ancho>150 && alto >100 && ancho<300 && alto <300){
                    caidas += 1;
                    if(caidas >= 10){
                        segundos = difftime( time(0), tiempoInicial);
                        if(segundos <= 10){
                            cout<<"Posible caída, realizando análisis"<<endl;
                            generarVideo();
                            //sleep(5);
                            imwrite("Soket/Validacion/deteccion.jpg", fondo);
                            int result = system("python3 Soket/Client/client.py 1");                            
                            caidas = 0;
                            tiempoInicial = time(0);
                            }
                        }
                    //Un resultado exitoso devolverá cero
                    //cout << "El resultado es: "<<result<<endl;                    
                    //cout<<"Columnas: "<<ancho<<endl;
                    //cout<<"Filas: "<<alto<<endl;
                }
                rectangle(fondo, area.tl(), area.br(), Scalar(0,0 ,255), 2 );
                //drawContours(fondo,contornos,contornoPrincipal,Scalar(255,255,255),1,LINE_8, jerarquia,8);
            }
                    
            imshow("Video", fondo);
            
            if(fondo.rows == 0 || fondo.cols == 0)
                break;
            
            if(waitKey(23)==27)
                break;
            
        }
        destroyAllWindows();
    }
    else{
        
        cout<<"No se ha podido capturar el video";
    }
    
    return 0;
}


