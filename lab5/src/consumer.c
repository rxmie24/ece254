#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>



int main(int argc, char *argv[]) {

	/* processing inputs */
	int cid = atoi(argv[5]);
	int consumer_count = atoi(argv[4]);
	int number_of_messages = atoi(argv[1]);
	int queue_size = atoi(argv[2]);

	/* opening the message queue for the consumer */

	mqd_t qdes;
	mode_t mode = S_IRUSR | S_IWUSR;
	struct mq_attr attr;

	attr.mq_maxmsg  = queue_size;
	attr.mq_msgsize = sizeof(int);
	attr.mq_flags   = 0;	/* a blocking queue  */

	char *qname = "/mailbox_lab4";

	//open the queue
	qdes  = mq_open(qname, O_RDONLY, mode, &attr);
	if (qdes == -1 ) {
		perror("mq_open()");
		exit(1);
	}


	/* consume elements */

	int i;
	int message_priority = 0;
	while(1){
		int recieved_message;

		//receive message from the queue
		if (mq_receive(qdes, (char *)&recieved_message, sizeof(int), message_priority) == -1){
			printf("mq_receive() failed\n");
			return 1;
		} 

		if (recieved_message >= number_of_messages){
			break;
		} else {
			printf("%d is consumed by %d", recieved_message, cid);
		}
	}

	/* closing the message queue on the consumer side */

	if (mq_close(qdes) == -1) {
		perror("mq_close() failed");
		exit(2);
	}

	return 0;

}