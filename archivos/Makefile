MCC := mpicc
RUN := mpirun
MDIR := mkdir
TOR := toroide/
CUB := hipercubo/
N := 64

setup: toroide hipercubo

toroide:
	$(MDIR) -p $(TOR)
	mv toroide.c $(TOR)
	$(MCC) $(TOR)toroide.c -o $(TOR)toroide -lm
	
hipercubo:
	$(MDIR) -p $(CUB)
	mv hipercubo.c $(CUB)
	$(MCC) $(CUB)hipercubo.c -o $(CUB)hipercubo -lm

exe_cubo:
	$(RUN) -n $(N) ./$(CUB)hipercubo
	
exe_toro:
	$(RUN) -n $(N) ./$(TOR)toroide
	
limpiar:
	rm -r $(TOR)
	rm -r $(CUB)
	rm datos.dat
	