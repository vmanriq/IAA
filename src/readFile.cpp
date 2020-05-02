
void normMatrix(vector<vector<vector<float>>> matrices){
    
}

vector<vector <float>> pondSum(vector<vector <float>> dist, vector<vector <float>> riesgo, float alpha, int n_nodos){
    vector<vector <float>> resultado;
    vector <float> fila;
    for(int i = 0; i < n_nodos; i++){
        for(int j = 0; j < n_nodos ; j++){
         fila.push_back((alpha)*riesgo[i][j]+(1- alpha)*dist[i][j]);   
        }
        resultado.push_back(fila);
        fila.clear();
    }
    return resultado;
}

//realiza split sobre string segun delimiter y devuelve un vector con los elementos int 
vector<float> stringToVector(string input, char delimiter){
    vector<float> splitStr;
    stringstream ss(input);
    string token;
    while(getline(ss, token, delimiter)){
        if(!token.compare("A")){
            splitStr.push_back(1);
        }
        else if(!token.compare("B")){
            splitStr.push_back(2);
        }
        else if(!token.compare("C")){
            splitStr.push_back(3);
        }
        else if(!token.compare("D")){
            splitStr.push_back(4);
        }
        else if(!token.compare("E")){
            splitStr.push_back(5);
        }
        else if(!token.compare("-")){
            splitStr.push_back(0);
        }
        else{
            splitStr.push_back(strtof(token.c_str(), NULL ));
        }
    }

    return splitStr;
}

vector<vector<float>> leerMatriz(int cantNodos, ifstream *file){
    vector<vector<float>> matriz;
    string fila;
    for(int i = 0 ; i < cantNodos ; i++){
        getline(*file, fila);
        matriz.push_back(stringToVector(fila, ' '));
    }
    return matriz;
}


instancia leer_instancia(string nombre_instancia, float alpha){

    //se crea nuevo elemento de instancia 
    struct instancia inst;
    struct nodo dummy_nodo;
    vector<float> dummy_vector;
    string line;
    int cant_nodos;
    //se lee el archivo 
    ifstream infile(nombre_instancia);
    //cant camiones
    getline(infile, line);
    inst.cant_camiones = stoi(line);
    //capacidad camiones
    getline(infile, line);
    inst.capacidad_camiones = stringToVector(line,' ');
    //cantidad nodos 
    getline(infile, line);
    cant_nodos = stoi(line);
    inst.cant_nodos = cant_nodos;
    //se rellenan los nodos
    vector<nodo> nodos;
    for(int i = 0; i < cant_nodos; i++){
        getline(infile, line);
        dummy_vector = stringToVector(line, ' ');
        dummy_nodo.id = dummy_vector[0];
        dummy_nodo.idx = i;
        dummy_nodo.cantidad_material = dummy_vector[1];
        dummy_nodo.tipo_material = dummy_vector[2];
        dummy_nodo.visitado = false;
        nodos.push_back(dummy_nodo);
    }
    inst.nodos = nodos;
    //------------DISTANCIAS -----------
    //distancia depot 
    getline(infile, line);
    dummy_vector = stringToVector(line, ' ');
    //dummy_vector.erase(dummy_vector.begin());
    inst.distancia_depot = dummy_vector;
    //distancia matrices
    inst.distancias.push_back(leerMatriz(cant_nodos, &infile));
    inst.distancias.push_back(leerMatriz(cant_nodos, &infile));
    inst.distancias.push_back(leerMatriz(cant_nodos, &infile));
    inst.distancias.push_back(leerMatriz(cant_nodos, &infile));
    inst.distancias.push_back(leerMatriz(cant_nodos, &infile));
    //riesgo matrices
    inst.riesgos.push_back(leerMatriz(cant_nodos, &infile));
    inst.riesgos.push_back(leerMatriz(cant_nodos, &infile));
    inst.riesgos.push_back(leerMatriz(cant_nodos, &infile));
    inst.riesgos.push_back(leerMatriz(cant_nodos, &infile));
    inst.riesgos.push_back(leerMatriz(cant_nodos, &infile));
    //suma ponderada de las mnatrices
    // inst.ponderados.push_back(pondSum(distancias_A, riesgo_A, alpha, cant_nodos));
    // inst.ponderados.push_back(pondSum(distancias_B, riesgo_B, alpha, cant_nodos));
    // inst.ponderados.push_back(pondSum(distancias_C, riesgo_C, alpha, cant_nodos));
    // inst.ponderados.push_back(pondSum(distancias_D, riesgo_D, alpha, cant_nodos));
    // inst.ponderados.push_back(pondSum(distancias_E, riesgo_E, alpha, cant_nodos));
    return inst;

}