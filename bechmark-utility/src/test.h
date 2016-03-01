
#include <sys/time.h>

#include "common.h"

#ifndef TEST_H__
#define TEST_H__

void *perform_test (void *tid);
int retrieve_token(char *person, char *tgtname, char *service, char *password);

#endif
