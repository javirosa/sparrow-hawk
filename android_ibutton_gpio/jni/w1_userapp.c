/*
 * w1_userapp.c
 *
 *  Created on: 2011-11-9
 *      Author: deven
 */


#include <sys/types.h>
#include <sys/socket.h>

//#include <signal.h>
//#include <linux/socket.h>
#include <linux/netlink.h>
#include <linux/connector.h>

#include "w1_netlink.h"



#define MAX_MSGSIZE 256

#define MIN_MSGSIZE (sizeof(struct cn_msg) + sizeof(struct w1_netlink_msg) + sizeof(struct w1_netlink_cmd))



static int sd;								//SOCKET
struct sockaddr_nl addrLocal, addrRemote;	//socket address
static struct nlmsghdr * nlMsgSend = NULL;	//netlink message header, for sending
static struct nlmsghdr * nlMsgRecv = NULL;	//netlink message header, for receiving
static struct msghdr socketMsgSend;			//socket message header, for sending
static struct msghdr socketMsgRecv;			//socket message header, for receiving
static struct iovec iovSend;				//data storage structure for I/O using uio(Userspace I/O)
static struct iovec iovRecv;				//data storage structure for I/O using uio(Userspace I/O)
//struct cn_msg * cnmsg;




int send_w1_msg(cn_msg * cmsg)
{
	struct w1_netlink_msg * msg = (struct w1_netlink_msg *)(cmsg + 1);
	struct w1_netlink_cmd * cmd = (struct w1_netlink_cmd *)(msg + 1);

	__u32 realSize = sizeof(struct cn_msg) + sizeof(struct w1_netlink_msg)
			+ sizeof(struct w1_netlink_cmd) + cmd->len;

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

	socketMsgSend.msg_name = (void *)&addrRemote;
	socketMsgSend.msg_namelen = sizeof(addrRemote);
	socketMsgSend.msg_iov = &iovSend;
	socketMsgSend.msg_iovlen = 1;

	return sendmsg(sd, &socketMsgSend, 0);
//	if (ret == -1)
//	{
//		perror("sendmsg error:");
//		exit(-1);
//	}
}


int retrieve_w1_msg(cn_msg * cmsg)
{
	memset(nlMsgRecv, 0, NLMSG_SPACE(MAX_MSGSIZE));
	memset(&iovRecv, 0, sizeof(struct iovec));
	memset(&socketMsgRecv, 0, sizeof(struct msghdr));

	iovRecv.iov_base = (void *)nlMsgRecv;
	iovRecv.iov_len = NLMSG_SPACE(MAX_MSGSIZE);

	socketMsgRecv.msg_name = (void *)&addrRemote;
	socketMsgRecv.msg_namelen = sizeof(addrRemote);
	socketMsgRecv.msg_iov = &iov;
	socketMsgRecv.msg_iovlen = 1;

	return recvmsg(sd, &socketMsgRecv, 0);
}


void on_w1_msg_received(cn_msg * cmsg)
{
	struct w1_netlink_msg * msg = (struct w1_netlink_msg *)(cmsg + 1);
	struct w1_netlink_cmd * cmd = (struct w1_netlink_cmd *)(msg + 1);

	u64 slave_rn;

	switch(cmd->cmd)
	{
		case W1_CMD_ALARM_SEARCH:
		case W1_CMD_SEARCH:

			if(cmd->len == sizeof(u64))
			{
				slave_rn = (u64 *) cmd->data;
				printf("A w1 salve device found: %d \n", slave_rn);
			}
			else
				perror("Wrong data in the w1 search slave message!\n");
			break;


	}

}




void build_w1_msg(cn_msg * cnmsg)
{

	struct cb_id w1_id = {.idx = CN_W1_IDX, .val = CN_W1_VAL};

    cnmsg->id.idx = CN_W1_IDX;
    cnmsg->id.val = CN_W1_VAL;
    cnmsg->seq = 0;
    cnmsg->ack = 0;
}


int main(void)
{
	struct cn_msg * cmsg;
	struct w1_netlink_msg * msg;
	struct w1_netlink_cmd * cmd;
	int ret;

	nlMsgSend = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_MSGSIZE));
	nlMsgRecv = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_MSGSIZE));
	if (nlMsgSend == NULL || nlMsgRecv == NULL)
	{
		perror("Cannot allocate memory for netlink message header!");
		exit(-1);
	}
	memset(nlMsgSend, 0, NLMSG_SPACE(MAX_MSGSIZE));
	memset(nlMsgRecv, 0, NLMSG_SPACE(MAX_MSGSIZE));


	addrRemote.nl_family = AF_NETLINK;
    addrRemote.nl_pid = 0;
    addrRemote.nl_groups = CN_IDX_W1;

	sd = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_CONNECTOR);

	addrLocal.nl_family = AF_NETLINK;
	addrLocal.nl_groups = CN_IDX_PROC;
	addrLocal.nl_pid = getpid();

	if (bind(sd, (struct sockaddr *)&addrLocal, sizeof(struct sockaddr_nl)) == -1) {
		perror("bind");
		close(sd);
		return -1;
	}


	while (1)
	{
		ret = retrieve_w1_msg(&socketMsgRecv);

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
}


