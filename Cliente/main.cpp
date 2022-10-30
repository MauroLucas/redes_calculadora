#include <iostream>
#include <winsock2.h>
#include <string>
#include <thread>
using namespace std;

//Varibles Globales
int tiempo = 0;
bool timeOut = false;

void temporizador(int segundos);

class Client{
public:
    WSADATA WSAData;
    SOCKET server;
    SOCKADDR_IN addr;
    char buffer[4000];
    char mensaje[4000];
    char ip[1024];
    int puerto, tiempoMaximo;
    bool servidorConectado;

    Client()
    {
        //Limpieza de variables
        memset(ip, 0, sizeof(ip));
        memset(buffer, 0, sizeof(buffer));
        servidorConectado = false;
        timeOut = false;
        tiempoMaximo = 60;

    }
    void IniciarCliente()
    {

        //Inicio el contador
        thread th(temporizador,tiempoMaximo);

        while(true){
        system("cls");
        cout<<"Conectando al servidor..."<<endl<<endl;
        WSAStartup(MAKEWORD(2,0), &WSAData);
        //Creacion del Socket
        server = socket(AF_INET, SOCK_STREAM, 0);
        if(server == INVALID_SOCKET)
        {
            cout<<"No se pudo crear el socket"<<endl;
            system("pause");
            WSACleanup();
            exit(1);
        }
        cout<<"Ingrese la direccion IP a la que desea conectarse"<<endl;
        cin>>this->ip;
        cout<<"Ingrese el puerto"<<endl;
        cin>>this->puerto;
        addr.sin_addr.s_addr = inet_addr(ip);
        addr.sin_family = AF_INET;
        addr.sin_port = htons(puerto);

        //Conectar al servidor
        int connResult = connect(server, (SOCKADDR *)&addr, sizeof(addr));
        if(connResult == SOCKET_ERROR)
        {
            closesocket(server);
            WSACleanup();
            cout<<"No se pudo conectar al servidor"<<endl;
            system("pause");
        }
        else{
            servidorConectado = true;
            tiempo = 0;

            while(servidorConectado)
            {
                MenuPrincipal();

            }
        }
        //Limpiar Winsock
        WSACleanup();
        }

    }

    void MenuPrincipal()
    {
        system("cls");
        int opcion = 0;
        cout<<"Que desea realizar?"<<endl;
        cout<<"1 - Realizar Calculo"<<endl;
        cout<<"2 - Ver registro de actividades"<<endl;
        cout<<"3 - Cerrar Sesion"<<endl;
        cin>>opcion;
        switch(opcion){
            case 1: RealizarCalculo();
            break;

            case 2: VerRegistroDeActividades();
            break;

            case 3: CerrarSocket();
            break;
            default: break;
        }

    }

    void Recibir()
    {
        recv(server, mensaje, sizeof(mensaje), 0);
        cout << mensaje << endl;
        memset(mensaje, 0, sizeof(mensaje));
    }

    bool SuperoElTiempo(int segundos)
    {
        bool superoElTiempo = false;
        if(tiempo>segundos){
            superoElTiempo = true;
        }
        return superoElTiempo;
    }
    void RealizarCalculo(){
        if(!SuperoElTiempo(tiempoMaximo)){


        string bufferAux = "";
        char input[1021];
        int i,cantCaracteres = 0;
        bool mensajeValido = true;
        cout<<"Ingrese el calculo que desea realizar: ";
        do{
           mensajeValido = true;
           cin>>bufferAux;
           //fgets(input,1021,stdin);
           //for(i=0; i<input[i]!='\0';i++){
            //bufferAux[i] = input[i];
           //}
           if(strlen(bufferAux.c_str())>20 || bufferAux[0] == '\n'){
            mensajeValido = false;
            cout<<"La operacion debe tener entre 1 y 20 caracteres"<<endl;
           }
           if(bufferAux != "volver" && mensajeValido){
            buffer[0] = 'a';
            for(i = 1; i<1024;i++){
                this->buffer[i] = bufferAux[i-1];
            }
            send(server, buffer, sizeof(buffer), 0);
            memset(buffer, 0, sizeof(buffer));
            Recibir();
           }

        }while(bufferAux != "volver");
        }
        else{
            cout<<"Cliente Desconectado por Inactividad"<<endl;
            CerrarSocket();
        }
    }
    void VerRegistroDeActividades(){
        string linea="";
        this->buffer[0] = 'b';
        send(server,buffer,sizeof(buffer),0);
        memset(buffer, 0, sizeof(buffer));
        while(mensaje[0] != 'E' && mensaje[1] != 'O' && mensaje[2] != 'F'){
            recv(server, mensaje, sizeof(mensaje), 0);

            if(mensaje[0] != 'E' && mensaje[1] != 'O' && mensaje[2] != 'F'){
                cout<<mensaje<<endl;
                memset(mensaje, 0, sizeof(mensaje));
            }
        }

      system("pause");
    }

    void CerrarSocket()
    {
       buffer[0] = 'c';
       EnviarMensaje(mensaje);
       send(server,buffer,sizeof(buffer),0);
       memset(buffer, 0, sizeof(buffer));
       servidorConectado = false;
       tiempo = 0;
       closesocket(server);
       cout<<"Se cerro la conexion con el servidor"<<endl;
       system("pause");
       //WSACleanup();
       //cout << "Socket cerrado." << endl << endl;
    }
    void EnviarMensaje(string mensaje){
        for(int i = 0; i<(int)strlen(mensaje.c_str());i++){
            this->mensaje[i] = mensaje[i];
        }
        send(server, buffer, sizeof(buffer), 0);
        memset(buffer, 0, sizeof(buffer));
    }
};

 void temporizador(int segundos)
    {

        while(true)
        {
            Sleep(1000); //1 segundo
            //if(tiempo == segundos)
            //{
              //  cout<<"Se llego al limite de tiempo"<<endl;

            //}
            tiempo++;

        }
    }

int main()
{
    Client *Cliente = new Client();
    Cliente->IniciarCliente();

}
