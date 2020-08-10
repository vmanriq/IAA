# Proyecto IAA: Recoleccion de HAZMAT

## Para la ejecución
1.  Compilar por medio de: 
```bash
    make build
 ```
 2. Ejecutar por:
 ```bash
    ./hazmat {alpha} {instancia} {seed} {lenlist} {gstop} {T_init} {alpha_cooldown} {n_iter} {L}
 ```

 donde:
 + __alpha__: es la importancia que se le dara al objetivo Z1, Z2 por defecto se le dara (1-alpha)
 + __instancia__: es la ruta donde se encuentra la instancia a probar.
 + __seed__: numero de la seed utilizada para la generacion de numeros aleatorios 
 + __lenlist__: largo lista utilizada por randomized greedy  
 + __gstop__: numero de iteraciones de GRASP 
 + __T_init__: temperatura inicial del algoritmo SA 
 + __alpha_cooldown__: Factor de enfriamiento de la temperatura 
 + __n_iter___ numero de iteraciones del algoritmo SA
 + __L__: Cada cuantas iteraciones la temperatura se actualizara 
 

 __Cantidad de iteraciones esta dado por **gstop** **n_iter**__ 