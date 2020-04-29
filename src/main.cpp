#include "structs.cpp"
#include "readFile.cpp"

/*chekea compatbilidad, tanto de carga como de elementos, y devuelve indice del camion compatible
Params:    
    vector<camion>: vector con camiones 
    nodo nod: nodo a visitar
    instancia inst: instancia del problema 
Return:
    int idx: indice del camion compatible para visitar al nodo, -1 de no encontrar ningun camio compatible 
*/
int checkCompatibility(vector<camion> camiones,nodo nod ,instancia inst){
    int idx = -1;
    int material = nod.tipo_material;
    bool validity = false;
    for(auto cam = camiones.begin() ; cam != camiones.end();cam++){
        idx++;
        //se chequea si la capacidad es compatible
        if(cam->capacidad_restante-nod.cantidad_material < 0){
            continue;
        }
        //se recorren la compatiblidad de los elementos
        for(int j = 0; j < cam->materiales_cargados.size();j++ ){
            if(cam->materiales_cargados[j] == 1 &&inst.incompatibilidad[material][j] == 1){
                break;
            }
            //si es la ultima iteracion y paso todos los casos
            if(j == cam->materiales_cargados.size()-1){
                validity = true;
            }
        }
        if(validity){
            return idx;
        }
    }
    return -1;
}

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
    //vector con los nodos no vistidos
    vector<nodo> nodos = inst.nodos;
    struct nodo auxnodo;
    //se le suma uno ya que el arreglo no toma encuenta al depot 
    int idx = (min_element(inst.distancia_depot.begin(),inst.distancia_depot.end())-inst.distancia_depot.begin()) +1;
    auxnodo = nodos[idx];
    //se toma coomo nodo visitado
    nodos.erase(nodos.begin()+idx);

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
    struct instancia inst = leer_instancia("Instances/peligro-mezcla4-min-riesgo-zona7-2k-AE.2.hazmat");
    inst.incompatibilidad = incompatibilidad;
    inst.alpha = alpha;
    struct solucion sol = initSol(inst, incompatibilidad);
    return 0;
}
