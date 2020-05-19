p_src := phone.c phone_sub.c
p_head := phone.h
l_src := log.c
l_head := log.h
src := $(p_src) $(l_src)
head := $(p_head) $(l_head)
out := phone.elf
release := phone

.phony: clean install

$(out): $(src) $(head)
	clang $(src) -Wall -pedantic -o $(out)

$(release): $(src) $(head)
	clang $(src) -D PHONE_RELEASE -Wall -pedantic -o $(release)

install: $(release)
	cp $(release) $(HOME)/localApp/phone

clean:
	rm -f $(out) $(release)
