
#include "report.h"
#include "test.h"

int print_report(struct test_result_t *res, size_t n) {
	float fast = 0;
	float slow = 0;
	float total = 0;
	int i = 0;

	for (i = 0; i < n; i++) {
		struct test_result_t *t = &res[i];
		float delta = timedifference_msec(t->start, t->stop);
		if (delta < fast || fast == 0) {
			fast = delta;
		}
		if (delta > slow) {
			slow = delta;
		}
		total = total + delta;
	}

	fprintf(stdout, "Slow	: %fms\n", slow);
	fprintf(stdout, "Fast	: %fms\n", fast);
	fprintf(stdout, "Total  : %fms\n", total);
	fprintf(stdout, "# reqs : %d requests\n", n);

	fprintf(stdout, "\n");
}

