/*
 * w1_kernapp.c
 *
 *  Created on: 2011-11-14
 *      Author: deven
 */


//#include <sys/types.h>
//#include <sys/socket.h>
//
////#include <signal.h>
////#include <linux/socket.h>
//#include <linux/netlink.h>
//#include <linux/connector.h>

#include <linux/config.h>
#include <linux/module.h>
#include <linux/netlink.h>
#include <linux/sched.h>
#include <net/sock.h>

#include "w1_netlink.h"


#define BUF_SIZE 16384
static struct sock * netlink_w1_sock;
//static unsigned char buffer[BUF_SIZE];
//static unsigned int buffer_tail = 0;
static int exit_flag = 0;
static DECLARE_COMPLETION(exit_completion);



static void on_w1_msg_received(cn_msg * cmsg)
{
	struct w1_netlink_msg * w1msg = (struct w1_netlink_msg *)(cmsg + 1);
	struct w1_netlink_cmd * w1cmd = (struct w1_netlink_cmd *)(w1msg + 1);

	printk("cn_msg->len: 			%d\n", cmsg->len);
	printk("w1_netlink_msg->len: 	%d\n", w1msg->len);
	printk("w1_netlink_cmd->len: 	%d\n", w1cmd->len);

	u64 slave_rn = 0;

	switch(w1cmd->cmd)
	{
		case W1_CMD_ALARM_SEARCH:
		case W1_CMD_SEARCH:

			if(w1cmd->len == sizeof(u64))
			{
				slave_rn = (u64 *) w1cmd->data;
				printf("A w1 salve device found: %d \n", slave_rn);
			}
			else
				perror("Wrong data in the w1 search slave message!\n");
			break;


	}
}


static void recv_handler(struct sock * sk, int length)
{
	wake_up(sk->sk_sleep);
}

static int process_message_thread(void * data)
{
	struct sk_buff * skb = NULL;		//socket buffer
	struct nlmsghdr * nlhdr = NULL;		//netlink message header
	struct cn_msg * cmsg = NULL;		//netlink connector message
	struct w1_netlink_msg * w1msg;		//w1 netlink message
	struct w1_netlink_cmd * w1cmd;		//w1 netlink command

	int minSize = sizeof(struct nlmsghdr) + sizeof(struct cn_msg)
			+ sizeof(struct w1_netlink_msg) + sizeof(struct w1_netlink_cmd);

	int len;

	DEFINE_WAIT(wait);

	daemonize("w1_kernapp");

	while (exit_flag == 0)
	{
		prepare_to_wait(netlink_w1_sock->sk_sleep, &wait, TASK_INTERRUPTIBLE);
		schedule();
		finish_wait(netlink_w1_sock->sk_sleep, &wait);

		while ( (skb = skb_dequeue(&netlink_w1_sock->sk_receive_queue)) != NULL )
		{
			nlhdr = (struct nlmsghdr *)skb->data;
			if (nlhdr->nlmsg_len < minSize)
			{
				printk("Corrupt w1 netlink message.\n");
				continue;
			}

			len = nlhdr->nlmsg_len - NLMSG_LENGTH(0);
			printk("Got a netlink msg, it's length is %d.\n", len);

			cmsg = NLMSG_DATA(nlhdr);
			on_w1_msg_received(cmsg);

			/*
			nlhdr->nlmsg_pid = 0;
			nlhdr->nlmsg_flags = 0;
			NETLINK_CB(skb).pid = 0;
			NETLINK_CB(skb).dst_pid = 0;
			NETLINK_CB(skb).dst_group = 1;

			netlink_broadcast(netlink_w1_sock, skb, 0, 1, GFP_KERNEL);
			*/
		}
	}
	complete(&exit_completion);
	return 0;
}


//static int netlink_exam_readproc(char *page, char **start, off_t off,
//                          int count, int *eof, void *data)
//{
//        int len;
//
//        if (off >= buffer_tail) {
//                * eof = 1;
//                return 0;
//        }
//        else {
//                len = count;
//                if (count > PAGE_SIZE) {
//                        len = PAGE_SIZE;
//                }
//                if (len > buffer_tail - off) {
//                        len = buffer_tail - off;
//                }
//                memcpy(page, buffer + off, len);
//                *start = page;
//                return len;
//        }
//
//}

static int __init w1_netlink_kernelapp_init(void)
{
	netlink_w1_sock = netlink_kernel_create(NETLINK_GENERIC, 0, recv_handler, THIS_MODULE);
	if (!netlink_w1_sock) {
		printk("Fail to create w1 netlink socket for kernapp.\n");
		return 1;
	}

	kernel_thread(process_message_thread, NULL, CLONE_KERNEL);
//	create_proc_read_entry("netlink_exam_buffer", 0444, NULL, netlink_exam_readproc, 0);
	return 0;
}

static void __exit w1_netlink_kernelapp_exit(void)
{
	exit_flag = 1;
	wake_up(netlink_w1_sock->sk_sleep);
	wait_for_completion(&exit_completion);
	sock_release(netlink_w1_sock->sk_socket);
}

module_init(w1_netlink_kernelapp_init);
module_exit(w1_netlink_kernelapp_exit);
MODULE_LICENSE("GPL");






