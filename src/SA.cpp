
#include "structs.cpp"

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


void regenerateRute(camion *cam, instancia inst){
    camion new_cam;
    //se visitan todos los nodos 
    for(auto n = cam->ruta.begin(); n != cam->ruta.end(); n++){
        visitarNodo(&new_cam, inst.nodos[*n]);
    }
    *cam = new_cam;
}


void solEvaluation(solucion *sol, instancia inst){
    float distancia_fit = 0;
    float riesgo_fit = 0;
    int idx_cam = 0, idx_nod;
    float riesgo;
    float pond_fit = 0;
    //recorre cada camion 
    for(auto cam = sol->camiones.begin(); cam != sol->camiones.end(); cam++){
        camion aux_cam;
        aux_cam.capacidad_restante = inst.capacidad_camiones[idx_cam]; 
        for(int pos_nod = 0;  pos_nod < cam->ruta.size(); pos_nod++){
            //inidice del nodo real 
            idx_nod = cam->ruta[pos_nod];
            //si es que es primer nodo que se visita 
            if((int)aux_cam.ruta.size() < 1){
                visitarNodo(&aux_cam, inst.nodos[idx_nod]);
                distancia_fit+= inst.distancia_depot[idx_nod];
            }
            else{
                visitarNodo(&aux_cam, inst.nodos[idx_nod]);
                riesgo = aux_cam.riesgo_max - 1;
                distancia_fit += inst.distancias[riesgo][aux_cam.ruta[pos_nod-1]][aux_cam.ruta[pos_nod]];
                riesgo_fit += inst.riesgos[riesgo][aux_cam.ruta[pos_nod-1]][aux_cam.ruta[pos_nod]];
                pond_fit += (inst.alpha)*inst.normRiesgos[riesgo][aux_cam.ruta[pos_nod-1]][aux_cam.ruta[pos_nod]]
                             + (1-inst.alpha)*inst.normDistancias[riesgo][aux_cam.ruta[pos_nod-1]][aux_cam.ruta[pos_nod]];
            }
        }
        idx_cam++;
    }
    sol->fitness_riesgo = riesgo_fit;
    sol->fitness_camino = distancia_fit;
    sol->fitness_pond = pond_fit;
}


/*Devuelve el indice de un vector que haya salido del depot  */
int random_cam(vector<camion> camiones){
    vector<int> idxs;
    for(int i = 0 ; i < camiones.size() ; i++){
        if(camiones[i].ruta.size() > 0){
            idxs.push_back(i);
        }
    }
    random_shuffle(idxs.begin(), idxs.end());
    return idxs.back();
}


/*
Extrae el nodo de una ruta y se lo asigna a otra, esto puede generar una nueva ruta si es que no todos los camiones fueron utilizados
se aceptan soluciones infactibles (?)
 */
void relocation(solucion *sol, instancia inst){
    solucion new_sol = *sol;
 
    //se elige un camion al azar  el cual hay salido del depot 
    int idx_cam1 = random_cam(sol->camiones);
    //se elige otro camion al azar, puede ser el mismo 
    vector<int> camiones_idx(inst.cant_camiones);
    iota(camiones_idx.begin(), camiones_idx.end(), 0);
    random_shuffle(camiones_idx.begin(), camiones_idx.end());
    int idx_cam2 = camiones_idx.back();
    camiones_idx.pop_back();
 
    //se elige la posicion del nodo a extraer
    int nodo_idx = (int)((sol->camiones[idx_cam1].ruta.size()-2)*drand48());
    //se guarda el valor del nodo 
    int nod_val = sol->camiones[idx_cam1].ruta[nodo_idx];
    //si es que el camion a insertar el nodo no ha salido del depot no habra problemas de factibilidad 
    if( sol->camiones[idx_cam2].ruta.empty() ) {
        //se elimina el nodo del primer camion 
        new_sol.camiones[idx_cam1].ruta.erase(new_sol.camiones[idx_cam1].ruta.begin()+nodo_idx);
        new_sol.camiones[idx_cam2].ruta.push_back(nod_val);
    }
    //se inserta en una ruta con mas nodos, ! puede exisitr la posibilidad de infactibilidad 
    else{
        //se ve la factibilidad de insertar el nodo en esa ruta, mientras no sea factible se elige otra ruta al azar 
        while(!checkCompatibility(new_sol.camiones[idx_cam2], inst.nodos[nod_val], inst)){
            idx_cam2 = camiones_idx.back();
            camiones_idx.pop_back();
        }
        //posicion donde insertar el nodo 
        int nodo_idx_insert = (int)((sol->camiones[idx_cam1].ruta.size()-2)*drand48());
        //ahora existen 2 opciones, que el camion elegido sea el mismo o otro 
        //si es el mismo camion deben ser diferentes posiciones 
        if(idx_cam2 == idx_cam1){
            while(nodo_idx == nodo_idx_insert){
                nodo_idx_insert = (int)((sol->camiones[idx_cam2].ruta.size()-2)*drand48());
            }
        }
        new_sol.camiones[idx_cam1].ruta.erase(new_sol.camiones[idx_cam1].ruta.begin()+nodo_idx);
        new_sol.camiones[idx_cam2].ruta.insert(new_sol.camiones[idx_cam2].ruta.begin()+nodo_idx_insert, nod_val);
        
    }
    //se regeneran las rutas
    for(auto cam = new_sol.camiones.begin(); cam!= new_sol.camiones.end(); cam++){
        regenerateRute(&(*cam), inst);
    }
    solEvaluation(&new_sol, inst);
}


/*
function search_n
Se  elige un movimiento aleatorio para crear un vecino de la solucion actual 
 */

//se intenta solo con un movimiento por el momento 
solucion search_n (solucion sol, instancia inst){
    solucion new_sol = sol;
    double randNe = drand48();
    //0.5 de elegir este movimiento 
    if(randNe <= 1){
        relocation(&new_sol, inst);
    }
 /*    else if (randNe < 0.66){
         exchange(&new_sol, inst);
     }
    else {
         opt_2(&new_sol, inst);
     }*/

    return new_sol;
}
/*
instancia: instancia del problema 
sol: puntero a solucion completa
init_T: temperatura inicial 
alpha: facto de decrecimiento de la temperatura 
Mp: cantiadad de pasos (loop exterior)
Mi: cantidad de iteraciones. (loop interior)

 */

void SAA (instancia inst, solucion *sol, float init_T, float alpha, int Mp, int Mi){
    float T = init_T;
    double randN;
    solucion act = *sol, best = *sol, sn;
    //iteraciones en la que la temperatura baja 
    for(int paso = 0; paso < Mp; paso++){
        //iteraciones en que la temperatura se mantiene constante 
        for(int iter = 0; iter < Mi; iter++){
            //se elige un numero aleatorio 
            randN = drand48();
            //se busca un vecino de la solucion actual y se asigna a sn  // solo soluciones factibles hasta ahora 
            sn = search_n(act, inst);
            //me quedo con la nueva solucion si es que es mejor 
            if(sn.fitness_pond < act.fitness_pond){
                act = sn ;
            }
            //se acpeta una peor solucion dependdiendo del numero aleatorio 
            else if (randN < exp((act.fitness_pond - sn.fitness_pond)/T)){
                act = sn ;
            }

            if(act.fitness_pond < best.fitness_pond){
                best = act ;
            }
        }
        T *= alpha;
    }

    sol = &best; 
}