TESTEVAL = ./testeval
ASM      = ../src/assemble

.PHONY: all clean

all: raytracer.out

compiled.s: prelude.mu compiler.mu raytracer.mu
	echo ":quit" | $(TESTEVAL) $^
raytracer.out: raytracer.s fixed_fast.s fixed_precise.s compiled.s
	$(ASM) $< $@

clean:
#	rm -f compiled.s
	rm -f raytracer.out
