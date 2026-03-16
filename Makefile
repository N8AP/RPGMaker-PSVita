all: final gfx minimal simple

final:
	$(MAKE) -f Makefile.final

gfx:
	$(MAKE) -f Makefile.gfx

minimal:
	$(MAKE) -f Makefile.minimal

simple:
	$(MAKE) -f Makefile.simple

clean:
	$(MAKE) -f Makefile.final clean
	$(MAKE) -f Makefile.gfx clean
	$(MAKE) -f Makefile.minimal clean
	$(MAKE) -f Makefile.simple clean
	rm -f *.o *.elf *.velf eboot.bin param.sfo *.vpk

.PHONY: all final gfx minimal simple clean