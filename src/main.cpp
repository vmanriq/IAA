#include "structs.cpp"
#include "readFile.cpp"




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
    sol->fitness_Total = 0;
    sol->fitness_riesgo = 0;
    sol->fitness_camino = 0;
    vector<nodo> nodos = inst.nodos;
    vector <camion> cam = camiones;
    struct camion aux_cam;
    float best_riesgo ,best_dist, best_obj = 10000000;
    float obj_act;
    int best_idx;
    //recorre nodos se salta el primero por que es el depot
    for(int i = 1; i < (int)nodos.size(); i++){
        best_obj = 10000000;
        //Recorre camiones
        for(int j = 0; j < (int)cam.size(); j++){
            aux_cam = cam[j];
            //si es que no es compatible no se considera 
            if(!checkCompatibility(aux_cam, nodos[i], inst)){
                continue;
            }
            obj_act = 0;
            //no ha salido del depot
            if (aux_cam.riesgo_max == 0){
                obj_act = (1-inst.alpha)*inst.distancia_depot[i];
            }
            else if (aux_cam.riesgo_max != 0){
                obj_act = (inst.alpha)*inst.riesgos[aux_cam.riesgo_max-1][aux_cam.ruta.back()][i]+
                          (1-inst.alpha)*inst.distancias[aux_cam.riesgo_max-1][aux_cam.ruta.back()][i];
            }
            //si el camion lo hace mejor 
            if(obj_act < best_obj){
                best_obj = obj_act;
                best_idx = j;
                if(aux_cam.riesgo_max == 0){ 
                    best_dist = best_obj;
                }
                else{
                    best_riesgo  = (inst.alpha)*inst.riesgos[aux_cam.riesgo_max-1][aux_cam.ruta.back()][i];
                    best_dist  = (1-inst.alpha)*inst.distancias[aux_cam.riesgo_max-1][aux_cam.ruta.back()][i];
                }
            }
        }
        //EL CAMION VISITA EL NODO
        visitarNodo(&cam[best_idx], nodos[i]);
        sol->fitness_camino += best_dist;
        sol->fitness_riesgo += best_riesgo;
        sol->fitness_Total += best_obj;
    }
    //se vuelve al depot
    for(int j = 0; j < (int)cam.size(); j++){
        aux_cam = cam[j];
        // nunca salio del depot
        if(aux_cam.ruta.size() == 0){
            continue;
        }
        else{
            sol->fitness_camino += (1-inst.alpha)*inst.distancias[aux_cam.riesgo_max-1][aux_cam.ruta.back()][0];
            sol->fitness_riesgo += (inst.alpha)*inst.riesgos[aux_cam.riesgo_max-1][aux_cam.ruta.back()][0];
            sol->fitness_Total += (1-inst.alpha)*inst.distancias[aux_cam.riesgo_max-1][aux_cam.ruta.back()][0]+
                                  (inst.alpha)*inst.riesgos[aux_cam.riesgo_max-1][aux_cam.ruta.back()][0];
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

int main(int argc, char const *argv[])
{
    float alpha = 0.5;
    //se genera matriz de compatibilidad 0 = compatible 
    vector<vector<int>> incompatibilidad ={
                            {0,1,0,0,1},
                            {1,0,0,0,0},
                            {0,0,0,1,0},
                            {0,0,1,0,0},
                            {1,0,0,0,0}};
    //orden de dominancia de mayor a menor 
    struct instancia inst = leer_instancia("Instances/peligro-mezcla4-min-riesgo-zona7-2k-AE.2.hazmat", alpha);
    inst.incompatibilidad = incompatibilidad;
    inst.alpha = alpha;
    struct solucion sol = initSol(inst, incompatibilidad);
    cout << "Zona 7 " << endl;
    cout << "alpha " << inst.alpha << endl; 
    cout << "Z1 " << sol.fitness_riesgo << endl;
    cout << "Z2 " << sol.fitness_camino << endl;
    cout << "Total " << sol.fitness_Total << endl;

    return 0;
}
