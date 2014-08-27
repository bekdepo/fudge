MOD_$(DIR):=$(DIR)/ps2.ko.0
OBJ_$(DIR):=$(DIR)/main.o $(DIR)/keyboard_driver.o $(DIR)/mouse_driver.o

$(MOD_$(DIR)): $(OBJ_$(DIR))
	$(LD) -o $@ $^ $(LDFLAGS)

MODULES:=$(MODULES) $(MOD_$(DIR))
CLEAN:=$(CLEAN) $(MOD_$(DIR)) $(OBJ_$(DIR))
