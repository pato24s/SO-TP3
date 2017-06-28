.PHONY : all clean run new

MPIROOT = /usr

CFLAGS+= -Wall -g $(INCL)
CC=gcc
MPICC=  $(MPIROOT)/bin/mpic++
INCL= -I$(MPIROOT)/include
SRCS= consola.cpp main.cpp nodo.cpp HashMap.cpp
BIN= dist_hashmap

all: dist_hashmap

$(BIN): $(SRCS)
	$(MPICC) $(CFLAGS) -o $(BIN) $(SRCS)

clean:
	rm -f $(BIN)

new: clean all

# ejecuta todos los tests
test-all: test-load test-addAndInc test-member test-maximum

# ejecuta todos los tests para la funcion load
test-load: test-load-1 test-load-2 test-load-3

# ejecuta todos los tests para la funcion addAndInc
test-addAndInc: test-addAndInc-1 test-addAndInc-2 test-addAndInc-3

# ejecuta todos los tests para la funcion member
test-member: test-member-1 test-member-2 test-member-3 test-member-4

# ejecuta todos los tests para la funcion maximum
test-maximum: test-maximum-1 test-maximum-2 test-maximum-3

# load con cantidad de archivos menor que la cantidad de nodos
# Ejecucion:
# mpiexec -np 4 ./dist_hashmap
# load corpus-0 corpus-1
# print
# q
test-load-1:
	########## test-load-1 ##########
	awk -f corpus.awk corpus | sort > corpus-post
	for i in 0 1; do sed -n "$$((i * 1250 + 1)),$$(((i + 1) * 1250))p" corpus > corpus-"$$i"; done
	printf " load corpus-0 corpus-1 \n print \n q \n" > input
	cat input | mpiexec -np 4 ./dist_hashmap | sed -e '1,/finalizado/d' | sed '/>/d' | sort | diff -u - corpus-post | awk '{ print } END { if (NR) print "Test FAILED"; else print "Test OK" }'
	rm -f input corpus-post corpus-[0-1]

# load con cantidad de archivos mayor que la cantidad de nodos
# Ejecucion:
# mpiexec -np 3 ./dist_hashmap
# load corpus-0 corpus-1 corpus-2 corpus-3 corpus-4
# print
# q
test-load-2:
	########## test-load-2 ##########
	awk -f corpus.awk corpus | sort > corpus-post
	for i in 0 1 2 3 4; do sed -n "$$((i * 500 + 1)),$$(((i + 1) * 500))p" corpus > corpus-"$$i"; done
	printf " load corpus-0 corpus-1 corpus-2 corpus-3 corpus-4 \n print \n q \n" > input	
	cat input | mpiexec -np 3 ./dist_hashmap | sed -e '1,/finalizado/d' | sed '/>/d' | sort | diff -u - corpus-post	| awk '{ print } END { if (NR) print "Test FAILED"; else print "Test OK" }'
	rm -f input corpus-post corpus-[0-4]

# load con cantidad de archivos igual que la cantidad de nodos
# Ejecucion:
# mpiexec -np 5 ./dist_hashmap
# load corpus-0 corpus-1 corpus-2 corpus-3
# print
# q
test-load-3:
	########## test-load-3 ##########
	awk -f corpus.awk corpus | sort > corpus-post
	for i in 0 1 2 3; do sed -n "$$((i * 625 + 1)),$$(((i + 1) * 625))p" corpus > corpus-"$$i"; done
	printf " load corpus-0 corpus-1 corpus-2 corpus-3 \n print \n q \n" > input	
	cat input | mpiexec -np 5 ./dist_hashmap | sed -e '1,/finalizado/d' | sed '/>/d' | sort | diff -u - corpus-post	| awk '{ print } END { if (NR) print "Test FAILED"; else print "Test OK" }'
	rm -f input corpus-post corpus-[0-3]

# addAndInc de "primerstring" sobre un hashmap vacio
# Ejecucion:
# mpiexec -np 3 ./dist_hashmap
# addAndInc primerstring
# print
# q
test-addAndInc-1:
	########## test-addAndInc-1 ##########	
	printf " addAndInc primerstring \n print \n q \n" > input		
	printf "primerstring 1\n" > output
	cat input | mpiexec -np 3 ./dist_hashmap | sed -e '1,/primerstring/d' | sed '/>/d' | diff -u - output | awk '{ print } END { if (NR) print "Test FAILED"; else print "Test OK" }'
	rm -f input output
	
