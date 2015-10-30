
# Makefile for Reason
#
# -wall = Show all warning messages
# -w	= Inhibit all warning messages
# -lm	= Link the standard C math library
# -lpthread = Posix threads library
# -lrt = Realtime library for high resolution timers
# -Os	= Optimise for size
# -O3	= Optimise level 3
# -O0 	= No optimisation
# -g	= Generate debug information
# -ggdb	= Generate GDB debug information

PROJECT=reason
REASON=./

CC=g++

INC=-Isrc -Isrc/$(PROJECT) -I$(REASON)src 

# Ruby requires some aditional libraries, and disturbingly so does sqlite 3.5
# what is it with dependency bloat.
LIB=-lm -lpthread -lz -L$(REASON) -ldl 

OBJ=

#$(REASON)lib/ruby/libruby-static.a


#OPTARCH=-march=pentium4
OPTARCH=
OPTFLAG=-fpermissive -fno-operator-names -fno-rtti -fno-exceptions

#CFLAGS=$(OPTFLAG) $(OPTARCH) -w -Os 
#CFLAGS=$(OPTFLAG) $(OPTARCH) -w -O3 
# OSX requires -Wl,flat_namespace or it doesnt build the right symbols for a library
# to be built from the object files.
#CFLAGS=$(OPTFLAG) $(OPTARCH) -w -O3 -ggdb
CFLAGS=$(OPTFLAG) $(OPTARCH) -w -O0 -ggdb -D_DEBUG 
#CFLAGS=$(OPTFLAG) $(OPTARCH) -w -O2 -g -D_DEBUG 
#CFLAGS=$(OPTFLAG) $(OPTARCH) -fvtable-thunks -w -g -O0 -D_DEBUG 

SRCDIR=src
OBJDIR=build

#SRCFILES=$(shell find $(SRCDIR)/ -maxdepth 3 -name "*.cpp")
SRCFILES=$(shell find $(SRCDIR)/ -name "*.cpp")
INCFILES=$(shell find $(SRCDIR)/ -name "*.d")
#SUBFILES=$(wildcard *.d)
OBJFILES=$(patsubst %.cpp,%.o,$(SRCFILES))
DEPFILES=$(patsubst %.cpp,%.d,$(SRCFILES))

default: all

clean:
	$(warning $(OBJFILES))
	rm -f $(OBJFILES)
	rm -f $(DEPFILES)
	#rm -rf *.o
	#rm -rf *.d

all: $(PROJECT)

$(PROJECT): $(OBJFILES)	 
	$(CC) $(OBJFILES) $(OBJ) $(LIB) -o $@

#%.o: $(SRCFILES)
#%.o:
#$(OBJFILES): $(SRCFILES)

#$(warning $(OBJFILES))

$(OBJFILES):  
	$(CC) $(INC) $(CFLAGS) -c $< -o $@

#dep: $(SRCFILES)
#	$(CC) $(INC) -MMD -E $(SRCFILES) > /dev/null

#$(warning $(SRCFILES))

depend: $(DEPFILES)

#$(DEPFILES): $(SRCFILES)
#	$(warning $< $@)
#	$(CC) $(INC) $< -E -MM -MF $(patsubst %.cpp,%.d,$<) > /dev/null

library: $(OBJFILES)
	rm -f libreason.a
	libtool -static $(filter-out reason% test%,$(OBJFILES)) -o libreason.a
	
#ar rcs libreason.a $(filter-out reason% test%,$(OBJFILES)) 

# The -MF option combined with the $@ to specify the rule from $(DEPFILES) allows me to 
# specify the correct output file.  This is the only way ive found to do this, you cant
# use $< as it represents only the first dependency, as in the .cpp file which includes
# all the .h files in a dependency output.  The -MT lets me specify the name of the rule
# target which is created so that it can be matched with whats in $(OBJFILES), again we
# want the full path to remove ambiguity.

$(DEPFILES): 
	$(warning $< $@)
	$(CC) $(INC) $(patsubst %.d,%.cpp,$@) -E -MM -MT $(patsubst src/%.d,src/%.o,$@) -MF $@ > /dev/null
	
# To get the dependencies and objects to generate in the build directory we first need 
# to construct a directory heirarchy that looks like the src directory under build.
# This can be done using cd build;(cd src; find -type d ! -name . | xargs mkdir)
##$(CC) $(INC) $(patsubst %.d,%.cpp,$@) -E -MM -MT $(patsubst src/%.d,build/%.o,$@) -MF $(patsubst src/%.d,build/%.d,$@) > /dev/null

#$(warning $(INCFILES))

#$(warning $(SUBFILES))

# Ok, so ive got some magic above outputing a dependency file for every path in $(DEPFILES)
# in the relevant subdirectory, but now i dont think we can include them because include
# seems to be missing the full path or cant look in subdirectories.

include $(INCFILES)
