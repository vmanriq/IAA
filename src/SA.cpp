




solucion search_n (solucion sol){
    
    solucion new_sol;

    return new_sol
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
            randN = nrand48();
            //se busca un vecino de la solucion actual y se asigna a sn 
            sn = search_n(act);
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