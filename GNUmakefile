
#############################################################################
## Variables
#############################################################################

SHELL = /bin/bash

LIB_HDRS = $(wildcard *.h)
LIB_SRCS = $(wildcard *.cc)
LIB_OBJS = $(patsubst %.cc, %.o, $(LIB_SRCS))
LIB_DEPS = $(patsubst %.o, %.d, $(LIB_OBJS))

PRG_SRCS = $(wildcard *.cpp)
PRG_EXES = $(patsubst %.cpp, %.exe, $(PRG_SRCS))

LIB = libcsvchunks.so

CXX = g++
CPPFLAGS = -I.
LDFLAGS = -L.
CXXFLAGS = -std=c++20 -O2 -Wall -Wextra -Wfloat-equal
DEPFLAGS = -MT $@ -MMD -MP -MF $*.td


#############################################################################
## General targets
#############################################################################

all: $(PRG_EXES)

libs: $(LIB)

objs: $(LIB_OBJS)

verbose:
	@echo ""
	@echo "LIB_HDRS:" $(LIB_HDRS)
	@echo "LIB_SRCS:" $(LIB_SRCS)
	@echo "LIB_OBJS:" $(LIB_OBJS)
	@echo "LIB_DEPS:" $(LIB_DEPS)
	@echo ""
	@echo "PRG_SRCS:" $(PRG_SRCS)
	@echo "PRG_EXES:" $(PRG_EXES)
	@echo ""
	@echo "LIB:" $(LIB)
	@echo "LDFLAGS:" $(LDFLAGS)
	@echo ""

clean:
	rm -f *.exe *.o *.d *.td
	rm -f *~ core.* core vgcore.* .stamp-*

doc:
	doxygen


#############################################################################
## Bibliotheques
## http://www.yolinux.com/TUTORIALS/LibraryArchives-StaticAndDynamic.html
#############################################################################

libcsvchunks.so: $(LIB_OBJS)
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
## Regles implicites
#############################################################################

%.exe: %.cpp $(LIB)
	rm -f $@
	$(LINK.cc) -lpthread -o $@ $^

%.o : %.cc
%.o : %.cc %.d
	$(COMPILE.cc) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

%.d: ;
.PRECIOUS: %.d %.o

include $(LIB_DEPS)


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

