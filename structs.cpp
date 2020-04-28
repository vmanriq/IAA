#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>



using namespace std;


struct  nodo
{
    int id;
    int tipo_material;
    int cantidad_material;
};

struct  instancia
{
   //numero de camiones
   int cant_camiones;
   //capacidad camiones
   vector<float> capacidad_camiones; 
   //listas de nodos del problema
   vector<nodo> nodos;
   //distancias camion vacio
   vector<float> distancia_depot;
   //distancias de cada elemento 
   vector<vector <float>> distancias_A;
   vector<vector <float>> distancias_B;
   vector<vector <float>> distancias_C;
   vector<vector <float>> distancias_D;
   vector<vector <float>> distancias_E;
   //riesgo de cada elemento segun posicion
   vector<vector <float>> riesgo_A;
   vector<vector <float>> riesgo_B;
   vector<vector <float>> riesgo_C;
   vector<vector <float>> riesgo_D;
   vector<vector <float>> riesgo_E;
};
