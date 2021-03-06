# ------------------------------------------------------------
# Basic variables
# ------------------------------------------------------------

PART_SRCS = boson.cpp classical.cpp boson_eff.cpp fermion_eff.cpp \
	fermion.cpp fermion_nonrel.cpp part.cpp quark.cpp \
	fermion_rel.cpp fermion_deriv_rel.cpp fermion_deriv_nr.cpp \
	classical_deriv.cpp boson_rel.cpp fermion_mag_zerot.cpp \
	part_deriv.cpp
HEADER_VAR = boson.h classical.h boson_eff.h fermion_eff.h \
	fermion.h fermion_nonrel.h part.h quark.h fermion_rel.h \
	part_deriv.h fermion_deriv_rel.h fermion_deriv_nr.h classical_deriv.h \
	boson_rel.h fermion_mag_zerot.h
TEST_VAR = classical.scr fermion_eff.scr fermion_rel.scr boson.scr \
	fermion.scr fermion_nonrel.scr part.scr quark.scr \
	boson_eff.scr fermion_deriv_rel.scr fermion_deriv_nr.scr \
	classical_deriv.scr boson_rel.scr fermion_mag_zerot.scr

# ------------------------------------------------------------
# Includes
# ------------------------------------------------------------

if O2SCL_PARTLIB
O2SCL_PART_MVAR = -DO2SCL_PART
else
O2SCL_PART_MVAR =
endif

if O2SCL_EOSLIB
O2SCL_EOS_MVAR = -DO2SCL_EOS
else
O2SCL_EOS_MVAR =
endif

if O2SCL_HDF
O2SCL_HDF_MVAR = -DO2SCL_HDF
else
O2SCL_HDF_MVAR =
endif

if O2SCL_OPENMP
O2SCL_OPENMP_MVAR = -DO2SCL_OPENMP -fopenmp
else
O2SCL_OPENMP_MVAR =
endif

if O2SCL_EIGEN
O2SCL_EIGEN_MVAR = -DO2SCL_EIGEN
else
O2SCL_EIGEN_MVAR =
endif

if O2SCL_ARMA
O2SCL_ARMA_MVAR = -DO2SCL_ARMA
else
O2SCL_ARMA_MVAR =
endif

AM_CPPFLAGS = -I@top_srcdir@/include/ -DO2SCL_DATA_DIR=\"${datadir}/o2scl/\" \
	$(O2SCL_PART_MVAR) $(O2SCL_EOS_MVAR) $(O2SCL_HDF_MVAR) \
	$(O2SCL_OPENMP_MVAR) $(O2SCL_EIGEN_MVAR) $(O2SCL_ARMA_MVAR) \
	-DO2SCL_COND_FLAG

# ------------------------------------------------------------
# Install headers
# ------------------------------------------------------------

am__pkginclude_HEADERS_DIST = $(HEADER_VAR)

pkginclude_HEADERS = $(HEADER_VAR)

# ------------------------------------------------------------
# libtool testing targets
# ------------------------------------------------------------

check_PROGRAMS = classical_ts fermion_eff_ts fermion_rel_ts \
	boson_ts fermion_ts fermion_nonrel_ts \
	part_ts quark_ts fermion_deriv_rel_ts fermion_mag_zerot_ts \
	classical_deriv_ts fermion_deriv_nr_ts boson_rel_ts boson_eff_ts

check_SCRIPTS = o2scl-test

VCHECK_LIBS = ../libo2scl_part.la ../hdf/libo2scl_hdf.la ../libo2scl.la

classical_ts_LDADD = $(VCHECK_LIBS)
fermion_eff_ts_LDADD = $(VCHECK_LIBS)
fermion_rel_ts_LDADD = $(VCHECK_LIBS)
boson_ts_LDADD = $(VCHECK_LIBS)
fermion_ts_LDADD = $(VCHECK_LIBS)
part_ts_LDADD = $(VCHECK_LIBS)
quark_ts_LDADD = $(VCHECK_LIBS)
classical_deriv_ts_LDADD = $(VCHECK_LIBS)
fermion_nonrel_ts_LDADD = $(VCHECK_LIBS)
fermion_deriv_rel_ts_LDADD = $(VCHECK_LIBS)
fermion_deriv_nr_ts_LDADD = $(VCHECK_LIBS)
boson_rel_ts_LDADD = $(VCHECK_LIBS)
boson_eff_ts_LDADD = $(VCHECK_LIBS)
fermion_mag_zerot_ts_LDADD = $(VCHECK_LIBS)

