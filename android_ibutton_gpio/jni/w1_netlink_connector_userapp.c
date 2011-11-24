

#include <stdint.h>
//#include <sys/cdefs.h>
#include <sys/types.h>		//must
#include <sys/socket.h>		//must

#include <pthread.h>

#include "w1_netlink_userspace.h"


#define MAX_MSGSIZE 1024


#define MIN_MSGSIZE (sizeof(struct nlmsghdr) + sizeof(struct cn_msg) 	\
	+ sizeof(struct w1_netlink_msg) + sizeof(struct w1_netlink_cmd))



static int w1Socket;						//SOCKET
struct sockaddr_nl bindAddr, dataAddr;		//socket address

static struct msghdr socketMsgSend;			//socket message header, for sending
static struct msghdr socketMsgRecv;			//socket message header, for receiving
static struct iovec iovSend;				//data storage structure for I/O using uio(Userspace I/O)
static struct iovec iovRecv;				//data storage structure for I/O using uio(Userspace I/O)
static struct nlmsghdr * nlMsgSend = NULL;	//netlink message header, for sending
static struct nlmsghdr * nlMsgRecv = NULL;	//netlink message header, for receiving


static struct pthread_t receivingThread;
static int receivingThreadStopFlag = 0;

int send_w1_msg(cn_msg * cmsg)
{
	struct w1_netlink_msg * msg = (struct w1_netlink_msg *)(cmsg + 1);
	struct w1_netlink_cmd * cmd = (struct w1_netlink_cmd *)(msg + 1);

	__u32 realSize = sizeof(struct nlmsghdr) + sizeof(struct cn_msg)
			+ sizeof(struct w1_netlink_msg) + sizeof(struct w1_netlink_cmd) + cmd->len;

	memset(nlMsgSend, 0, sizeof(NLMSG_SPACE(MAX_MSGSIZE)));
	memset(&iovSend, 0, sizeof(struct iovec));
	memset(&socketMsgSend, 0, sizeof(struct msghdr));

	memset(NLMSG_DATA(nlhdr), cmsg, realSize);

	nlMsgSend->nlmsg_len = realSize;
	nlMsgSend->nlmsg_pid = getpid();
	nlMsgSend->nlmsg_flags = 0;
	nlMsgSend->nlmsg_type = NLMSG_DONE;
	nlMsgSend->nlmsg_seq = 0;

//	cnmsg->id.idx = CN_IDX_PROC;
//	cnmsg->id.val = CN_VAL_PROC;
//	cnmsg->seq = 0;
//	cnmsg->ack = 0;
//	cnmsg->len = sizeof(enum proc_cn_mcast_op);

	iovSend.iov_base = (void *)nlMsgSend;
	iovSend.iov_len = nlMsgSend->nlmsg_len;

	socketMsgSend.msg_name = (void *)&dataAddr;
	socketMsgSend.msg_namelen = sizeof(dataAddr);
	socketMsgSend.msg_iov = &iovSend;
	socketMsgSend.msg_iovlen = 1;

	return sendmsg(w1Socket, &socketMsgSend, 0);
}



void on_w1_msg_received(cn_msg * cmsg)
{
	struct w1_netlink_msg * msg = (struct w1_netlink_msg *)(cmsg + 1);
	struct w1_netlink_cmd * cmd = (struct w1_netlink_cmd *)(msg + 1);

	switch(cmd->cmd)
	{
		case W1_CMD_ALARM_SEARCH:
		case W1_CMD_SEARCH:

			u64 * slave_rn;
			int count = cmd->len / sizeof(u64);

			while(count-- > 0)
			{
				slave_rn = (u64 *) cmd->data;
				printf("A w1 salve device found: %d \n", *slave_rn);
				slave_rn++;
			}
			break;
	}

}



