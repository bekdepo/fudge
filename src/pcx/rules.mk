B:=\
    $(DIR_SRC)/pcx/pcxcmap \

O:=\
    $(DIR_SRC)/pcx/gfx.o \
    $(DIR_SRC)/pcx/pcx.o \
    $(DIR_SRC)/pcx/pcxcmap.o \

L:=\
    $(DIR_SRC)/abi/abi.a \
    $(DIR_SRC)/fudge/fudge.a \

include $(DIR_MK)/bin.mk

B:=\
    $(DIR_SRC)/pcx/pcxdata \

O:=\
    $(DIR_SRC)/pcx/gfx.o \
    $(DIR_SRC)/pcx/pcx.o \
    $(DIR_SRC)/pcx/pcxdata.o \

L:=\
    $(DIR_SRC)/abi/abi.a \
    $(DIR_SRC)/fudge/fudge.a \

include $(DIR_MK)/bin.mk
