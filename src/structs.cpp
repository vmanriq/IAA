#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <string>
using namespace std;

struct  nodo
{
    int id;
    int idx;
    int tipo_material;
    int cantidad_material;
    bool visitado;
};

struct camion
{
    vector<int> ruta;
    float capacidad_restante;
    int riesgo_max;
    vector<int> materiales_cargados;
};



struct  instancia
{
   //numero de camiones
   int cant_camiones;
   //capacidad camiones
   vector<float> capacidad_camiones; 
   //cantidad de nodos
   int cant_nodos;
   //listas de nodos del problema
   vector<nodo> nodos;
   //distancias camion vacio
   vector<float> distancia_depot;
   //distancias de cada elemento 
   vector<vector<vector <float>>> distancias;
   //riesgo de cada elemento segun posicion
   vector<vector<vector <float>>> riesgos;
   //----------------ponderado de cada elemento
 //  vector<vector<vector <float>>> ponderados;
   
   //matriz de incompatibilidad
   vector<vector<int>> incompatibilidad;
   //ponderador de objetivos
   float alpha;
};

struct solucion
{
    //calidad de la solucion
    float fitness_Total, fitness_riesgo, fitness_camino;
    //flota de camiones con sus rutas 
    vector<camion> camiones;
};