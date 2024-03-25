#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define _STR(x) #x
#define STR(x) _STR(x)
#define STREQ(s1, s2) (strcmp(s1, s2) == 0)
#define errorf(...) fprintf(stderr, __VA_ARGS__)
#ifndef FONT_SIZE_PX
#define FONT_SIZE_PX 24
#endif
#ifndef PROGNAME
#define PROGNAME argv[0]
#endif
#define BMP_FILENAME "banner.bmp"

static inline void print_usage(const char* progname, int enable_font_size, int enable_outfile)
{
	printf("Usage: %s [text]", progname);
	if (enable_font_size)
		printf(" [--font-size px]");
	if (enable_outfile)
		printf(" [--out filename]");
	printf("\n");
	printf("  [text]: the text to be drawn. Reads from stdin if not specified.\n");
	if (enable_font_size)
		printf("  --font-size: font size in pixels, defaults to %d.\n", FONT_SIZE_PX);
	if (enable_outfile)
		printf("  --out: the output bmp file's name, defaults to %s.\n", BMP_FILENAME);
	printf("  --help: print this message.\n\n");
}

/* parses command line arguments.
 * set font_size parameter to NULL disables `--font-size` command line argument, and same for outfile.
 * calls exit() if any of the args are invalid.
 */
static inline void parse_args(int argc, char *argv[], char *text, int textlen, int *font_size, char *outfile, int outfilelen)
{
	text[0] = '\0';
	if (font_size)
		*font_size = FONT_SIZE_PX;
	if (outfile)
		strncpy(outfile, BMP_FILENAME, outfilelen);

	for (int i = 1; i < argc; i++) {
		if (font_size && STREQ(argv[i], "--font-size")) {
			char* endptr;
			long px;
			if (i + 1 == argc) {
				errorf("missing argument for option `--font-size`.\n");
				exit(EXIT_FAILURE);
			}
			px = strtol(argv[i + 1], &endptr, 10);
			if (errno == ERANGE || *endptr != '\0') {
				errorf("invalid argument for option `--font-size`: %s\n", argv[i + 1]);
				exit(EXIT_FAILURE);
			}
			*font_size = (int)px;
			i++;
		}
		else if (outfile && STREQ(argv[i], "--out")) {
			if (i + 1 == argc) {
				errorf("missing argument for option `--out`.\n");
				exit(EXIT_FAILURE);
			}
			strncpy(outfile, argv[i + 1], outfilelen);
			i++;
		}
		else if (STREQ(argv[i], "--help")) {
			print_usage(PROGNAME, font_size != NULL, outfile != NULL);
			exit(0);
		}
		else if (i == 1) {
			strncpy(text, argv[i], textlen);
		}
		else {
			errorf("unknown option: %s\n", argv[i]);
			print_usage(PROGNAME, font_size != NULL, outfile != NULL);
			exit(EXIT_FAILURE);
		}
	}
}

#endif