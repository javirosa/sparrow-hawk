/*
 * w1_kernapp.c
 *
 *  Created on: 2011-11-14
 *      Author: deven
 */


#include <linux/module.h>

#include <linux/skbuff.h>
#include <linux/netlink.h>
#include <linux/connector.h>

#include "w1.h"
#include "w1_log.h"
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



static void w1_cn_callback(void *data)
{
	struct cn_msg * cmsg = data;
	struct w1_netlink_msg * w1msg = (struct w1_netlink_msg *)(cmsg + 1);

	struct w1_netlink_cmd * w1cmd;

	struct w1_slave * salve;
	struct w1_master * master;
	int err = 0;

	while (w1msg->len && !err)
	{
		struct w1_reg_num id;
		u16 mlen = w1msg->len;
		u8 *cmd_data = w1msg->data;

		master = NULL;
		salve = NULL;
		w1cmd = NULL;

		memcpy(&id, w1msg->id.id, sizeof(id));

		printk("%s: %02x.%012llx.%02x: type=%02x, len=%u.\n",
				__func__, id.family, (unsigned long long)id.id, id.crc, w1msg->type, w1msg->len);

		if (w1cmd->len + sizeof(struct w1_netlink_msg) > cmsg->len) {
			err = -E2BIG;
			break;
		}

		if (w1msg->type == W1_MASTER_CMD)
		{
			master = w1_search_master_id(w1msg->id.mst.id);
		}
		else if (w1msg->type == W1_SLAVE_CMD)
		{
			salve = w1_search_slave(&id);
			if (salve)
				master = salve->master;
		}
		else
		{
			err = w1_process_command_root(msg, m);
			goto out_cont;
		}

		if (!master) {
			err = -ENODEV;
			goto out_cont;
		}

		err = 0;
		if (!mlen)
			goto out_cont;

		mutex_lock(&master->mutex);

		if (salve && w1_reset_select_slave(salve)) {
			err = -ENODEV;
			goto out_up;
		}

		while (mlen) {
			cmd = (struct w1_netlink_cmd *)cmd_data;

			if (cmd->len + sizeof(struct w1_netlink_cmd) > mlen) {
				err = -E2BIG;
				break;
			}

			if (salve)
				err = w1_process_command_slave(salve, msg, m, cmd);
			else
				err = w1_process_command_master(master, msg, m, cmd);

			w1_netlink_send_error(msg, m, cmd, err);
			err = 0;

			cmd_data += cmd->len + sizeof(struct w1_netlink_cmd);
			mlen -= cmd->len + sizeof(struct w1_netlink_cmd);
		}
out_up:
		atomic_dec(&master->refcnt);
		if (salve)
			atomic_dec(&salve->refcnt);
		mutex_unlock(&master->mutex);
out_cont:
		if (!cmd || err)
			w1_netlink_send_error(msg, m, cmd, err);
		msg->len -= sizeof(struct w1_netlink_msg) + w1msg->len;
		m = (struct w1_netlink_msg *)(((u8 *)m) + sizeof(struct w1_netlink_msg) + w1msg->len);

		/*
		 * Let's allow requests for nonexisting devices.
		 */
		if (err == -ENODEV)
			err = 0;
	}
}


static int __init w1_init_netlink_client(void)
{
	struct cb_id w1_id = {.idx = CN_W1_IDX, .val = CN_W1_VAL};

	int ret = cn_add_callback(&w1_id, "w1", &w1_cn_callback);

	if(ret == 0)
		printk("w1(1-wire) netlink client start OK.\n");
	else
		printk("w1(1-wire) netlink client start failed.\n");

	return ret;
}

static void __exit w1_exit_netlink_client(void)
{
	struct cb_id w1_id = {.idx = CN_W1_IDX, .val = CN_W1_VAL};

	cn_del_callback(&w1_id);

	printk("w1(1-wire) netlink client stop OK.\n");
}


module_init(w1_init_netlink_client);
module_exit(w1_exit_netlink_client);
MODULE_LICENSE("GPL");


