################################################################################
# These are variables for the GBA toolchain build
# You can add others if you wish to
# ***** Joshua Philipose *****
################################################################################

# game name
PROGNAME = DefendTheWall

# List of object files that will be compiled into the program.
OFILES = main.o font.o myLib.o text.o images.o

# The header files I've created.
# This is necessary to determine when to recompile for files.
HFILES = text.h images.h

################################################################################
# These are various settings used to make the GBA toolchain work
################################################################################
include res/GBAVariables.mak

.PHONY: all
all : CFLAGS += $(CRELEASE)
all: $(PROGNAME).gba
	@echo "[FINISH] Created $(PROGNAME).gba"

$(PROGNAME).gba : $(PROGNAME).elf
	@echo "[LINK] Linking objects together to create $(PROGNAME).gba"
	@$(OBJCOPY) -O binary $(PROGNAME).elf $(PROGNAME).gba

$(PROGNAME).elf : res/crt0.o $(GCCLIB)/crtbegin.o $(GCCLIB)/crtend.o $(GCCLIB)/crti.o $(GCCLIB)/crtn.o $(OFILES)
	@$(CC) $(LINKFLAGS) -o $(PROGNAME).elf $^ -lgcc -lc $(LDDEBUG)
	@rm -f *.d


%.o : %.c
	@echo "[COMPILE] Compiling $<"
	@$(CC) $(CFLAGS) -c $< -o $@

%.o : %.s                                                                        
	@echo "[ASSEMBLE] Assembling $<"                                               
	@$(AS) $(MODEL) $< -o $@

.PHONY : vba
vba : CFLAGS += $(CRELEASE)
vba : $(PROGNAME).gba
	@echo "[EXECUTE] Running Emulator VBA-M"
	@vbam $(VBAOPT) $(PROGNAME).gba > /dev/null 2> /dev/null

.PHONY : mgba
mgba : CFLAGS += $(CRELEASE)
mgba : $(PROGNAME).gba
	@echo "[EXECUTE] Running Emulator VBA-M"
	@mgba $(PROGNAME).gba > /dev/null 2> /dev/null

.PHONY : med
med : CFLAGS += $(CRELEASE)
med : $(PROGNAME).gba
	@echo "[EXECUTE] Running emulator Mednafen"
	@mednafen $(PROGNAME).gba > /dev/null 2>&1

.PHONY : clean
clean :
	@echo "[CLEAN] Removing all compiled files"
	@rm -f *.o *.elf *.gba *.d res/*.o res/*.d

-include $(CFILES:%.c=%.d)
