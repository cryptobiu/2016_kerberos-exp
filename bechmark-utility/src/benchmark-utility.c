
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include "test.h"
#include "utils.h"
#include "report.h"
#include "common.h"

void print_help() {
	printf("\n");
	printf("benchmark-utility\n");
	printf("-----------------\n");
	printf("\n");
	printf("\t-r n\tAmount of requests\n");
	printf("\t-c n\tAmount of parallel clients\n");
	printf("\t-u\tUser name (e.g. " USER ")\n");
	printf("\t-t\tTGT name (e.g. " TGTNAME ")\n");
	printf("\t-s\tService (e.g. " SERVICE ")\n");
	printf("\t-p\tPassword\n");
	printf("\n");
}


int main (int argc, char **argv) {
	long i = 0;
	int ret = 0;

	int index;
	int c;

	int n_requests = 0;
	int n_threads = 0;
	char *tgt = NULL;
	char *service = NULL;
	char *user = NULL;
	char *password = NULL;

    struct timeval stop;
    struct timeval start;

	opterr = 0;

	while (1) {
		static struct option long_options[] = {
			{"user",	required_argument, 0, 'u'},
			{"password",	required_argument, 0, 'p'},
			{"service",	required_argument, 0, 's'},
			{"tgt",	required_argument, 0, 't'},
			{"clients",	required_argument, 0, 'c'},
			{"request",	required_argument, 0, 'r'},
			{0, 0, 0, 0}
		};

		int option_index = 0;
		c = getopt_long (argc, argv, "r:c:u:t:s:u:p:",
                       long_options, &option_index);


		/* Detect the end of the options. */
		if (c == -1)
			break;

		switch (c) {
      			case 'r':
				n_requests = atoi(optarg);
				break;
      			case 'c':
       				n_threads = atoi(optarg);
				break;
			case 't':
				tgt = optarg;
				break;
			case 's':
				service = optarg;
				break;
			case 'u':
				user = optarg;
				break;
			case 'p':
				password = optarg;
				break;
      			case '?':
        			if (optopt == 'c')
          				fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        			else if (isprint (optopt))
					fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				else
					fprintf (stderr, "Unknown option character `\\x%x'.\n",
						optopt);
				print_help();
        			return 1;

			default:
				print_help();
				abort ();
		}
	}

	for (index = optind; index < argc; index++) {
		printf ("Non-option argument %s\n", argv[index]);
		print_help();
	}

	if (user == NULL ||
		password == NULL ||
		service == NULL ||
		tgt == NULL ||
		n_requests <= 0 ||
		n_threads <= 0) {
		print_help();
		return 1;
	}


	res = malloc((sizeof(struct test_result_t)*n_requests)*n_threads);
	memset(res, 0, (sizeof(struct test_result_t)*n_requests)*n_threads);

	pthread_t *threads;
	threads = malloc(sizeof(struct test_result_t)*n_threads);

	printf("\033[2J");
	printf("\n");
	printf(" Username: %s\n", user);
	printf(" Password: %s\n", password);
	printf("  Service: %s\n", service);
	printf("      TgT: %s\n", tgt);
	printf("\n");

    gettimeofday(&start, NULL);
	for (i = 0; i < n_threads; i++) {
		struct test_config_t *test = malloc(sizeof(struct test_config_t));
		test->user = user;
		test->password = password;
		test->service = service;
		test->tgt = tgt;
		test->thread = i;
		test->requests = n_requests;


		ret = pthread_create(&threads[i], NULL, perform_test, (void *) test);
		if (ret) {
			printf("ERROR; return code from pthread_create() is %d\n", ret);
			return -1;
		}
	}

	for (i = 0; i < n_threads; i++) {
		pthread_join(threads[i], NULL);
	}
    gettimeofday(&stop, NULL);

	print_report(res, n_requests*n_threads, timedifference_msec(&stop, &start));

	free(threads);
	free(res);
	return 0;
failed:
	fprintf(stderr, "Failed.\n");
	return 1;
}

