#include <iostream>
#include <winsock2.h>
#include <string>
#include <math.h>
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>

using namespace std;

class Server{
public:
    WSADATA WSAData;
    SOCKET server, client;
    SOCKADDR_IN serverAddr, clientAddr;
    char buffer[1024];
    char mensaje[4000];
    int puerto = 5000;
    Server()
    {
        serverLog("=============================");
        serverLog("=======Inicia Servidor=======");
        serverLog("=============================");
        WSAStartup(MAKEWORD(2,0), &WSAData);
        server = socket(AF_INET, SOCK_STREAM, 0);

        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(puerto);

        bind(server, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
        listen(server, 0);
        cout<<"Socket Creado. Puerto de escucha: " << to_string(puerto) << endl;
        serverLog("Socket Creado. Puerto de escucha: " + to_string(puerto));



        cout << "Escuchando para conexiones entrantes." << endl;
        int clientAddrSize = sizeof(clientAddr);
        if((client = accept(server, (SOCKADDR *)&clientAddr, &clientAddrSize)) != INVALID_SOCKET)
        {
            cout << "Conexion Aceptada" << endl;
            serverLog("Conexion Aceptada");
        }
    }

    string Recibir()
    {
      char opcion;
      int iResult;
      bool desconectado = false;
      while(!desconectado){
        iResult = recv(client, buffer, sizeof(buffer), 0);
        if(iResult == SOCKET_ERROR){
            cout<<"Cliente Desconcetado"<<endl;
            serverLog("Conexion Cerrada");
            desconectado = true;
        }
      }

      cout<<iResult<<endl;
      cout << "El cliente dice: " << buffer << endl;
      opcion = buffer[0];
      string mensaje;
      mensaje.assign(buffer);
      cout<<opcion<<endl;
      switch(opcion){
          case 'a': RealizarCalculo(mensaje);
          break;
          default: break;
      }
      memset(buffer, 0, sizeof(buffer));
    }
    void Enviar()
    {
        send(client, mensaje, sizeof(mensaje), 0);
        memset(mensaje, 0, sizeof(mensaje));
        cout << "Mensaje enviado!" << endl;
    }
    int convertirAEntero(string a){
        int i = 0;
        int num = 0;
        while (a[i] !=0){
            num = (a[i] - '0') + (num *10);
            i++;
        }
        return num;
    }
    int factorial(int x){
        int i, resultado = 1;
        for(i = 1; i<=x;i++){
            resultado = resultado * i;
        }
        return resultado;
    }
    bool validarCaracter(char c){
        bool esValido = false;

        if(c == '+' || c == '-' || c == '*' || c == '/' || c == '!' || c== '^'){
            esValido = true;
        }
        if(c == '0' || c == '1' || c == '2' || c == '3' || c == '4' || c == '5' || c == '6' || c == '7' || c == '8' || c == '9'){
            esValido = true;
        }
        return esValido;
    }
    bool validarCalculo(string mensaje){
        bool esValido = true;
        int cantCaracteres = 0;
        for(int i=0; mensaje[i]!='\0';i++){

            //Verifico que el caracter sea correcto
            if(!validarCaracter(mensaje[i])){
                esValido = false;
            }

            //Hago un conteo de los caracteres de tipo operacion
            if(mensaje[i] == '+'){
                cantCaracteres++;
            }
            if(mensaje[i] == '-'){
                cantCaracteres++;
            }
            if(mensaje[i] == '*'){
                cantCaracteres++;
            }
            if(mensaje[i] == '/'){
                cantCaracteres++;
            }
            if(mensaje[i] == '!'){
                cantCaracteres++;
            }
            if(mensaje[i] == '^'){
                cantCaracteres++;
            }
        }

        if(cantCaracteres != 1){
            esValido = false;
        }

        return esValido;
    }
    void serverLog(string mensaje){
        fstream file;
        file.open("server.log",ios::app | ios::out);
        if(file.fail()){
            cout<<"Error al acceder al archivo server.log";
        }
        file << getCurrentTime() << ": "<< mensaje.c_str() << endl;
    }
    string getCurrentTime(){

        string fechaHora = "";
        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);
        std::ostringstream oss;

        oss << std::put_time(&tm, "%d-%m-%Y_%H:%M");
        auto str = oss.str();
        return str;



    }
    void RealizarCalculo(string calculo){
        char operacion;
        string mensajeAux = "";
        int i,num1,num2,posOperacion,totCaracteres=0;

        if(!validarCalculo(calculo)){
            mensajeAux = "No se pudo realizar la operacion se encontro un caracter no contemplado";
        }

        //Tipo de Operacion
        for(i=1;calculo[i]!='\0';i++){
            if(calculo[i] == '+' || calculo[i] == '-' || calculo[i] == '/' || calculo[i] == '*' || calculo[i] == '!' || calculo[i] == '^'){
               operacion = calculo[i];
               posOperacion = i;
            }
        }
        totCaracteres = i - 1 ;
        cout<<totCaracteres<<endl;
        //Primera Operador
        num1 = convertirAEntero(calculo.substr(1,calculo.find(operacion)-1));
        //Segundo Operador
        num2 = convertirAEntero(calculo.substr(calculo.find(operacion) + 1,totCaracteres - 1));
        switch(operacion){
            case '+': mensajeAux = to_string(num1+num2);

            break;
            case '-': mensajeAux = to_string(num1-num2);

            break;
            case '*': mensajeAux = to_string(num1*num2);

            break;
            case '/': mensajeAux = to_string(num1/num2);

            break;
            case '!': mensajeAux = to_string(factorial(num1));

            break;
            case '^': mensajeAux = to_string((int)pow(num1,num2));

            break;
        }
        for(int i=0;i<(int)strlen(mensajeAux.c_str());i++){
            this->mensaje[i] = mensajeAux[i];
        }
        Enviar();
    }

    void CerrarSocket()
    {
        closesocket(client);
        cout << "Socket cerrado, cliente desconectado." << endl;
    }
};


int main()
{
  Server *Servidor = new Server();
  while(true)
  {
     Servidor->Recibir();
     //Servidor->Enviar();
  }
}
