
#############################################################################
## Variables
#############################################################################

SHELL = /bin/bash

HDRS_UTIL = $(wildcard ../src/util/*.h)
SRCS_UTIL = $(wildcard ../src/util/*.cc) #/usr/local/src/tz.cc
OBJS_UTIL = $(patsubst ../src/util/%.cc, %.o, $(filter-out ../src/util/fichier_pdf.cc, $(SRCS_UTIL))) tz.o

HDRS_DATA = $(wildcard ../src/data/*.h)
SRCS_DATA = $(wildcard ../src/data/*.cc)
OBJS_DATA = $(patsubst ../src/data/%.cc, %.o, $(SRCS_DATA))

HDRS_ALGO = $(wildcard ../src/algo/*.h)
SRCS_ALGO = $(wildcard ../src/algo/*.cc)
OBJS_ALGO = $(patsubst ../src/algo/%.cc, %.o, $(SRCS_ALGO))

SRCS_PROG = $(wildcard ../src/prog/*.cpp)
OBJS_PROG = $(patsubst ../src/prog/%.cpp, %.o, $(SRCS_PROG))
PRGS = $(patsubst ../src/prog/%.cpp, %.exe, $(SRCS_PROG))

LIBS = libutil.so libdata.so libalgo.so
DEPS = $(patsubst %.o, %.d, $(OBJS_UTIL) fichier_pdf.o $(OBJS_DATA) $(OBJS_ALGO) $(OBJS_PROG))

CXX = g++
CPPFLAGS = -I${MAIN_DIR}/src
LDFLAGS = -L${MAIN_DIR}/build
CXXFLAGS = -std=c++20 -O2 -Wall -Wextra -Wfloat-equal
DEPFLAGS = -MT $@ -MMD -MP -MF $*.td


#############################################################################
## General targets
#############################################################################

all: $(PRGS)

libs: $(LIBS)

objs: fichier_pdf.o $(OBJS_DATA) $(OBJS_ALGO) $(OBJS_PROG)

verbose:
	@echo ""
	@echo "HDRS_UTIL:" $(HDRS_UTIL)
	@echo "SRCS_UTIL:" $(SRCS_UTIL)
	@echo "OBJS_UTIL:" $(OBJS_UTIL)
	@echo ""
	@echo "HDRS_DATA:" $(HDRS_DATA)
	@echo "SRCS_DATA:" $(SRCS_DATA)
	@echo "OBJS_DATA:" $(OBJS_DATA)
	@echo ""
	@echo "HDRS_ALGO:" $(HDRS_ALGO)
	@echo "SRCS_ALGO:" $(SRCS_ALGO)
	@echo "OBJS_ALGO:" $(OBJS_ALGO)
	@echo ""
	@echo "SRCS_PROG:" $(SRCS_PROG)
	@echo "OBJS_PROG:" $(OBJS_PROG)
	@echo "PRGS:" $(PRGS)
	@echo ""
	@echo "LIBS:" $(LIBS)
	@echo "DEPS:" $(DEPS)
	@echo "LDFLAGS:" $(LDFLAGS)

clean:
	rm -f *.exe *.o *.d *.td
	rm -f *~ core.* core vgcore.* .stamp-*

doc:
	doxygen


#############################################################################
## Bibliotheques
## http://www.yolinux.com/TUTORIALS/LibraryArchives-StaticAndDynamic.html
#############################################################################

libutil.so: $(OBJS_UTIL)
	gcc -shared -o $@ $^

libdata.so: $(OBJS_DATA)
	gcc -shared -o $@ $^

libalgo.so: $(OBJS_ALGO)
	gcc -shared -o $@ $^


#############################################################################
## Correction des variables par defaut
## 
## Le passage par un fichier *.td (temporary dependencies) qui
## est renommé à la fin d'une compilation réussie, suivi d'un touch
## sert à garantir que le *.o aura une date ulterieure à l'*.d
#############################################################################

COMPILE.cc = $(CXX) $(DEPFLAGS) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -fPIC -c
POSTCOMPILE = @mv -f $*.td $*.d && touch $@


#############################################################################
## Regles explicites (ajout des bibliotheques hpdf et z)
#############################################################################

# un extern rajouté au vol
tz.o:
	$(COMPILE.cc) -DUSE_OS_TZDB=1 -o tz.o /usr/local/src/date/tz.cpp

# seule libutil.so est necessaire
vx_reformat.exe: vx_reformat.o libutil.so
	@rm -f vx_reformat.exe
	$(LINK.cc) -lpthread -o vx_reformat.exe $^

# ajout des bibliotheques hpdf et z
vx_genere_pdf.exe: vx_genere_pdf.o fichier_pdf.o $(LIBS)
	@rm -f vx_genere_pdf.exe
	$(LINK.cc) -lpthread -o vx_genere_pdf.exe $^ -lhpdf -lz


#############################################################################
## Regles implicites
#############################################################################


%.exe: %.o $(LIBS)
	@rm -f $@
	$(LINK.cc) -lpthread -o $@ $^

%.o : ../src/util/%.cc
%.o : ../src/util/%.cc %.d
	$(COMPILE.cc) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

%.o : ../src/data/%.cc
%.o : ../src/data/%.cc %.d
	$(COMPILE.cc) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

%.o : ../src/algo/%.cc
%.o : ../src/algo/%.cc %.d
	$(COMPILE.cc) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

%.o : ../src/prog/%.cpp
%.o : ../src/prog/%.cpp %.d
	$(COMPILE.cc) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

%.d: ;
.PRECIOUS: %.d %.o

include $(DEPS)


#############################################################################
## Dépendances
## http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/#tldr
#############################################################################

# DEPFLAGS = -MT $@ -MMD -MP -MF $*.d
# 
# %.o : %.c
# %.o : %.c $(DEPDIR)/%.d | $(DEPDIR)
#         $(COMPILE.c) $(OUTPUT_OPTION) $<
# 
# DEPFILES := $(SRCS:%.c=%.d)
# $(DEPFILES):
# 
# include $(wildcard $(DEPFILES))

