#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

const char metric_pref_pot[] = { 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, -1, -1, 60, -1, 30, -1, -1, -1, 10, -1, 20, -1, -1, 50, -1, -1, -1, 40, -1, -1, -1, -1, 80, 70, -1, -1, -1, -1, -1, -1, -1, 0, -1, -1, 60, -1, 30, -1, -1, -1, 10, -1, 20, -1, -1, 50, -1, -1, -1, 40, -1, -1, -1, -1, 80, 70, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

const char dec_digits[] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

size_t parse_len(unsigned const char* str) {
	size_t r = 0;
	while (*str) {
		char x = dec_digits[*str];
		if (x < 0) break;
		r = r * 10 + x;
		str++;
	}

	char x = metric_pref_pot[*str];
	if (x < 0) return 0;

	return r << x;
}

int cont = 1;

void noop(int s) {
	(void)s;
	cont = 0;
}

void fault_pages(char* pages, size_t n, int page_sz) {
	while (cont) {
		for (size_t off = 0; off < n && cont; off += page_sz) {
			pages[off] += 5;
		}

		// don't stress the cpu too much, especially for small mappings
		struct timespec ts = { .tv_sec = 0, .tv_nsec = 10000 };
		nanosleep(&ts, NULL);
	}
}

int main(int argc, char** argv) {
	size_t n;
	if (argc != 2 || (n = parse_len((unsigned char*)argv[1])) == 0) {
		fprintf(stderr,
				"Usage: %s <AMOUNT>\n"
				"\n"
				"Allocate and lock AMOUNT memory, rounded down to the next page size,\n"
				"until SIGINT, SIGTERM or SIGHUP is sent. If memory can't be locked,\n"
				"continuously fault all allocated pages.\n"
				"\n"
				"The following units are supported:\n"
				"\tnone/b/B for bytes\n"
				"\tk/K for 2^10 bytes\n"
				"\tm/M for 2^20 bytes\n"
				"\tg/G for 2^30 bytes\n"
				"\tt/T for 2^40 bytes\n"
				"\tp/P for 2^50 bytes\n"
				"\te/E for 2^60 bytes\n"
				"\tz/Z for 2^70 bytes (UB on systems with 64-bit size_t)\n"
				"\ty/Y for 2^80 bytes (UB on systems with 64-bit size_t)\n",
				argv[0]);

		return 1;
	}

	int page_sz = sysconf(_SC_PAGESIZE);
	if (page_sz < 0) page_sz = 4096;

	// round down
	n = n / page_sz * page_sz;

	void* map = mmap(NULL, n, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (map == MAP_FAILED) {
		perror("Couldn't allocate memory");
		return 1;
	}

	signal(SIGINT, noop);
	signal(SIGTERM, noop);
	signal(SIGHUP, noop);

	if (mlock(map, n) == 0) {
		// just wait for some signal
		pause();
	}
	else {
		// if we couldn't lock the pages, fault them continuously
		fault_pages(map, n, page_sz);
	}

	munlock(map, n);
	munmap(map, n);

	return 0;
}
