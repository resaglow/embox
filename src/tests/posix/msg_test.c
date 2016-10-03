/**
 * @file Messaging testsuite
 *
 * @date Oct 3, 2016
 * @author: Artem Lobanov
 */

#include <stdlib.h>    /* exit */
#include <string.h>    /* strlen */
#include <pthread.h>
#include <kernel/thread/thread_msg.h>

#include <kernel/task.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("messaging testsuite");

static volatile int senderror = -1;

static void *thread1(void *arg) {
    const char *str = "teststr";
    msg_send(0, (char *)str, strlen(str));

    return NULL;
}

static void *thread2(void *arg) {
    char *str = malloc(10);
    msg_receive(0, (char *)str, 10);

    if (strcmp(str, "teststr")) {
        senderror = 1;
    } else {
        senderror = 0;
    }

    free(str);

    return NULL;
}

static void test_msg_basic(void) {
	pthread_t thread_1, thread_2;

    pthread_create(&thread_1, NULL, thread1, NULL);
    pthread_create(&thread_2, NULL, thread2, NULL);

    pthread_join(thread_1, (void **)NULL);
    pthread_join(thread_2, (void **)NULL);
}

TEST_CASE("messaging system should pass sanity checks") {
    test_msg_basic();

    test_assert_zero(senderror);
}
