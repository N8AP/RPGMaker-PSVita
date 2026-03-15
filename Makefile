TARGET = final
OBJS = final.o virtual_buttons.o

PREFIX = arm-vita-eabi
CC = $(PREFIX)-gcc
CFLAGS = -Wl,-q -Wall -I.
LIBS = -lSceLibKernel_stub -lSceCtrl_stub -lSceTouch_stub

all: $(TARGET).vpk

$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET).velf: $(TARGET).elf
	vita-elf-create $< $@

eboot.bin: $(TARGET).velf
	vita-make-fself -s $< $@

param.sfo:
	vita-mksfoex -s TITLE_ID="FINA00001" "Final Test" $@

$(TARGET).vpk: eboot.bin param.sfo
	vita-pack-vpk -s param.sfo -b eboot.bin $@

clean:
	rm -f *.o *.elf *.velf eboot.bin param.sfo *.vpk

.PHONY: all clean