# addAndInc de "nuevapalabrauno" y "nuevapalabrados" sobre un hashmap con corpus-0 y corpus-1 cargados
# Ejecucion:
# mpiexec -np 3 ./dist_hashmap
# load corpus-0 corpus-1
# addAndInc nuevapalabrauno
# addAndInc nuevapalabrados
# print
# q
test-addAndInc-2:
	########## test-addAndInc-2 ##########		
	printf " load corpus-0 corpus-1 \n addAndInc nuevapalabrauno \n addAndInc nuevapalabrados \n print \n q \n" > input
	cat corpus > corpus-aux && printf "\nnuevapalabrauno\nnuevapalabrados" >> corpus-aux
	awk -f corpus.awk corpus-aux | sort > corpus-post
	for i in 0 1; do sed -n "$$((i * 1250 + 1)),$$(((i + 1) * 1250))p" corpus > corpus-"$$i"; done	
	cat input | mpiexec -np 3 ./dist_hashmap | sed -e '1,/nuevapalabrados/d' | sed '/>/d' | sort | diff -u - corpus-post | awk '{ print } END { if (NR) print "Test FAILED"; else print "Test OK" }'
	rm -f input corpus-aux corpus-post corpus-[0-1]

# addAndInc de "archivo" "estabilidad" y "funcional" (todas ya definidas) sobre un hashmap con corpus cargado
# Ejecucion:
# mpiexec -np 3 ./dist_hashmap
# load corpus
# addAndInc archivo
# addAndInc estabilidad
# addAndInc funcional
# print
# q
test-addAndInc-3:
	########## test-addAndInc-3 ##########		
	printf " load corpus \n addAndInc archivo \n addAndInc estabilidad \n addAndInc funcional \n print \n q \n" > input	
	cat corpus > corpus-aux && echo "\narchivo\nestabilidad\nfuncional" >> corpus-aux
	awk -f corpus.awk corpus-aux | sort > corpus-post
	cat input | mpiexec -np 3 ./dist_hashmap | sed -e '1,/funcional/d' | sed '/>/d' | sort | diff -u - corpus-post | awk '{ print } END { if (NR) print "Test FAILED"; else print "Test OK" }'
	rm -f input corpus-aux corpus-post
	
# member de "noestoy" y "yotampocoestoy" (ninguna definida) sobre un hashmap vacio
# Ejecucion:
# mpiexec -np 3 ./dist_hashmap
# member noestoy
# member yotampocoestoy
# q
test-member-1:
	########## test-member-1 ##########		
	printf " member noestoy \n member yotampocoestoy \n q \n" > input		
	printf "La clave noestoy NO ESTA DEFINIDA\nLa clave yotampocoestoy NO ESTA DEFINIDA\n" > output			
	cat input | mpiexec -np 3 ./dist_hashmap | grep DEFINIDA | sed 's/> //' | diff -u - output | awk '{ print } END { if (NR) print "Test FAILED"; else print "Test OK" }'
	rm -f input output

# member de "noestoy" y "yotampocoestoy" (ninguna definida) sobre un hashmap con 4 archivos cargados, todos en distintos nodos
# Ejecucion:
# mpiexec -np 5 ./dist_hashmap
# load load corpus-0 corpus-1 corpus-2 corpus-3
# member noestoy
# member yotampocoestoy
# q
test-member-2:
	########## test-member-2 ##########		
	printf " load corpus-0 corpus-1 corpus-2 corpus-3 \n member noestoy \n member yotampocoestoy \n q \n" > input		
	printf "La clave noestoy NO ESTA DEFINIDA\nLa clave yotampocoestoy NO ESTA DEFINIDA\n" > output		
	for i in 0 1 2 3; do sed -n "$$((i * 625 + 1)),$$(((i + 1) * 625))p" corpus > corpus-"$$i"; done	
	cat input | mpiexec -np 5 ./dist_hashmap | grep DEFINIDA | sed 's/> //' | diff -u - output | awk '{ print } END { if (NR) print "Test FAILED"; else print "Test OK" }'
	rm -f input output corpus-[0-3]

