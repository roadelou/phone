p_src := phone.c phone_sub.c
p_head := phone.h
l_src := log.c
l_head := log.h
src := $(p_src) $(l_src)
head := $(p_head) $(l_head)
out := phone.elf

.phony: clean

$(out): $(src) $(head)
	clang $(src) -Wall -pedantic -o $(out)

clean:
	rm -f $(out)
