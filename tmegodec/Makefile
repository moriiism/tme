include ../../mitool/Makefile.in
include ../../mitool/milib/Makefile.in
include ../../mitool/miblaslib/Makefile.in
include ../../mitool/mifitslib/Makefile.in
include ../../mitool/mirootlib/Makefile.in

INCLUDE = $(MIFITSLIB_INC) $(MIBLASLIB_INC) $(MILIB_INC) $(CFITSIO_INC) $(MT_INC)
LDLIBS  = $(MIFITSLIB_LIB) $(MIBLASLIB_LIB) $(MILIB_LIB) $(CFITSIO_LIB) \
	$(LAPACK_LIB) $(BLAS_LIB) $(MT_LIB) $(LIB)

TARGET = tmegodec  tmegodec_4d

all: $(TARGET) 

tmegodec: tmegodec.o  arg_tmegodec.o  sub_tmegodec.o
	$(CXXN) $@.o arg_$@.o sub_$@.o ${LDLIBS} -o $@

tmegodec_4d: tmegodec_4d.o  arg_tmegodec.o  sub_tmegodec.o
	$(CXXN) $@.o arg_tmegodec.o sub_tmegodec.o ${LDLIBS} -o $@

test: test.o
	$(CXXN) $@.o ${LDLIBS} -o $@

temp: temp.o
	$(CXXN) $@.o ${LDLIBS} -o $@

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

install: all
	if [ ! -d $(INST_BIN_DIR) ]; then mkdir $(INST_BIN_DIR); fi
	install ${TARGET} ${INST_BIN_DIR}

clean:
	-rm -f *.o

cleaner: clean
	-rm -f ${TARGET} *% *~ core