# member de "archivo" y "estabilidad" (ambas definidas) sobre un hashmap con 4 archivos cargados, todos en distintos nodos
# Ejecucion:
# mpiexec -np 5 ./dist_hashmap
# load corpus-0 corpus-1 corpus-2 corpus-3
# member archivo
# member estabilidad
# q
test-member-3:
	########## test-member-3 ##########		
	printf " load corpus-0 corpus-1 corpus-2 corpus-3 \n member archivo \n member estabilidad \n q \n" > input
	printf "La clave archivo ESTA DEFINIDA\nLa clave estabilidad ESTA DEFINIDA\n" > output	
	for i in 0 1 2 3; do sed -n "$$((i * 625 + 1)),$$(((i + 1) * 625))p" corpus > corpus-"$$i"; done	
	cat input | mpiexec -np 5 ./dist_hashmap | grep DEFINIDA | sed 's/> //' | diff -u - output | awk '{ print } END { if (NR) print "Test FAILED"; else print "Test OK" }'
	rm -f input output corpus-[0-3]

# member de "nuevaclave" (no definida) sobre hashmap con un archivo cargado. Luego hace addAndInc de "nuevaclave" y llama a member de "nuevaclave" nuevamente (esta vez definido)
# Ejecucion:
# mpiexec -np 3 ./dist_hashmap
# load corpus
# member nuevaclave
# addAndInc nuevaclave
# member nuevaclave
# q
test-member-4:
	########## test-member-4 ##########		
	printf " load corpus \n member nuevaclave \n addAndInc nuevaclave \n member nuevaclave \n q \n" > input		
	printf "La clave nuevaclave NO ESTA DEFINIDA\nLa clave nuevaclave ESTA DEFINIDA\n" > output
	cat input | mpiexec -np 2 ./dist_hashmap | grep DEFINIDA | sed 's/> //' | diff -u - output | awk '{ print } END { if (NR) print "Test FAILED"; else print "Test OK" }'
	rm -f input output

# maximum sobre hashmap con corpus-max cargado 
# Ejecucion:
# mpiexec -np 3 ./dist_hashmap
# load corpus-max
# maximum
# q
test-maximum-1:
	########## test-maximum-1 ##########		
	printf " load corpus-max \n maximum \n q \n" > input		
	printf "El maximo es <clavemaxima, 9>\n" > output
	cat input | mpiexec -np 3 ./dist_hashmap | grep maximo | sed 's/> //' | diff -u - output | awk '{ print } END { if (NR) print "Test FAILED"; else print "Test OK" }'
	rm -f input output

# maximum sobre hashmap con corpus-max cargado (dividido en 5 archivos). Localmente en cada nodo, "clavemaxima" no es maximo en ninguno de lo 5, sin embargo es maximo del hashmap completo
# Ejecucion:
# mpiexec -np 6 ./dist_hashmap
# load corpus-max-0 corpus-max-1 corpus-max-2 corpus-max-3 corpus-max-4
# maximum
# q
test-maximum-2:
	########## test-maximum-2 ##########		
	printf " load corpus-max-0 corpus-max-1 corpus-max-2 corpus-max-3 corpus-max-4 \n maximum \n q \n" > input
	printf "El maximo es <clavemaxima, 9>\n" > output
	for i in 0 1 2 3 4; do sed -n "$$((i * 500 + 1)),$$(((i + 1) * 500))p" corpus-max > corpus-max-"$$i"; done
	cat input | mpiexec -np 6 ./dist_hashmap | grep maximo | sed 's/> //' | diff -u - output | awk '{ print } END { if (NR) print "Test FAILED"; else print "Test OK" }'
	rm -f input output corpus-max-[0-4]

# maximum sobre hashmap con corpus-max cargado (unico archivo). Se llama a maximum para ver que el maximo es "clavemaxima", con valor 9. Luego se hace addAndInc de "nuevomax" 10 veces y se llama a maximum nuevamente
# Ejecucion:
# mpiexec -np 3 ./dist_hashmap
# load corpus-max
# maximum
# addAndInc nuevomax
# ...
# addAndInc nuevomax
# maximum
# q
test-maximum-3:
	########## test-maximum-3 ##########		
	printf " load corpus-max \n maximum \n" > input && for i in 0 1 2 3 4 5 6 7 8 9; do printf " addAndInc nuevomax \n" >> input; done && printf " maximum \n q \n" >> input
	printf "El maximo es <clavemaxima, 9>\nEl maximo es <nuevomax, 10>\n" > output
	cat input | mpiexec -np 3 ./dist_hashmap | grep maximo | sed 's/> //' | diff -u - output | awk '{ print } END { if (NR) print "Test FAILED"; else print "Test OK" }'
	rm -f input output