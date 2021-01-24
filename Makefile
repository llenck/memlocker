CFLAGS ?= -Wall -Wextra -s

.PHONY: clean uninstall

memlocker: memlocker.c
	$(CC) $(CFLAGS) $< -o $@

install: memlocker
	install -m755 memlocker /bin/memlocker

uninstall:
	$(RM) /bin/memlocker

clean:
	$(RM) memlocker
