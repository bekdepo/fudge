M:=\
    $(DIR_SRC)/modules/udp/udp.ko \

O:=\
    $(DIR_SRC)/modules/udp/main.o \

L:=\
    $(DIR_SRC)/fudge/fudge.a \

include $(DIR_MK)/mod.mk
