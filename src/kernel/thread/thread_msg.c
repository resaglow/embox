/**
 * @file Interthread messaging
 *
 * @date   03.10.2016
 * @author Artem Lobanov
 */

#include <kernel/thread/thread_msg.h>

struct mq_info {
    char *msg_ptr;
    size_t msg_size;

    volatile int was_written;

    pthread_mutex_t msg_snd_mutex;
    pthread_cond_t msg_snd_cv;

    pthread_mutex_t msg_rcv_mutex;
    pthread_cond_t msg_rcv_cv;
};

static struct mq_info msg_table[MQ_MAXCOUNT];

int msg_send(int mq_id, char *msg, int size) {
    if (mq_id > sizeof(msg_table) / sizeof(msg_table[0])) {
        return -1;
    }

    pthread_mutex_lock(&msg_table[mq_id].msg_rcv_mutex);

    if (msg_table[mq_id].was_written) {
        pthread_cond_wait(&msg_table[mq_id].msg_rcv_cv, &msg_table[mq_id].msg_rcv_mutex);
    }

    msg_table[mq_id].msg_ptr = sysmalloc(size + 1);
    strncpy(msg_table[mq_id].msg_ptr, msg, size);
    msg_table[mq_id].msg_ptr[size] = '\0';

    msg_table[mq_id].msg_size = size + 1;

    msg_table[mq_id].was_written = 1;

    pthread_mutex_unlock(&msg_table[mq_id].msg_rcv_mutex);

    pthread_cond_broadcast(&msg_table[mq_id].msg_snd_cv);

    return 0;
}

int msg_receive(int mq_id, char *msg, int size) {
    if (mq_id > sizeof(msg_table) / sizeof(msg_table[0])) {
        return -1;
    }

    pthread_mutex_lock(&msg_table[mq_id].msg_snd_mutex);

    if (!msg_table[mq_id].was_written) {
        pthread_cond_wait(&msg_table[mq_id].msg_snd_cv, &msg_table[mq_id].msg_snd_mutex);
    }

    strncpy(msg, msg_table[mq_id].msg_ptr, size < msg_table[mq_id].msg_size ? size : msg_table[mq_id].msg_size);
    msg[size - 1] = '\0';

    sysfree(msg_table[mq_id].msg_ptr);

    msg_table[mq_id].was_written = 0;

    pthread_mutex_unlock(&msg_table[mq_id].msg_snd_mutex);

    pthread_cond_broadcast(&msg_table[mq_id].msg_rcv_cv);

    return 0;
}
