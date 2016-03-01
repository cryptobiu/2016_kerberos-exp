
#include "test.h"


void *perform_test (void *ttest) {
	struct test_config_t *test = (struct test_config_t *) ttest;

	int id = test->thread;
	int i = 0;
	int offset = id*test->requests;

	for (i = 0; i < test->requests; i++) {
		int ret = 0;
		struct test_result_t *t = &res[offset+i];
		int doff = (int) id * 4;
		if (doff) { doff = doff + 1; }

		gettimeofday(&t->start, NULL);
		ret = retrieve_token(test->user, test->tgt, test->service,
			test->password);
		if (ret) {
			return;
		}
		gettimeofday(&t->stop, NULL);

		printf("\033[10;%dH%02d%% - id:`%d`               ",
			doff, (int) (i * 100 / test->requests, id));

		//printf("\033[11;%dH%s", doff, "....");
		//printf("t:%d - n:%d\n", test->thread, i);
	}
}


int retrieve_token(char *person, char *tgtname,
	char *service, char *password) {
	char *unparsed;
	krb5_principal client, tgtprinc, server;
	krb5_creds tgt, st, *tgts;
	krb5_ticket *ticket;
	krb5_keytab keytab;
	krb5_keytab_entry entry;
	krb5_context context;
	krb5_error_code ret;

	initialize_krb5_error_table();

	ret = krb5_init_context(&context);
	if (ret) {
		fprintf(stderr, "Error in krb5_init_context().\n");
		goto failed;
	}

	ret = krb5_parse_name(context, person, &client);
	if (ret) {
		const char *s = krb5_get_error_message(context, ret);
		fprintf(stderr, "Error in krb5_parse_name() - %s.\n", s);
		goto failed;
	}

	ret = krb5_parse_name(context, tgtname, &tgtprinc);
	if (ret) {
		const char *s = krb5_get_error_message(context, ret);
		fprintf(stderr, "Error in krb5_parse_name() - %s.\n", s);
		goto failed;
	}

	ret = krb5_parse_name(context, service, &server);
	if (ret) {
		const char *s = krb5_get_error_message(context, ret);
		fprintf(stderr, "Error in krb5_parse_name(). - %s.\n", s);
		goto failed;
	}

	memset(&tgt, 0, sizeof(tgt));
	tgt.client = client;
	tgt.server = tgtprinc;

	ret = krb5_get_init_creds_password(context, &tgt, client, password,
					   NULL, NULL, 0, NULL, NULL);
	if (ret) {
		const char *s = krb5_get_error_message(context, ret);
		fprintf(stderr, "Error in get_init_creds() - %s.\n", s);
		goto failed;
	}

	memset(&st, 0, sizeof(st));
	st.client = client;
	st.server = server;

	ret = krb5_get_cred_via_tkt(context, &tgt, 0, NULL, &st, &tgts);
	if (ret) {
		fprintf(stderr, "Error in get_credentials().\n");
		goto failed;
	}

	ret = krb5_unparse_name(context, tgts[0].server, &unparsed);
	if (ret) {
		fprintf(stderr, "Error in unparse_name().\n");
		goto failed;
	}

	ret = krb5_kt_resolve(context, "/etc/krb5.keytab", &keytab);
	if (ret) {
		fprintf(stderr, "Error in kt_resolve()");
		goto failed;
	}

	ret = krb5_decode_ticket(&tgts[0].ticket, &ticket);
	if (ret) {
		fprintf(stderr, "Error in decode_ticket().\n");
	}

	ret = krb5_kt_get_entry(context, keytab, server, ticket->enc_part.kvno,
		       		ticket->enc_part.enctype, &entry);
	if (ret) {
		fprintf(stderr, "Error in get_entry().\n");
		goto failed;
	}

#ifdef BREAK_VALIDATION
	entry.key.contents[0] = 5;
#endif
	ret = krb5_decrypt_tkt_part(context, &entry.key, ticket);
	if (ret) {
		fprintf(stderr, "Error in decrypt()\n");
	}

	return 0;
failed:
	return 1;
}

