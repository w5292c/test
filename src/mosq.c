#include "mosq.h"

#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <mosquitto.h>

#ifdef MOSQ_MAIN
int main(int argc, char **argv)
{
  (void)argc;
  (void)argv;
  test_mosq();
  return 0;
}
#endif /* MOSQ_MAIN */

static bool quit_request = false;

static void signal_handler(int signum);
static void on_message(void *user_ptr, const struct mosquitto_message *message);

void test_mosq(void)
{
  int major = 0;
  int minor = 0;
  int revision = 0;

  struct sigaction handlers;
  memset(&handlers, 0, sizeof (struct sigaction));
  handlers.sa_handler = signal_handler;
  const int result = sigaction(SIGUSR1, &handlers, 0);
  if (result) {
    printf("Error: failed installing signal handler, code: %d\n", result);
  }

  mosquitto_lib_version(&major, &minor, &revision);
  printf("Version: %d.%d.%d\n", major, minor, revision);

  if (MOSQ_ERR_SUCCESS != mosquitto_lib_init()) {
    fprintf(stderr, "Error: mosq init failed\n");
    return;
  }

  struct mosquitto *mosquitto = mosquitto_new("eee5bb237f8a16d61621036f2edb246c8ec94480", true, NULL);
  if (!mosquitto) {
    fprintf(stderr, "Error: failed creating mosquitto instance\n");
    goto cleanup;
  }

  const int connect_result = mosquitto_connect(mosquitto, "localhost", 1234, false);
  fprintf(stdout, "Connected: %d\n", connect_result);
  if (MOSQ_ERR_SUCCESS != connect_result) {
    fprintf(stderr, "Error: %s\n", strerror(errno));
    goto cleanup;
  }

  if (MOSQ_ERR_SUCCESS != mosquitto_subscribe(mosquitto, NULL, "com.test.w5292c", 0)) {
    fprintf(stderr, "Error: cannot subscribe\n");
    goto cleanup;
  }

  mosquitto_message_callback_set(mosquitto, on_message);

  while (!quit_request) {
    const int loop_result = mosquitto_loop(mosquitto, 500, 10);
    if (MOSQ_ERR_SUCCESS != loop_result) {
      fprintf(stderr, "Error: loop failed, code: %d\n", loop_result);
      break;
    }
  }

cleanup:
  /* Clean-up */
  if (mosquitto) {
    if (MOSQ_ERR_SUCCESS != mosquitto_unsubscribe(mosquitto, NULL, "com.test.w5292c")) {
      fprintf(stderr, "Error: cannot unsubscribe\n");
    }
    if (MOSQ_ERR_SUCCESS != mosquitto_disconnect(mosquitto)) {
      fprintf(stderr, "Error: cannot disconnect\n");
    }

    mosquitto_destroy(mosquitto);
  }

  if (MOSQ_ERR_SUCCESS != mosquitto_lib_cleanup()) {
    fprintf(stderr, "Error: mosq clean-up failed\n");
  }
}

void signal_handler(int signum)
{
  (void)signum;
  quit_request = true;
}

void on_message(void *user_ptr, const struct mosquitto_message *message)
{
  (void)user_ptr;

  fprintf(stdout, "New message:\n");
  fprintf(stdout, "- mid: %u\n", message->mid);
  fprintf(stdout, "- topic: \"%s\"\n", message->topic);
  fprintf(stdout, "- topic-length: %u: \"%s\"\n", message->payloadlen, message->payload);
  fprintf(stdout, "- mid: %d, retain: %d\n", message->qos, message->retain);
}
