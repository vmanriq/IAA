#include "structs.cpp"
#include "readFile.cpp"
#include "SA.cpp"

/* 
function evalSol:
Funcion que evalua el fitnees de la solucion 
return void:


*/

void evalSol(vector<camion> camiones, solucion *sol, instancia inst)
{
    float distancia = 0;
    float riesgo = 0;
    int max_riesgo = 0;
    float pond_fit = 0;
    //se recorren los camiones
    for (auto cam = camiones.begin(); cam != camiones.end(); cam++)
    {
        //si es que  nunca salio del depot
        if (cam->ruta.size() == 0)
        {
            continue;
        }
        //se toma la distancia del depot
        distancia += inst.distancia_depot[cam->ruta[0]];
        max_riesgo = inst.nodos[cam->ruta[0]].tipo_material;
        for (int i = 1; i < (int)cam->ruta.size(); i++)
        {
            distancia += inst.distancias[max_riesgo - 1][cam->ruta[i - 1]][cam->ruta[i]];
            riesgo += inst.riesgos[max_riesgo - 1][cam->ruta[i - 1]][cam->ruta[i]];
            pond_fit += (inst.alpha) * inst.normRiesgos[max_riesgo - 1][cam->ruta[i - 1]][cam->ruta[i]] + (1 - inst.alpha) * inst.normDistancias[max_riesgo - 1][cam->ruta[i - 1]][cam->ruta[i]];
            //se actualiza el riesgo
            if (inst.nodos[cam->ruta[i - 1]].tipo_material < inst.nodos[cam->ruta[i]].tipo_material)
            {
                max_riesgo = inst.nodos[cam->ruta[i]].tipo_material;
            }
        }
    }
    sol->fitness_camino = distancia;
    sol->fitness_riesgo = riesgo;
    sol->fitness_pond = pond_fit;
    sol->camiones = camiones;
}

int getRandomNode(camion cam, int idx_node, vector<int> nodos_idx, instancia inst, int lenL)
{
    float act_fitness;
    int pos;
    vector<tuple<float, int>> RCL;
    float alpha = inst.alpha;
    //recorro los nodos disponibles
    for (auto i = nodos_idx.begin(); i != nodos_idx.end(); i++)
    {
        //si no son compatibles, por peso o por conflicto no se toma en cuenta
        if (!checkCompatibility(cam, inst.nodos[*i], inst))
        {
            continue;
        }
        //se calcula la fitness hacia el nodo *i
        if (cam.riesgo_max != 0)
        {
            act_fitness = (alpha * inst.normRiesgos[cam.riesgo_max - 1][*i][idx_node] +
                           (1 - alpha) * inst.normDistancias[cam.riesgo_max - 1][*i][idx_node]);
        }
        else
        {
            act_fitness = alpha * inst.distancia_depot_norm[*i];
        }
        RCL.push_back(make_tuple(act_fitness, *i));
    }
    //se ordena  segun fitness, ascendente
    if (RCL.empty())
    {
        return -1;
    }
    sort(RCL.begin(), RCL.end());
    //se selecciona un nodo aleatoria deacuerdo al largo de la lista
    pos = (int)((min(lenL, (int)RCL.size()) - 1) * drand48());
    return get<1>(RCL[pos]);
}

