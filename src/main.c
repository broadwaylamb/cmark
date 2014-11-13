#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "cmark.h"

void print_usage()
{
	printf("Usage:   cmark [FILE*]\n");
	printf("Options: --help, -h    Print usage information\n");
	printf("         --ast         Print AST instead of HTML\n");
	printf("         --version     Print version\n");
}

static void print_document(node_block *document, bool ast)
{
	strbuf html = GH_BUF_INIT;

	if (ast) {
		cmark_debug_print(document);
	} else {
		cmark_render_html(&html, document);
		printf("%s", html.ptr);
		strbuf_free(&html);
	}
}

int main(int argc, char *argv[])
{
	int i, numfps = 0;
	bool ast = false;
	int files[argc];
	node_block *document = NULL;

	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "--version") == 0) {
			printf("cmark %s", VERSION);
			printf(" - CommonMark converter (c) 2014 John MacFarlane\n");
			exit(0);
		} else if ((strcmp(argv[i], "--help") == 0) ||
			   (strcmp(argv[i], "-h") == 0)) {
			print_usage();
			exit(0);
		} else if (strcmp(argv[i], "--ast") == 0) {
			ast = true;
		} else if (*argv[i] == '-') {
			print_usage();
			exit(1);
		} else { // treat as file argument
			files[numfps++] = i;
		}
	}

	if (numfps == 0) {
		document = cmark_parse_file(stdin);
		print_document(document, ast);
		cmark_free_blocks(document);
	} else {
		for (i = 0; i < numfps; i++) {
			FILE *fp = fopen(argv[files[i]], "r");

			if (fp == NULL) {
				fprintf(stderr, "Error opening file %s: %s\n",
					argv[files[i]], strerror(errno));
				exit(1);
			}

			document = cmark_parse_file(fp);
			print_document(document, ast);
			cmark_free_blocks(document);
			fclose(fp);
		}
	}

	return 0;
}
