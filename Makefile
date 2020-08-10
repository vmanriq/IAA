build:
	g++  -Wall src/main.cpp -o hazmat

example:
	./hazmat 1 Instances/peligro-mezcla4-min-riesgo-zona2-2k-AE.2.hazmat 42 15 10 300 0.90 2000 15 
clean:
	rm *.o
debug:
	g++ -g -Wall src/main.cpp

test_all:
	for zone in 1 2 3 4 5 6 7; do \
		for alpha in 0.0 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 1; do \
			./hazmat $$alpha Instances/peligro-mezcla4-min-riesgo-zona$$zone-2k-AE.2.hazmat 42 15 10 300 0.95 5000 15;\
		done ;\
	done
