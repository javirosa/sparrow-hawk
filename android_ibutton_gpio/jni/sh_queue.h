/*
 *
 * Copyright (c) 2011 Deven Fan <deven@sparrow-hawk.net>
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

#ifndef __SH_QUEUE_H
#define __SH_QUEUE_H

typedef struct sh_queue_node {
	struct sh_queue_node * next;
	int data_len;
	void * data;
} sh_queue_node;

typedef struct sh_queue {
	sh_queue_node * head;
	sh_queue_node * tail;
} sh_queue;

void sh_queue_init(sh_queue * myqueue);

void sh_enqueue(sh_queue * myqueue, sh_queue_node * mynode);

sh_queue_node * sh_dequeue(sh_queue * myqueue);

int sh_queue_is_empty(sh_queue * myqueue);

int sh_queue_get_length(sh_queue * myqueue);

#endif /* __SH_QUEUE_H */
