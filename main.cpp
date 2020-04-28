#include "structs.cpp"


//realiza split sobre string segun delimiter y devuelve un vector con los elementos int 
vector<float> stringToVector(string input, char delimiter){
    vector<float> splitStr;
    stringstream ss(input);
    string token;
    while(getline(ss, token, delimiter)){
        if(token.compare("A")){
            splitStr.push_back(1);
        }
        else if(token.compare("B")){
            splitStr.push_back(2);
        }
        else if(token.compare("C")){
            splitStr.push_back(3);
        }
        else if(token.compare("D")){
            splitStr.push_back(4);
        }
        else if(token.compare("E")){
            splitStr.push_back(5);
        }
        else if(token.compare("-")){
            splitStr.push_back(0);
        }
        else{
            splitStr.push_back(strtof(token.c_str, NULL));
        }
    }

    return splitStr;
}


instancia leer_instancia(string nombre_instancia){

    //se crea nuevo elemento de instancia 
    struct instancia inst;
    string line;
    int dummy_n;
    //se lee el archivo 
    ifstream infile(nombre_instancia);
    infile>>inst.cant_camiones;
    getline(infile, line);

    return inst;

}


int main(int argc, char const *argv[])
{
    // if(argc == 0 ){
    //     cout << "Faltan argumentos" << endl;
    //     return 1;
    // }
    /* code */
    //float alpha = stof(argv[1]);
    struct instancia inst = leer_instancia("Instances/peligro-mezcla4-min-riesgo-zona1-2k-AE.2.hazmat");
    return 0;
}
