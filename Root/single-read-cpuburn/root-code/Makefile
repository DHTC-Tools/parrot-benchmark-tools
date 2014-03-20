RC     := root-config
ifeq ($(shell which $(RC) 2>&1 | sed -ne "s@.*/$(RC)@$(RC)@p"),$(RC))
MKARCH := $(wildcard $(shell $(RC) --etcdir)/Makefile.arch)
endif
ifneq ($(MKARCH),)
include $(MKARCH)
else
include $(ROOTSYS)/test/Makefile.arch
endif
# -include ../MyConfig.mk

ALIBS = $(LIBS) -lTreePlayer

#------------------------------------------------------------------------------
READO       = read.$(ObjSuf)
READS       = read.$(SrcSuf)
READ        = readDirect-cpuburn$(ExeSuf)

INVERT	    = invert
INVERTS     = invert.C

OBJS          = $(READO) 
PROGRAMS      = $(READ) $(INVERT)

#------------------------------------------------------------------------------

.SUFFIXES: .$(SrcSuf) .$(ObjSuf) .$(DllSuf)

all:            $(PROGRAMS)

$(INVERT):
		g++ $(INVERTS) -o $(INVERT)
		@echo "$@ done"

$(READ):      $(READO)
		$(LD) $(LDFLAGS) $^ $(ALIBS) $(OutPutOpt)$@
		$(MT_EXE)
		@echo "$@ done"

.PHONY: clean

clean:
	rm -f $(PROGRAMS) $(OBJS)
