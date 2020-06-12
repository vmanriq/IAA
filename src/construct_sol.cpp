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
    //se ordenan los nodos de menor a mayor riesgo  
    vector<int> nodos_idx = sortIndex(inst);
    //se elimina el indice del depot
    nodos_idx.erase(remove(nodos_idx.begin(), nodos_idx.end(), 0),nodos_idx.end());
    
    //si es que no todos los nodos han sido asignados, excepto el depot
    while(nodos_idx.size() != 0 ){
        //si es qu eno hay camiones 
        if(cam.size() == 0){
            cout << "SOLUCION NO FACTIBLE" << endl;
        } 
        //se  toma un camion 
        aux_cam = cam.back();
        //se elimina de la lista de camiones en el "depot"
        cam.pop_back();

        //se le asigna el nodo mas con mayor peligro 
        visitarNodo(&aux_cam,inst.nodos[nodos_idx.back()]);
        //se visita este nodo 
        nodos_idx.pop_back();
        //mientras la ruta de este camion sea factible
        while (true)
        {
            //se busca el nodo que otorge menos a la funcion objetivo 
            idx_nodo = getBestNode(aux_cam ,aux_cam.ruta.back(), nodos_idx, inst);
            //no existe nodo compatible, es necesario generar otra ruta 
            if(idx_nodo == -1){
                //se devuelve al depot
                visitarNodo(&aux_cam,inst.nodos[0]);
                sol_cam.push_back(aux_cam);
                break;
            }
            //si es factible se visita el nodo seleccionado 
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