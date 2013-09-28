MOD:=modules/base/base.ko
OBJ:=modules/base/main.o modules/base/block.o modules/base/clock.o modules/base/keyboard.o modules/base/mouse.o modules/base/network.o modules/base/terminal.o modules/base/timer.o modules/base/video.o

$(MOD): $(OBJ) $(LIBFUDGE)
	$(LD) $(LDFLAGS) -o $@ $^

MODULES+=$(MOD)
MODULES_OBJECTS+=$(OBJ)
