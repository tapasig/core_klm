AUTOMAKE_OPTIONS = foreign

lib_LTLIBRARIES = \
    libdifftaggana.la

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(ROOTSYS)/include \
  -isystem ${G4_MAIN}/include

#  -I/cvmfs/eic.opensciencegrid.org/x8664_sl7/opt/fun4all/core/gsl-2.6/include 


pkginclude_HEADERS = \
  magnetMaterialAnalysis.h

if ! MAKEROOT6
  ROOT5_DICTS = \
  diff_tagg_ana_Dict.h
endif

libdifftaggana_la_SOURCES = \
  $(ROOT5_DICTS) \
  magnetMaterialAnalysis.cc

libdifftaggana_la_LDFLAGS  = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -lcalo_io \
  -lfun4all \
  -lg4detectors_io \
  -lphg4hit\
  -lgsl \
  -lgslcblas \
  -lg4eval_io \
  -lg4eval \
  -leiceval


#   -L/cvmfs/eic.opensciencegrid.org/x8664_sl7/opt/fun4all/core/gsl-2.6/lib \ 
#   \ -lm

################################################
# linking tests

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.C
testexternals_LDADD = libdifftaggana.la


testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

# Rule for generating table CINT dictionaries.
%_Dict.cc: %.h %LinkDef.h
	rootcint -f $@ @CINTDEFS@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

clean-local:
	rm -f *Dict* $(BUILT_SOURCES) *.pcm