void bestInsertionH(instancia inst, solucion *sol, int lenL)
{
    sol->fitness_riesgo = 0;
    sol->fitness_camino = 0;
    sol->fitness_pond = 0;
    vector<nodo> nodos = inst.nodos;
    vector<camion> cam = sol->camiones;
    vector<camion> sol_cam;
    struct camion aux_cam;
    int idx_nodo;
    //se ordenan los nodos de menor a mayor riesgo
    vector<int> nodos_idx((int)inst.nodos.size());
    iota(nodos_idx.begin(), nodos_idx.end(), 0);
    //se elimina el indice del depot
    nodos_idx.erase(remove(nodos_idx.begin(), nodos_idx.end(), 0), nodos_idx.end());

    //si es que no todos los nodos han sido asignados, excepto el depot
    while (nodos_idx.size() != 0)
    {
        //si es qu eno hay camiones
        if (cam.size() == 0)
        {
            cout << "SOLUCION NO FACTIBLE" << endl;
        }
        //se  toma un camion
        aux_cam = cam.back();
        //se elimina de la lista de camiones en el "depot"
        cam.pop_back();

        //se le asigna un  nodo aleatorio segun la heuristica
        idx_nodo = getRandomNode(aux_cam, 0, nodos_idx, inst, lenL);
        //se visita este nodo
        visitarNodo(&aux_cam, inst.nodos[idx_nodo]);
        nodos_idx.erase(remove(nodos_idx.begin(), nodos_idx.end(), idx_nodo), nodos_idx.end());
        //mientras la ruta de este camion sea factible
        while (true)
        {
            //se busca el nodo que otorge menos a la funcion objetivo
            idx_nodo = getRandomNode(aux_cam, aux_cam.ruta.back(), nodos_idx, inst, lenL);
            //no existe nodo compatible, es necesario generar otra ruta
            if (idx_nodo == -1)
            {
                //se devuelve al depot
                visitarNodo(&aux_cam, inst.nodos[0]);
                sol_cam.push_back(aux_cam);
                break;
            }
            //si es factible se visita el nodo seleccionado
            visitarNodo(&aux_cam, inst.nodos[idx_nodo]);
            //se elimina el nodo visitado de la lista de nodos
            nodos_idx.erase(remove(nodos_idx.begin(), nodos_idx.end(), idx_nodo), nodos_idx.end());
        }
    }
    //se agregan los camiones vacios que no se utilizaron
    sol_cam.insert(sol_cam.end(), cam.begin(), cam.end());

    evalSol(sol_cam, sol, inst);
}

solucion initSol(instancia inst)
{
    solucion sol;
    struct camion cam;
    vector<camion> camiones;
    //se llena el vector con camiones
    for (int i = 0; i < inst.cant_camiones; i++)
    {
        cam.capacidad_restante = inst.capacidad_camiones[i];
        camiones.push_back(cam);
    }
    sol.camiones = camiones;
    return sol;
}

solucion GRASP(instancia inst, int lenList, int gStop, float initT, float alpha, int Mi, int coolDown)
{
    struct solucion sol;
    struct solucion best_sol;

    float best_fit = numeric_limits<float>::max();
    //main loop greedy
    for (int iter = 0; iter < gStop; iter++)
    {
        sol = initSol(inst);
        bestInsertionH(inst, &sol, lenList);
        SAA(inst, &sol, initT, alpha, Mi, coolDown);
        if (iter == 0 || sol.fitness_pond < best_fit)
        {
            best_sol = sol;
            best_fit = sol.fitness_pond;
            //cout << "UPDATE SOL" << endl;
        }
        // mejor sol ?
    }
    return best_sol;
}

void displaySol(solucion sol, instancia inst)
{
    struct camion cam;
    /*cout << "Las rutas son: " << endl;
    for (int i = 0; i < (int)sol.camiones.size(); i++)
    {
        cam = sol.camiones[i];
        cout << "La ruta del camion " << i << " es : ";
        for (int j = 0; j < (int)cam.ruta.size(); j++)
        {
            //cout << inst.nodos[cam.ruta[j]].id << " ";
            cout << cam.ruta[j] << " ";
        }
        cout << endl;
    }*/
    cout << "(Z1) Riesgo SA:  " << sol.fitness_riesgo << endl;
    cout << "(Z2) Distancia SA:  " << sol.fitness_camino << endl;
    cout << "Pond Fitness SA: " << sol.fitness_pond << endl;
}

/* 
argv
[alpha, instancia, seed, T, decrecimiento_T, iter,]
*/
int main(int argc, char const *argv[])
{
    vector<vector<int>> incompatibilidad = {
        {0, 1, 0, 0, 1},
        {1, 0, 0, 0, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 1, 0, 0},
        {1, 0, 0, 0, 0}};
    float alpha = atof(argv[1]);
    string fName = argv[2];
    srand(atof(argv[3]));
    cout << "\n------------------------------------------- \n ";
    cout << "Instancia: " << fName << endl;
    cout << "Alpha utilizado: " << alpha << endl;
    struct instancia inst = leer_instancia(fName, alpha);
    inst.incompatibilidad = incompatibilidad;
    inst.alpha = alpha;
    struct solucion sol = GRASP(inst, atoi(argv[4]), atoi(argv[5]), atof(argv[6]), atof(argv[7]), atoi(argv[8]), atoi(argv[9]));

    displaySol(sol, inst);
    return 0;
}
