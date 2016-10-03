/**
 *
 * @date 03.10.2016
 * @author Artem Lobanov
 */

#ifndef MSG_H_
#define MSG_H_

#include <pthread.h>
#include <string.h>
#include <mem/sysmalloc.h>

#define MQ_MAXCOUNT 100

int msg_send(int mq_id, char *msg, int size);

int msg_receive(int mq_id, char *msg, int size);

#endif /* MSG_H_ */
