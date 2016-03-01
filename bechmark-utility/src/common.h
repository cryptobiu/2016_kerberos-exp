
#include <stdio.h>
#include <stdlib.h>
#include <krb5.h>
#include <string.h>
#include <pthread.h>

#ifndef COMMON_H__
#define COMMON_H__

struct test_result_t {
	struct timeval stop;
	struct timeval start;
};


struct test_config_t {
	char *password;
	char *service;
	char *user;
	char *tgt;
	int thread;
	int requests;
};


#define USER	 "user@KRB.ZIMMERLE.ORG"
#define TGTNAME	 "krbtgt/KRB.ZIMMERLE.ORG@KRB.ZIMMERLE.ORG"
#define SERVICE  "HTTP/www.example.com@KRB.ZIMMERLE.ORG"
#define PASSWORD "password"

#define AMOUNT_OF_REQUESTS 500
#define NUMBER_OF_TRHEADS 30

struct test_result_t *res;

#endif
