


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



void solEvaluation(solucion *sol, instancia inst){
    float distancia_fit = 0;
    float riesgo_fit = 0;
    int idx_cam = 0, idx_nod;
    float riesgo;
    int penalty = 0;
    //recorre cada camion 
    for(auto cam = sol->camiones.begin(); cam != sol->camiones.end(); cam++){
        camion aux_cam;
        aux_cam.capacidad_restante = inst.capacidad_camiones[idx_cam]; 
        for(int pos_nod = 0;  pos_nod < cam->ruta.size(); pos_nod++){
            idx_nod = cam->ruta[idx_nod];
            //si es que es primer nodo que se visita 
            if((int)aux_cam.ruta.size() < 1){
                visitarNodo(&aux_cam, inst.nodos[idx_nod]);
                distancia_fit+= inst.distancia_depot[idx_nod];
            }
            else{
                penalty = 0;
                //si es que el nodo es compatible no pasa nada :D
                if(!checkCompatibility(aux_cam, inst.nodos[idx_nod], inst)){
                    penalty = 500000;    
                }
                visitarNodo(&aux_cam, inst.nodos[idx_nod]);
                riesgo = aux_cam.riesgo_max - 1;
                distancia_fit += inst.distancias[riesgo][aux_cam.ruta[pos_nod-1]][aux_cam.ruta[pos_nod]] + penalty;
                riesgo_fit += inst.riesgos[riesgo][aux_cam.ruta[pos_nod-1]][aux_cam.ruta[pos_nod]] + penalty;
            }
        }
        idx_cam++;
    }
    sol->fitness_riesgo = riesgo_fit;
    sol->fitness_camino = distancia_fit;
    //falta sumar riesgos ponderados 

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

void regenerateRute(camion *cam, instancia inst){
    camion new_cam;
    //se visitan todos los nodos 
    for(auto n = cam->ruta.begin(); n != cam->ruta.end(); n++){
        visitarNodo(&new_cam, inst.nodos[*n]);
    }
    *cam = new_cam;
}

/*
Extrae el nodo de una ruta y se lo asigna a otra, esto puede generar una nueva ruta si es que no todos los camiones fueron utilizados
se aceptan soluciones infactibles (?)
 */
void realocate(solucion *sol, instancia inst){
    solucion new_sol = *sol;
 
    //se elige un camion al azar  el cual hay salido del depot 
    int idx_cam1 = random_cam(sol->camiones);
    int idx_cam2 = (int)inst.cant_camiones*drand48();
    //si es que se eligio el mismo camion se cambia 
    while(idx_cam1 == idx_cam2){
        idx_cam2 = (int)inst.cant_camiones*drand48();
    }
    //se elige la posicion del nodo a extraer
    int nodo_idx = (int)((sol->camiones[idx_cam1].ruta.size()-1)*drand48());
    //si es que el camion ya tenia ruta se elige donde insertar 
    if(sol->camiones[idx_cam2].ruta.size() > 0){
        //se elige el indice en que se insertara el nodo 
        int random_idx = (int)((sol->camiones[idx_cam2].ruta.size()-1)*drand48());
        //se altera la lista de nodos visitados 
        new_sol.camiones[idx_cam2].ruta.insert(new_sol.camiones[idx_cam2].ruta.begin() + random_idx, sol->camiones[idx_cam1].ruta[nodo_idx]);
        //se  actualiza el camion con la nueva ruta  
        regenerateRute(&(new_sol.camiones[idx_cam2]), inst);
    }
    //si el camion no tenia ruta se crea 
    else{
        //visitar nodo izi 
        visitarNodo(&(new_sol.camiones[idx_cam2]), inst.nodos[nodo_idx]);
    }
    //se elimina el nodo del camion 1, actualizando su ruta
    new_sol.camiones[idx_cam1].ruta.erase(new_sol.camiones[idx_cam1].ruta.begin()+nodo_idx);
    regenerateRute(&(new_sol.camiones[idx_cam1]), inst);
    //se evalua la solucion;
    //!!!!!!!!!!!!!!
    *sol = new_sol;
}


/*
function search_n
Se  elige un movimiento aleatorio para crear un vecino de la solucion actual 
 */
solucion search_n (solucion sol, instancia inst){
    solucion new_sol = sol;
    double randNe = drand48();
    //0.5 de elegir este movimiento 
    if(randNe <= 1){
        realocate(&new_sol, inst);
    }
    // else if (randNe < 0.66){
    //     exchange(&new_sol, inst);
    // }
    // else {
    //     opt_2(&new_sol, inst);
    // }

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
    for(int paso = 0; paso < Mp; paso++){
        for(int iter = 0; iter < Mi; iter++){
            //se elige un numero aleatorio 
            randN = drand48();
            //se busca un vecino de la solucion actual y se asigna a sn 
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