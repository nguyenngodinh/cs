/*
 * Naive sorting of big file.
 *
 * This program reads whole file to array held in memory
 * and then invokes qsort on this array.
 *
 * As bonus feature this program implements 
 * dynamic resizing array in `readfile` function.
 *
 * This is a baseline for testing performance and 
 * resourse usage in external sorting experiments.
 *
 * Copyright (c) 2014 Alex Dzyoba <avd@reduct.ru>
 * 
 * This project is licensed under the terms of the MIT license
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>


// Increasing coefficient used in dynamic resizing.
#define INC_COEFF 2

void print_arr(int A[], int n)
{
	int i;
	for (i = 0; i < n; i++)
		printf("%d\n", A[i]);
}

int readfile(int *array[], const char *filename)
{
	FILE *fp;
	int *A;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	int limit, i, d;

	fp = fopen(filename, "r");
	if (fp == NULL) {
		fprintf(stderr, "Failed to open file %s\n", filename);
		return -1;
	}

	limit = 10;
	A = calloc(limit, sizeof(int));
	if (!A) {
		perror("calloc");
		goto err_close;
	}

	i = 0;
	while ((read = getline(&line, &len, fp)) != -1) 
	{
		errno = 0;
		d = strtol(line, NULL, 0);
		if (errno) {
			perror("strtol");
			goto err_free;
		}

		A[i++] = d;

		// Expand array
		if (i == limit) {
			limit *= INC_COEFF;
			A = realloc(A, limit * sizeof(int));
		}
	}

	// Truncate back
	A = realloc(A, i * sizeof(int));
	fclose(fp);
	free(line);

	*array = A;

	return i;

err_free:
	if (A)
		free(A);

err_close:
	fclose(fp);
	return -1;
}

int compar(const void *p1, const void *p2)
{
	int *i1, *i2;

	i1 = (int *)p1;
	i2 = (int *)p2;

	return *i1 - *i2;
}

int main(int argc, const char *argv[])
{
	int *A;
	int n;

	if (argc < 2) {
		fprintf(stderr, "Usage: %s <file to sort>\n", argv[0]);
		return EXIT_FAILURE;
	}

	n = readfile(&A, argv[1]);
	if (n < 0) {
		fprintf(stderr, "Failed to parse file\n");
		return -1;
	}

	fprintf(stderr, "File read to array of size %d\n", n * sizeof(int));
	
	qsort(A, n, sizeof(int), compar);

	print_arr(A, n);
	
	free(A);
	return 0;
}