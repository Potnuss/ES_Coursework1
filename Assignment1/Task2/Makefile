# Specify directories where the various tools and libraries can be found
FS_PREFIX=/group/teaching/espractical/Freescale/

FST_PREFIX=$(FS_PREFIX)/ARM_Tools/Command_Line_Tools/
MQX_PREFIX=$(FS_PREFIX)/MQX

CC=$(FST_PREFIX)mwccarm
AS=$(FST_PREFIX)mwasmarm
LD=$(FST_PREFIX)mwccarm

# Set up CFLAGS to build an MQX project
INCDIRS=-i $(FS_PREFIX)/ARM_EABI_Support/ewl/EWL_C/include/ -i $(MQX_PREFIX)/ -i $(MQX_PREFIX)/bsp -i $(MQX_PREFIX)/psp -i $(MQX_PREFIX)/rtcs -i $(MQX_PREFIX)/shell -i $(MQX_PREFIX)/bsp/Generated_Code
CFLAGS=-gccinc -gccdep -MMD -proc=cortex-m4 -cwd include -enc ascii $(INCDIRS)

# Set up the linker to build an MQX project
MQX_LIBDIRS=-L$(MQX_PREFIX)/bsp -L$(MQX_PREFIX)/psp -L$(MQX_PREFIX)/rtcs -L$(FS_PREFIX)/ARM_EABI_Support/ewl/lib
MQX_LIBS= -lbsp.a -lpsp.a -ltss.a -lrtcs.a
LDSCRIPT=$(MQX_PREFIX)/mqx.lcf
LDFLAGS=$(MQX_LIBDIRS) $(MQX_LIBS) -proc=cortex-m4 -thumb -main __boot -nostdlib $(LDSCRIPT) -lavender model=ewl ,print=int ,scan=int ,io=raw

# Specify the source files to be compiled here
SOURCES=main.c rtcs_init.c
OBJECTS = $(SOURCES:.c=.o)
DEPFILES=$(SOURCES:.c=.d)

# Recipe to build the binary
main : $(OBJECTS)
	$(LD) $(LDFLAGS) $(STARTUP) $^ $(LDFLAGS) -o $@

# Recipe to make each individual object file
%.o : %.c
	$(CC) $(CFLAGS) -o "$@" -c "$<"

# Recipe to tidy up the build folder
clean:
	rm -f main
	rm -f $(OBJECTS) $(DEPFILES)

# Include dependency tracking files to make sure we rebuild the correct 
# objects if a header file is changed
-include $(DEPFILES)
