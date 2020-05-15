#include "structs.cpp"
#include "readFile.cpp"



void evalSol(vector<camion> camiones, solucion *sol, instancia inst){
    float distancia = 0;
    float riesgo = 0;
    int max_riesgo = 0;
    //se recorren los camiones
    for(auto cam = camiones.begin() ; cam != camiones.end(); cam++){
        //se toma la distancia del depot
        distancia+= inst.distancia_depot[cam->ruta[0]];
        max_riesgo = inst.nodos[cam->ruta[0]].tipo_material;
        for(int i = 0 ; i < (int)cam->ruta.size()-1 ; i++){
            distancia+=inst.distancias[max_riesgo-1][cam->ruta[i]][cam->ruta[i+1]];
            riesgo+=inst.riesgos[max_riesgo-1][cam->ruta[i]][cam->ruta[i+1]];
            //se actualiza el riesgo 
            if(inst.nodos[cam->ruta[i]].tipo_material < inst.nodos[cam->ruta[i+1]].tipo_material){
                max_riesgo = inst.nodos[cam->ruta[i+1]].tipo_material;
            }
        }
    }
    sol->fitness_camino = distancia;
    sol->fitness_riesgo = riesgo;
    sol->camiones = camiones;
}


//se ordenan por tipo de riesgo del material 
vector<int> sortIndex(instancia inst){
    vector<int> idx(inst.nodos.size());
    iota(idx.begin(), idx.end(), 0);
    vector<nodo> nodos = inst.nodos;
    stable_sort(idx.begin(), idx.end(), [nodos](int i1, int i2){return nodos[i1].tipo_material>nodos[i2].tipo_material;}); 
    return idx;
}

void visitarNodo(camion * cam, nodo nod){
    cam->ruta.push_back(nod.idx);
    cam->capacidad_restante-=nod.cantidad_material;
    //si es el depot
    if(nod.idx == 0){
        return;
    }
    //si es que no llevaba un material igual se agrega 
    if(cam->materiales_cargados[nod.tipo_material-1] != 1){
        cam->materiales_cargados[nod.tipo_material-1] = 1;
    }
    //se actualiza el riesgo del camion 
    if(cam->riesgo_max < nod.tipo_material){
        cam->riesgo_max = nod.tipo_material;
    }
}


/*chekea compatbilidad, tanto de carga como de elementos, y devuelve indice del camion compatible
Params:    
    vector<camion>: vector con camiones 
    nodo nod: nodo a visitar
    instancia inst: instancia del problema 
Return:
    int idx: indice del camion compatible para visitar al nodo, -1 de no encontrar ningun camio compatible 
*/
bool checkCompatibility(camion cam,nodo nod ,instancia inst){
    int idx = -1;
    int material = nod.tipo_material;
    idx++;
    //se chequea si la capacidad es compatible
    if(cam.capacidad_restante-nod.cantidad_material < 0){
        return false;
    }
    //se recorren la compatiblidad de los elementos
    for(int j = 0; j < (int)cam.materiales_cargados.size();j++ ){
        if(cam.materiales_cargados[j] == 1 &&inst.incompatibilidad[material-1][j] == 1){
            return false;
        }
    }
    //si poasa todo es compatible 
    return true;
}

int getBestNode(camion cam, int idx_node, vector<int> nodos_idx, instancia inst){
    int best_n = -1;
    float best_fitness = numeric_limits<float>::max(), alpha = inst.alpha; 
    float act_fitness;
    //recorro los nodos disponibles
    for(auto i = nodos_idx.begin(); i != nodos_idx.end();i++ ){
        //si no son compatibles, por peso o por conflicto no se toma en cuenta 
        if(!checkCompatibility(cam, inst.nodos[*i], inst)){
            continue;
        }
        //se calcula la fitness hacia el nodo *i
        act_fitness = (alpha*inst.normRiesgos[cam.riesgo_max-1][*i][idx_node]+
                      (1-alpha)*inst.normDistancias[cam.riesgo_max-1][*i][idx_node]);
                      
                      
        if(act_fitness < best_fitness){
            best_fitness = act_fitness;
            best_n = *i;
        }
    }
    return best_n;
}

