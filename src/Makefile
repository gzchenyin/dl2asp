CC = g++
LEX = flex
PARSER = bison
PICOSAT_INCLUDE = . 
CLASP_INCLUDE = . 
#CFLAGS = -DNEDEBUG -O  -Wno-deprecated
#CFLAGS = -Wall -Wno-deprecated -I$(PICOSAT_INCLUDE) -I$(CLASP_INCLUDE) -pipe -fno-strict-aliasing -ggdb
CFLAGS = -Wno-deprecated -I$(PICOSAT_INCLUDE) -I$(CLASP_INCLUDE) -pipe -fno-strict-aliasing -ggdb

#picosat
PICOSAT_LIB = libpicosat.a

#DL
DL_HEADER = tool.h \
	dlsolver.h \
	dlparser.h \
	dlrule.h \
	aspsolver.h \
	MUSbuilder.h \

DL_OBJECT = main.o \
	tool.o \
	dlsolver.o \
	dlparser.o \
	lex.o \
	parse.o \
	dlrule.o \
	aspsolver.o \
	MUSbuilder.o \
	

DL_SOURCE = $(DL_OBJECT:.o=.cpp)

#clasp
CLASP_LIB = clasp/libclasp.a

dl2asp : $(DL_OBJECT) $(PICOSAT_LIB) $(CLASP_LIB)
	$(CC) -o dl2asp $(DL_OBJECT) $(PICOSAT_LIB) $(CLASP_LIB)
	
$(DL_OBJECT) : $(DL_HEADER)

lex.o : lex.l parse.o
	$(LEX) -olex.cpp lex.l
	$(CC)  $(CFLAGS) -c -o lex.o lex.cpp

parse.o : parse.y
	$(PARSER) --defines=parse.h --output=parse.cpp parse.y
	$(CC) $(CFLAGS) -c -o parse.o parse.cpp


.cpp.o:
	$(CC) $< $(CFLAGS) -c -o $@

.C.o:
	$(CC) $< $(CFLAGS) -c -o $@

clean :
	rm -f lex.cpp
	rm -f parse.h
	rm -f parse.cpp
	rm -f *.o
	rm -f dl2asp
	

