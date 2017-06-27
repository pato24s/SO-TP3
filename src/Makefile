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

test-all: test-load test-addAndInc

test-load: test-load-1 test-load-2 test-load-3

test-addAndInc: test-addAndInc-1 test-addAndInc-2 test-addAndInc-3

# load con cantidad de archivos menor que la cantidad de nodos
test-load-1:
	awk -f corpus.awk corpus | sort > corpus-post
	for i in 0 1; do sed -n "$$((i * 1250 + 1)),$$(((i + 1) * 1250))p" corpus > corpus-"$$i"; done
	./tests-load.sh 1
	rm -f corpus-post corpus-[0-1]

# load con cantidad de archivos mayor que la cantidad de nodos
test-load-2:
	awk -f corpus.awk corpus | sort > corpus-post
	for i in 0 1 2 3 4; do sed -n "$$((i * 500 + 1)),$$(((i + 1) * 500))p" corpus > corpus-"$$i"; done
	./tests-load.sh 2
	rm -f corpus-post corpus-[0-4]

# load con cantidad de archivos igual que la cantidad de nodos
test-load-3:
	awk -f corpus.awk corpus | sort > corpus-post
	for i in 0 1 2 3; do sed -n "$$((i * 625 + 1)),$$(((i + 1) * 625))p" corpus > corpus-"$$i"; done
	./tests-load.sh 3
	rm -f corpus-post corpus-[0-3]

# addAndInc de "primerstring" sobre un hashmap vacio
test-addAndInc-1:
	./tests-addAndInc.sh 1

# addAndInc de "nuevapalabrauno" y "nuevapalabrados" sobre un hashmap con corpus cargado
test-addAndInc-2:
	cat corpus > corpus-aux && echo "\nnuevapalabrauno\nnuevapalabrados" >> corpus-aux
	awk -f corpus.awk corpus-aux | sort > corpus-post
	./tests-addAndInc.sh 2
	rm -f corpus-aux corpus-post

# addAndInc de "archivo" "estabilidad" y "funcional" (todas ya existentes) sobre un hashmap con corpus cargado
test-addAndInc-3:
	cat corpus > corpus-aux && echo "\narchivo\nestabilidad\nfuncional" >> corpus-aux
	awk -f corpus.awk corpus-aux | sort > corpus-post
	./tests-addAndInc.sh 3
	rm -f corpus-aux corpus-post
	