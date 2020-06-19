build:
	g++ -g -Wall src/main.cpp -o hazmat

example:
	./hazmat 0.3 Instances/peligro-mezcla4-min-riesgo-zona7-2k-AE.2.hazmat
clean:
	rm *.o
debug:
	g++ -g -Wall src/main.cpp
