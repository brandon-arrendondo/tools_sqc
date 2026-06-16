/* Reproduces the lib/srv_mosq.c ares_parse_srv_reply leak (upstream PR 02).
 *
 * Requires a resolvable SRV record at _mqtt._tcp.<domain>. The SRV target
 * host/port need not accept a connection — the leak (the unfreed
 * struct ares_srv_reply list) happens in srv_callback() at parse time,
 * before/regardless of the subsequent mosquitto_connect().
 *
 * Build against a WITH_SRV libmosquitto:
 *   gcc srv_leak_repro.c \
 *       -I ~/data-enterprise/mosquitto-main/include \
 *       -L /tmp/moz-build/lib -lmosquitto -o srv_leak_repro
 *
 * Run under valgrind:
 *   LD_LIBRARY_PATH=/tmp/moz-build/lib \
 *   valgrind --leak-check=full --show-leak-kinds=all ./srv_leak_repro <domain>
 */
#include <mosquitto.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	if(argc < 2){
		fprintf(stderr, "usage: %s <domain-with-_mqtt._tcp-SRV>\n", argv[0]);
		return 2;
	}
	mosquitto_lib_init();
	struct mosquitto *m = mosquitto_new(NULL, true, NULL);
	if(!m){ fprintf(stderr, "mosquitto_new failed\n"); return 1; }

	int rc = mosquitto_connect_srv(m, argv[1], 60, NULL);
	fprintf(stderr, "mosquitto_connect_srv(%s) rc=%d (%s)\n",
	        argv[1], rc, mosquitto_strerror(rc));

	/* Pump the loop so the c-ares SRV resolution completes and srv_callback
	 * fires (this is where reply is allocated and leaked). The connection
	 * attempt afterwards may fail; that does not affect the leak. */
	for(int i = 0; i < 100; i++){
		mosquitto_loop(m, 100, 1);
	}

	mosquitto_destroy(m);
	mosquitto_lib_cleanup();
	return 0;
}
