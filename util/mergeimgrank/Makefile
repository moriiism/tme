include ../../../mitool/Makefile.in
include ../../../mitool/milib/Makefile.in
include ../../../mitool/miblaslib/Makefile.in
include ../../../mitool/mifitslib/Makefile.in
include ../../../mitool/mirootlib/Makefile.in

INCLUDE = $(MIROOTLIB_INC) $(MIFITSLIB_INC) $(MIBLASLIB_INC) $(MILIB_INC) $(ROOT_INC) $(CFITSIO_INC) $(MT_INC)
LDLIBS  = $(MIROOTLIB_LIB) $(MIFITSLIB_LIB) $(MIBLASLIB_LIB) $(MILIB_LIB) $(ROOT_ALLLIBS) $(CFITSIO_LIB) \
	$(LAPACK_LIB) $(BLAS_LIB) $(MT_LIB) $(LIB)

TARGET = mergeimgrank

all: $(TARGET) 

mergeimgrank: mergeimgrank.o  arg_mergeimgrank.o
	$(CXXN) $@.o arg_$@.o ${LDLIBS} -o $@

.SUFFIXES:
.SUFFIXES: .o .c .f .F .cc

.c.o:
	$(CCN) -c $<

.F.o : 
	$(FCN) -c $<

.f.o:
	$(FCN) -c $<

.cc.o:
	${CXXN} -c $<

clean:
	-rm -f *.o

cleaner: clean
	-rm -f ${TARGET} *% *~ core