void bestInsertionH(instancia inst, vector <camion> camiones, solucion *sol){
    sol->fitness_riesgo = 0;
    sol->fitness_camino = 0;
    sol->fitness_pond = 0;
    vector<nodo> nodos = inst.nodos;
    vector <camion> cam = camiones;
    vector <camion> sol_cam ;
    struct camion aux_cam;
    int idx_nodo;
    //se ordenan los nodos de menor a mayor distancia del depot 
    vector<int> nodos_idx =sortIndex(inst);
    //se elimina el indice del depot
    nodos_idx.erase(remove(nodos_idx.begin(), nodos_idx.end(), 0),nodos_idx.end());
    
    //si es que no todos los nodos han sido asignados, excepto el depot
    while(nodos_idx.size() != 0 ){
        //se inicializa un camion
        if(cam.size() == 0){
            cout << "SOLUCION NO FACTIBLE" << endl;
        } 
        aux_cam = cam.back();
        cam.pop_back();
        //sale del depot
        //se le asigna el nodo mas ---- al depot 
        visitarNodo(&aux_cam,inst.nodos[nodos_idx.back()]);
        nodos_idx.pop_back();
        //mientras la ruta de este camion sea factible
        while (true)
        {
            idx_nodo = getBestNode(aux_cam ,aux_cam.ruta.back(), nodos_idx, inst);
            //no existe nodo compatible, es necesario generar otra ruta 
            if(idx_nodo == -1){
                //se devuelve al depot
                visitarNodo(&aux_cam,inst.nodos[0]);
                sol_cam.push_back(aux_cam);
                break;
            }
            visitarNodo(&aux_cam,inst.nodos[idx_nodo]);
            //se elimina el nodo visitado de la lista de nodos
            nodos_idx.erase(remove(nodos_idx.begin(), nodos_idx.end(), idx_nodo),nodos_idx.end());
        }
        
         


        }
    evalSol(sol_cam, sol, inst);
     
    }
   

solucion initSol(instancia inst, vector<vector<int>> incompatibilidad){
    struct solucion sol;
    struct camion cam;
    vector<int> ruta;
    vector<camion> camiones;
    //se llena el vector con camiones 
    for(int i = 0; i < inst.cant_camiones;i++){
        cam.capacidad_restante = inst.capacidad_camiones[i];
        cam.riesgo_max = 0;
        cam.materiales_cargados = {0,0,0,0,0};  
        cam.ruta = ruta;
        camiones.push_back(cam);
    }
    sol.camiones = camiones;
    //------------------gredy-----------------------------------------------
    bestInsertionH(inst, camiones, &sol);
    return sol;
}

void displaySol(solucion sol, instancia inst){
    struct camion cam;
    cout << "Las rutas son: "<< endl;
    for(int i = 0; i < (int)sol.camiones.size();i++){
        cam = sol.camiones[i];
        cout<< "La ruta del camion " << i << " es : ";
        for(int j = 0 ; j < (int)cam.ruta.size(); j++){
            //cout << inst.nodos[cam.ruta[j]].id << " ";
            cout << cam.ruta[j] << " ";
        }
        cout << endl;
    }
    cout << "(Z1) Riesgo:  " << sol.fitness_riesgo<<endl;
    cout << "(Z2) Distancia:  " << sol.fitness_camino<<endl;

}

int main(int argc, char const *argv[])
{
    vector<vector<int>> incompatibilidad ={
                                    {0,1,0,0,1},
                                    {1,0,0,0,0},
                                    {0,0,0,1,0},
                                    {0,0,1,0,0},
                                    {1,0,0,0,0}};
    float alpha = atof(argv[1]);
    string fName = argv[2];
    cout << "Instancia: " <<fName<< endl;
    cout << "Alpha utilizado: " <<alpha<< endl;
    struct instancia inst = leer_instancia(fName, alpha);
    inst.incompatibilidad = incompatibilidad;
    inst.alpha = alpha;
    struct solucion sol = initSol(inst, incompatibilidad);
    displaySol(sol, inst);
    return 0;
}
