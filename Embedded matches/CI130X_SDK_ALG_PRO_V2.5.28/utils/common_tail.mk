

%.o : %.c
	$(CC_PREFIX)$(CC) $(C_FLAGS) -c -o "$@" "$<"

%.o : %.S
	$(CC_PREFIX)$(AS) $(S_FLAGS) -c -o "$@" "$<"

build/source_file.mk: source_file.prj
	# sh $(SDK_PATH)/tools/generate_makefile.sh
	$(LUA) $(SDK_PATH)/utils/generate_makefile.lua source_file.prj $(SDK_PATH)

clean:
	-$(RM) -rf build/*
	-$(RM) -rf *.a
	-@echo ' '

.PHONY: all clean dependents
