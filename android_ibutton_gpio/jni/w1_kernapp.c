/*
 * w1_kernapp.c
 *
 *  Update on: 2011-11-23
 * 	   Author: Deven
 * 	   Remark: I think it cannot be used from kernel space.
 *
 *  Created on: 2011-11-14
 *      Author: deven
 */



#include <linux/version.h>
#include <linux/module.h>

#include <linux/sched.h>

#include <linux/skbuff.h>
#include <linux/netlink.h>
#include <linux/connector.h>

#include <net/sock.h>

#include "w1_netlink.h"


//#define BUF_SIZE 16384
static struct sock * netlink_w1_sock;
//static unsigned char buffer[BUF_SIZE];
//static unsigned int buffer_tail = 0;
static int exit_flag = 0;
static DECLARE_COMPLETION(exit_completion);



static void on_w1_msg_received(struct cn_msg * cmsg)
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
				printk("A w1 salve device found: %ld \n", slave_rn);
			}
			else
			{
				printk("Wrong data in the w1 search slave message!\n");
			}
			break;


	}
}


#if LINUX_VERSION_CODE >= 0x02061B

	static void recv_handler(struct sk_buff *__skb)
	{
		struct sk_buff * skb = NULL;		//socket buffer
		struct nlmsghdr * nlhdr = NULL;		//netlink message header
		struct cn_msg * cmsg = NULL;		//netlink connector message
		struct w1_netlink_msg * w1msg = NULL;		//w1 netlink message
		struct w1_netlink_cmd * w1cmd = NULL;		//w1 netlink command

		int minSize = sizeof(struct nlmsghdr) + sizeof(struct cn_msg)
				+ sizeof(struct w1_netlink_msg) + sizeof(struct w1_netlink_cmd);

		int len;

		//must be done here:
		skb = skb_get(__skb);

		nlhdr = (struct nlmsghdr *)skb->data;
		if (nlhdr->nlmsg_len < minSize)
		{
			printk("Corrupt w1 netlink message.\n");
			return;
		}

		len = nlhdr->nlmsg_len - NLMSG_LENGTH(0);
		printk("Got a netlink msg, it's length is %d.\n", len);

		cmsg = NLMSG_DATA(nlhdr);
		on_w1_msg_received(cmsg);
	}

#else

	static void recv_handler(struct sock * sk, int length)
	{
		wake_up(sk->sk_sleep);

		//can be done here:
		//skb = skb_dequeue(&sk->sk_receive_queue);
	}

#endif


#if LINUX_VERSION_CODE < 0x02061B

static int process_message_thread(void * data)
{
	struct sk_buff * skb = NULL;			//socket buffer
	struct nlmsghdr * nlhdr = NULL;			//netlink message header
	struct cn_msg * cmsg = NULL;			//netlink connector message
	struct w1_netlink_msg * w1msg = NULL;	//w1 netlink message
	struct w1_netlink_cmd * w1cmd = NULL;	//w1 netlink command

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

#endif


static int __init w1_netlink_kernelapp_init(void)
{

//	netlink_w1_sock = netlink_kernel_create(NETLINK_CONNECTOR, CN_W1_IDX, recv_handler, THIS_MODULE);

#if LINUX_VERSION_CODE >= 0x020618
	//init_net是一个内核变量
	netlink_w1_sock = netlink_kernel_create(&init_net, NETLINK_CONNECTOR, CN_W1_IDX, recv_handler, NULL, THIS_MODULE);

#elif LINUX_VERSION_CODE >= 0x020616
	netlink_w1_sock = netlink_kernel_create(NETLINK_CONNECTOR, CN_W1_IDX, recv_handler, NULL, THIS_MODULE);

#elif LINUX_VERSION_CODE >= 0x020610
	netlink_w1_sock = netlink_kernel_create(NETLINK_CONNECTOR, CN_W1_IDX, recv_handler, THIS_MODULE);

#else
	netlink_w1_sock = netlink_kernel_create(NETLINK_CONNECTOR, recv_handler);

#endif


	if (!netlink_w1_sock) {
		printk("Fail to create w1 netlink socket for kernapp.\n");
		return 1;
	}

#if LINUX_VERSION_CODE < 0x02061B
	kernel_thread(process_message_thread, NULL, CLONE_KERNEL);
#endif

	printk("w1 netlink kernapp started.\n");

	return 0;
}

static void __exit w1_netlink_kernelapp_exit(void)
{
	exit_flag = 1;
	wake_up(netlink_w1_sock->sk_sleep);
	wait_for_completion(&exit_completion);
	sock_release(netlink_w1_sock->sk_socket);

	printk("w1 netlink kernapp stopped.\n");
}

module_init(w1_netlink_kernelapp_init);
module_exit(w1_netlink_kernelapp_exit);
MODULE_LICENSE("GPL");






