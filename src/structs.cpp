#include <iostream>
#include <fstream>
#include <numeric>
#include <vector>
#include <sstream>
#include <algorithm>
#include <string>
#include <limits>
#include <tuple>
#include <math.h>

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
    camion(){
        riesgo_max = 0;
        materiales_cargados = {0,0,0,0,0};
    }
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
   //distancia depot norm
   vector<float> distancia_depot_norm;
   //distancias de cada elemento 
   vector<vector<vector <float>>> distancias;
   //norm distancias
   vector<vector<vector <float>>> normDistancias;
   //riesgo de cada elemento segun posicion
   vector<vector<vector <float>>> riesgos;
   //norm riesgos
   vector<vector<vector <float>>> normRiesgos;
   
   //matriz de incompatibilidad
   vector<vector<int>> incompatibilidad;
   //ponderador de objetivos
   float alpha;
};

struct solucion
{
    //calidad de la solucion
    float  fitness_riesgo, fitness_camino, fitness_pond;
    //flota de camiones con sus rutas 
    vector<camion> camiones;
};