int retrieve_socket_msg(void)
{
	memset(nlMsgRecv, 0, NLMSG_SPACE(MAX_MSGSIZE));
	memset(&iovRecv, 0, sizeof(struct iovec));
	memset(&socketMsgRecv, 0, sizeof(struct msghdr));

	iovRecv.iov_base = (void *)nlMsgRecv;
	iovRecv.iov_len = NLMSG_SPACE(MAX_MSGSIZE);

	socketMsgRecv.msg_name = (void *)&dataAddr;
	socketMsgRecv.msg_namelen = sizeof(dataAddr);
	socketMsgRecv.msg_iov = &iov;
	socketMsgRecv.msg_iovlen = 1;

	//This call return the number of bytes received, or -1 if an error occurred.
	//The return value will be 0 when the peer has performed an orderly shutdown.

	//If no messages are available at the socket, the receive calls wait for a message to arrive,
	//unless the socket is nonblocking (see fcntl(2)), in which case the value -1 is returned
	//and the external variable errno is set to EAGAIN or EWOULDBLOCK.
	//The receive calls normally return any data available, up to the requested amount,
	//rather than waiting for receipt of the full amount requested.

	int ret = recvmsg(w1Socket, &socketMsgRecv, 0);
	if(0 == ret)
		return E_SOCKET_PEER_GONE;
	else if(-1 == ret)
		return E_SOCKET_CANNOT_RECV;
	else
		return ret;	//return ssize_t
}


void init_w1_msg(cn_msg * cnmsg)
{

	struct cb_id w1_id = {.idx = CN_W1_IDX, .val = CN_W1_VAL};

    cnmsg->id.idx = CN_W1_IDX;
    cnmsg->id.val = CN_W1_VAL;
    cnmsg->seq = 0;
    cnmsg->ack = 0;
}


void socketmsg_receiving_loop(void * param)
{
	int ret;

	printf("w1 socketmsg receiving thread started!\n");

	while(!receivingThreadStopFlag)
	{
		ret = retrieve_socket_msg(&socketMsgRecv);

		if (E_SOCKET_PEER_GONE == ret) {
			printf("System error, socket peer is gone, application exit.\n");
			exit(0);
		}
		else if (E_SOCKET_CANNOT_RECV == ret) {
			perror("recvmsg error...");
			exit(1);
		}
		else
		{
			//base on the ret size...
			on_w1_msg_received((struct cn_msg *)NLMSG_DATA(nlhdr));
		}
	}

	printf("w1 socketmsg receiving thread stopped!\n");
}


void start_receiving_thread(void)
{
	receivingThreadStopFlag = 0;
	//Unless we need to use the 3rd argument in the callback, the third argument can be NULL
	pthread_create(&receivingThread, socketmsg_receiving_loop, NULL);
}

void stop_receiving_thread(void)
{
	receivingThreadStopFlag = 1;
}


int main(void)
{
	const int group = W1_GROUP;

	//open socket
	w1Socket = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_CONNECTOR);

	bindAddr.nl_family = AF_NETLINK;
	bindAddr.nl_groups = group;
	bindAddr.nl_pid = getpid();

	dataAddr.nl_family = AF_NETLINK;
	dataAddr.nl_groups = group;
	dataAddr.nl_pid = 0;

	//bind socket
	if (bind(w1Socket, (struct sockaddr *)&bindAddr, sizeof(struct sockaddr_nl)) == -1)
	{
		perror("bind");

		//close socket
		close(w1Socket);
		return -1;
	}

	//Add membership to W1 Group. Or, you cannot send any message.
	if (setsockopt(s, SOL_NETLINK, NETLINK_ADD_MEMBERSHIP, &group, sizeof(group)) < 0)
	{
		perror("setsockopt");
		exit(-1);
	}

	//Remove membership when you don't want to use netlink
	//setsockopt(s, SOL_NETLINK, NETLINK_DROP_MEMBERSHIP, &group, sizeof(group));


	//init socket messages
	nlMsgSend = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_MSGSIZE));
	nlMsgRecv = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_MSGSIZE));
	if (nlMsgSend == NULL || nlMsgRecv == NULL)
	{
		perror("Cannot allocate memory for netlink message header!");
		exit(-1);
	}

	memset(nlMsgSend, 0, NLMSG_SPACE(MAX_MSGSIZE));
	memset(nlMsgRecv, 0, NLMSG_SPACE(MAX_MSGSIZE));




	/*
	struct cn_msg * cmsg;
	struct w1_netlink_msg * msg;
	struct w1_netlink_cmd * cmd;
	int ret;

	while (1)
	{
		ret = retrieve_socket_msg(&socketMsgRecv);

		if (ret == 0) {
			printf("Exit.\n");
			exit(0);
		}
		else if (ret == -1) {
			perror("recvmsg:");
			exit(1);
		}
		else
		{
			on_w1_msg_received((struct cn_msg *)NLMSG_DATA(nlhdr));
		}
	}
	*/

}


