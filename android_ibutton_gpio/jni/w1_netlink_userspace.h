/*
 * w1_netlink_userspace.h
 *
 * Original filename: kernel_src_2.6.29/drivers/w1/w1_netlink.h
 * Shipped by Deven Fan: deven.fan@gmail.com
 *
 *
 * Copyright (c) 2003 Evgeniy Polyakov <johnpol@2ka.mipt.ru>
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef __W1_NETLINK_USERSPACE_H
#define __W1_NETLINK_USERSPACE_H

#ifdef __KERNEL__
	#include <asm/types.h>
#endif

#include <linux/netlink.h>		//it will include <linux/socket.h>
#include <linux/connector.h>	//it will include <linux/types.h>

#ifndef	W1_GROUP
#define W1_GROUP  	CN_W1_IDX
#endif

#ifndef  NETLINK_DROP_MEMBERSHIP
#define  NETLINK_DROP_MEMBERSHIP 0
#endif

#ifndef  NETLINK_ADD_MEMBERSHIP
#define  NETLINK_ADD_MEMBERSHIP 1
#endif

#ifndef  SOL_NETLINK
#define  SOL_NETLINK 270
#endif


enum w1_netlink_message_types {
	W1_SLAVE_ADD = 0,
	W1_SLAVE_REMOVE,
	W1_MASTER_ADD,
	W1_MASTER_REMOVE,
	W1_MASTER_CMD,
	W1_SLAVE_CMD,
	W1_LIST_MASTERS,
};

struct w1_netlink_msg
{
	__u8				type;
	__u8				status;
	__u16				len;
	union {
		__u8			id[8];
		struct w1_mst {
			__u32		id;
			__u32		res;
		} mst;
	} id;
	__u8				data[0];
};

enum w1_commands {
	W1_CMD_READ = 0,
	W1_CMD_WRITE,
	W1_CMD_SEARCH,
	W1_CMD_ALARM_SEARCH,
	W1_CMD_TOUCH,
	W1_CMD_RESET,
	W1_CMD_MAX,
};

struct w1_netlink_cmd
{
	__u8				cmd;
	__u8				res;
	__u16				len;
	__u8				data[0];
};


#ifdef __KERNEL__

	#include "w1.h"

	void w1_netlink_send(struct w1_master *, struct w1_netlink_msg *);

	int w1_init_netlink(void);

	void w1_fini_netlink(void);

#endif


#endif /* __W1_NETLINK_USERSPACE_H */
