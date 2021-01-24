A small tool useful for testing OOM situations.

```
Usage: memlocker <AMOUNT>

Allocate and lock AMOUNT memory, rounded down to the next page size,
until SIGINT, SIGTERM or SIGHUP is sent. If memory can't be locked,
continuously fault all allocated pages.

The following units are supported:
	none/b/B for bytes
	k/K for 2^10 bytes
	m/M for 2^20 bytes
	g/G for 2^30 bytes
	t/T for 2^40 bytes
	p/P for 2^50 bytes
	e/E for 2^60 bytes
	z/Z for 2^70 bytes (UB on systems with 64-bit size_t)
	y/Y for 2^80 bytes (UB on systems with 64-bit size_t)
```

This tool can be built using `make` and installed/uninstalled with `make install`/`make uninstall`.
