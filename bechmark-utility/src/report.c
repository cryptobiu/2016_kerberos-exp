
#include "report.h"
#include "test.h"

int print_report(struct test_result_t *res, size_t n, float total_time) {
	float fast = 0;
	float slow = 0;
	float total = 0;
	int i = 0;
    float seconds;

	for (i = 0; i < n; i++) {
		struct test_result_t *t = &res[i];
		float delta = timedifference_msec(&t->stop, &t->start);
		if (delta < fast || fast == 0) {
			fast = delta;
		}
		if (delta > slow) {
			slow = delta;
		}
	}

    for (i = 0; i < 10; i++)
        fprintf(stdout, "\n");

    seconds = total_time / 1000000;
	fprintf(stdout, "Slow	: %fmicroseconds\n", slow);
	fprintf(stdout, "Fast	: %fmicroseconds\n", fast);
	fprintf(stdout, "Total  : %fmicroseconds\n", total_time);
	fprintf(stdout, "Total  : %f seconds\n", seconds);
	fprintf(stdout, "# reqs : %d requests\n", n);
	fprintf(stdout, "# reqs per second : %f requests\n", (float) n / seconds);

    fprintf(stdout, "\n");
}