classical.scr: classical_ts$(EXEEXT) 
	./classical_ts$(EXEEXT) > classical.scr
fermion_eff.scr: fermion_eff_ts$(EXEEXT) 
	./fermion_eff_ts$(EXEEXT) > fermion_eff.scr
fermion_rel.scr: fermion_rel_ts$(EXEEXT) 
	./fermion_rel_ts$(EXEEXT) > fermion_rel.scr
boson.scr: boson_ts$(EXEEXT) 
	./boson_ts$(EXEEXT) > boson.scr
fermion.scr: fermion_ts$(EXEEXT) 
	./fermion_ts$(EXEEXT) > fermion.scr
part.scr: part_ts$(EXEEXT) 
	./part_ts$(EXEEXT) > part.scr
quark.scr: quark_ts$(EXEEXT) 
	./quark_ts$(EXEEXT) > quark.scr
classical_deriv.scr: classical_deriv_ts$(EXEEXT) 
	./classical_deriv_ts$(EXEEXT) > classical_deriv.scr
fermion_nonrel.scr: fermion_nonrel_ts$(EXEEXT) 
	./fermion_nonrel_ts$(EXEEXT) > fermion_nonrel.scr
fermion_deriv_rel.scr: fermion_deriv_rel_ts$(EXEEXT) 
	./fermion_deriv_rel_ts$(EXEEXT) > fermion_deriv_rel.scr
fermion_deriv_nr.scr: fermion_deriv_nr_ts$(EXEEXT) 
	./fermion_deriv_nr_ts$(EXEEXT) > fermion_deriv_nr.scr
boson_rel.scr: boson_rel_ts$(EXEEXT) 
	./boson_rel_ts$(EXEEXT) > boson_rel.scr
boson_eff.scr: boson_eff_ts$(EXEEXT) 
	./boson_eff_ts$(EXEEXT) > boson_eff.scr
fermion_mag_zerot.scr: fermion_mag_zerot_ts$(EXEEXT) 
	./fermion_mag_zerot_ts$(EXEEXT) > fermion_mag_zerot.scr

classical_ts_SOURCES = classical_ts.cpp
fermion_eff_ts_SOURCES = fermion_eff_ts.cpp
fermion_rel_ts_SOURCES = fermion_rel_ts.cpp
boson_ts_SOURCES = boson_ts.cpp
fermion_ts_SOURCES = fermion_ts.cpp
part_ts_SOURCES = part_ts.cpp
quark_ts_SOURCES = quark_ts.cpp
classical_deriv_ts_SOURCES = classical_deriv_ts.cpp
fermion_nonrel_ts_SOURCES = fermion_nonrel_ts.cpp
fermion_deriv_rel_ts_SOURCES = fermion_deriv_rel_ts.cpp
fermion_deriv_nr_ts_SOURCES = fermion_deriv_nr_ts.cpp
boson_rel_ts_SOURCES = boson_rel_ts.cpp
boson_eff_ts_SOURCES = boson_eff_ts.cpp
fermion_mag_zerot_ts_SOURCES = fermion_mag_zerot_ts.cpp

# ------------------------------------------------------------
# Library o2scl_basepart
# ------------------------------------------------------------

noinst_LTLIBRARIES = libo2scl_basepart.la

libo2scl_basepart_la_SOURCES = $(PART_SRCS)

am__libo2scl_basepart_la_SOURCES_DIST = $(PART_SRCS)

# ------------------------------------------------------------
# Misc
# ------------------------------------------------------------

EXTRA_DIST = *_ts.cpp

emacs-clean: 
	-rm *~

qbkup:
	cp *.cpp *.h Makefile.am bkup

# ------------------------------------------------------------
# Testing
# ------------------------------------------------------------

o2scl-test: $(TEST_VAR)
	echo $(TEST_VAR) > testlist
	../internal/tsumm$(EXEEXT)

o2scl-test-rec: $(TEST_VAR)
	echo "src/part" $(TEST_VAR) >> ../../testlist

test-clean:
	-rm *_ts.o *_ts *.scr

