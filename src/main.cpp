#include "structs.cpp"
#include "readFile.cpp"


vector<int> sortIndex(instancia inst){
    int idx_a = -1, idx_b = -1, idx_c = -1, idx_d = -1;
    int id_aux;
    vector<int> idx(inst.nodos.size());
    iota(idx.begin(), idx.end(), 0);
    vector<nodo> nodos = inst.nodos;
    stable_sort(idx.begin(), idx.end(), [nodos](int i1, int i2){return nodos[i1].tipo_material<nodos[i2].tipo_material;});
    for(int i = 0 ; i<(int)idx.size();i++){
        id_aux = idx[i];
        if(inst.nodos[id_aux].tipo_material == 1 && idx_a == -1){
            idx_a = id_aux;
        }
        if(inst.nodos[id_aux].tipo_material == 2 && idx_b == -1){
            idx_b = id_aux;
        }
        if(inst.nodos[id_aux].tipo_material == 3 && idx_c == -1){
            idx_c = id_aux;
        }
        if(inst.nodos[id_aux].tipo_material == 4 && idx_d == -1){
            idx_d = id_aux;
        }
        if(idx_a != -1 && idx_b != -1&& idx_c != -1&& idx_d != -1){
            break;
        }
    }
    //Sse fuerza a iniciar por nodos que tengan elementos A-B-C asi siempre se tienen soluciones factibles con respecto a los conflictos
    //minimo de camiones necesarios para que no haya conflictos es 2 
    swap(idx[0],idx[distance(idx.begin(),find(idx.begin(), idx.end(), idx_a))]);
    swap(idx[1],idx[distance(idx.begin(),find(idx.begin(), idx.end(), idx_b))]);
    swap(idx[2],idx[distance(idx.begin(),find(idx.begin(), idx.end(), idx_c))]);
    swap(idx[3],idx[distance(idx.begin(),find(idx.begin(), idx.end(), idx_d))]);
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

void bestInsertionH(instancia inst, vector <camion> camiones, solucion *sol){
    sol->fitness_riesgo = 0;
    sol->fitness_camino = 0;
    sol->fitness_pond = 0;
    vector<nodo> nodos = inst.nodos;
    vector <camion> cam = camiones;
    struct camion aux_cam;
    //se ordenan los nodos de menor a mayor distancia del depot 
    vector<int> nodos_idx =sortIndex(inst);
    float best_riesgo ,best_dist, best_obj = numeric_limits<float>::max();
    float dist_act, riesgo_act, obj_act;
    int best_idx, i;
    //recorre nodos se salta el primero por que es el depot
    for(auto idx = nodos_idx.begin(); idx != nodos_idx.end(); idx++){
        i = *idx;
        //si es que es el depot 
        if( i == 0){
            continue;
        }
        best_obj = numeric_limits<float>::max();
        best_idx = -1;
   //     cout << "---------------nodo " << i <<"--------------------------------" <<endl;
        //Recorre camiones
        for(int j = 0; j < (int)cam.size(); j++){
            //se toma el camion j 
            aux_cam = cam[j];
            riesgo_act = 0;
            dist_act = 0;
            
            //si es que no es compatible no se considera 
            if(!checkCompatibility(aux_cam, nodos[i], inst)){
                continue;
            }
            //no ha salido del depot
            if (aux_cam.riesgo_max == 0){
                //distancia del depot al nodo i 
                obj_act = (1-inst.alpha)*inst.distancia_depot_norm[i];
                dist_act = inst.distancia_depot[i];
            }
            else if (aux_cam.riesgo_max != 0){
                //se utiliza la matriz de riesgo correspondiente al maximo resigo del camion, luego se donde se encuentra actualemnte y adonde quiere ir
                obj_act = (inst.alpha)*inst.normRiesgos[aux_cam.riesgo_max-1][aux_cam.ruta.back()][i]+
                          (1-inst.alpha)*inst.normDistancias[aux_cam.riesgo_max-1][aux_cam.ruta.back()][i] - (1- aux_cam.capacidad_restante/40000)*0.25 ;
                riesgo_act = inst.riesgos[aux_cam.riesgo_max-1][aux_cam.ruta.back()][i];
                dist_act = inst.distancias[aux_cam.riesgo_max-1][aux_cam.ruta.back()][i];
            }
            //si otorga menos a la funcion objetivo 
            if(obj_act < best_obj){
                best_idx = j;
                best_obj = obj_act;
                best_dist = dist_act;
                best_riesgo = riesgo_act;
            }
      //      cout << "Mejor riesto hasta ahora " << best_riesgo << endl;
        }
        if(best_idx == -1 ){
            cout << " NODO NO ASIGNADO !!!!!!!!!!!!!!!!!!! "<< i <<endl;
        }
        //EL CAMION VISITA EL NODO
        visitarNodo(&cam[best_idx], nodos[i]);
        sol->fitness_camino += best_dist;
        sol->fitness_riesgo += best_riesgo;
        sol->fitness_pond+= best_obj;
    }
    //se recorren los camiones para que vuelvan al depot
    for(int j = 0; j < (int)cam.size(); j++){
        aux_cam = cam[j];
        // nunca salio del depot
        if(aux_cam.ruta.size() == 0){
            continue;
        }
        else{
            sol->fitness_camino += inst.distancias[aux_cam.riesgo_max-1][aux_cam.ruta.back()][0];
            sol->fitness_riesgo += inst.riesgos[aux_cam.riesgo_max-1][aux_cam.ruta.back()][0];
            visitarNodo(&cam[j], nodos[0]);
        }

    }
    sol->camiones = cam;
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
    // cout << "Las rutas son: "<< endl;
    // for(int i = 0; i < (int)sol.camiones.size();i++){
    //     cam = sol.camiones[i];
    //     cout<< "La ruta del camion " << i << " es : ";
    //     for(int j = 0 ; j < (int)cam.ruta.size(); j++){
    //         //cout << inst.nodos[cam.ruta[j]].id << " ";
    //         cout << cam.ruta[j] << " ";
    //     }
    //     cout << endl;
    // }
    
    printf(" %d;%d \n", (int)sol.fitness_riesgo,(int)sol.fitness_camino);
    //printf("Distancia total %d \n", (int)sol.fitness_camino);
}

int main(int argc, char const *argv[])
{
    //float alpha = atof(argv[1]);
    for(int i=1 ; i <8 ; i++){
        string fName = "Instances/peligro-mezcla4-min-riesgo-zona"+to_string(i)+"-2k-AE.2.hazmat";
        cout << fName<< endl;
        for(float alpha = 0.0 ; alpha < 1.1; alpha+=0.1){    
            //string fName = argv[2];
            //cout << "El alpha usado es " << alpha << endl;
            //se genera matriz de compatibilidad 0 = compatible 
            vector<vector<int>> incompatibilidad ={
                                    {0,1,0,0,1},
                                    {1,0,0,0,0},
                                    {0,0,0,1,0},
                                    {0,0,1,0,0},
                                    {1,0,0,0,0}};
            //orden de dominancia de mayor a menor 
            struct instancia inst = leer_instancia(fName, alpha);
            inst.incompatibilidad = incompatibilidad;
            inst.alpha = alpha;
            struct solucion sol = initSol(inst, incompatibilidad);
            displaySol(sol, inst);
            //cout << "----------------------------" << endl;
        }
        cout << "----------------------------" << endl;
    }
    return 0;
}
