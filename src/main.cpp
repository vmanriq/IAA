#include "structs.cpp"
#include "readFile.cpp"



solucion initSol(instancia inst, vector<vector<int>> incompatibilidad){
    struct solucion sol;
    struct camion cam;
    vector<int> ruta;
    vector<camion> camiones;
    //se
    for(int i = 0; i < inst.cant_camiones;i++){
        cam.capacidad_restante = inst.capacidad_camiones[i];
        cam.riesgo_max = 0;
        cam.materiales_cargados = {0,0,0,0,0};  
        cam.ruta = ruta;
        camiones.push_back(cam);
    }
    sol.camiones = camiones;
    //gredy
    vector<int> fitness_depot;
    for(int i = 0; i< inst.)

    return sol;
}




int main(int argc, char const *argv[])
{
    float alpha = 0.1;
    //se genera matriz de compatibilidad 0 = compatible 
    vector<vector<int>> incompatibilidad ={
                            {0,1,0,0,1},
                            {1,0,0,0,0},
                            {0,0,0,1,0},
                            {0,0,1,0,0},
                            {1,0,0,0,0}};
    //orden de dominancia de mayor a menor 
    int dom[] ={5,4,3,2,1};
    struct instancia inst = leer_instancia("Instances/peligro-mezcla4-min-riesgo-zona6-2k-AE.2.hazmat");
    struct solucion sol = initSol(inst, incompatibilidad);
    return 0;
}
