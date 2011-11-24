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


#include <stdio.h>

#include "sh_queue.h"

void sh_queue_init(sh_queue * myqueue)
{
	myqueue->head = NULL;
	myqueue->tail = NULL;
}

void sh_enqueue(sh_queue * myqueue, sh_queue_node * mynode)
{
	mynode->next = NULL;
	if (myqueue->tail != NULL)
	{
		myqueue->tail->next = mynode;
	}
	myqueue->tail = mynode;
	if (myqueue->head == NULL)
	{
		myqueue->head = mynode;
	}
}

sh_queue_node * sh_dequeue(sh_queue * myqueue)
{
	//fetch from head
	sh_queue_node * mynode = myqueue->head;

	if (myqueue->head != NULL)
		myqueue->head = myqueue->head->next;

	return mynode;
}

int sh_queue_is_empty(sh_queue * myqueue)
{
	return (myqueue->head == NULL);
}

int sh_queue_get_length(sh_queue * myqueue)
{
	int count = 0;
	sh_queue_node * mynode = myqueue->head;
	while(mynode != NULL)
	{
		count++;
		if(mynode != mynode->next)
			mynode = mynode->next;
		else
			break;
	}
	return count;
}


/*
int sh_thread_ctrl_init(sh_thread_ctrl * myctrl)
{
	int mystatus;
	if (pthread_mutex_init(&(myctrl->mutex), NULL))
		return 1;
	if (pthread_cond_init(&(myctrl->cond), NULL))
		return 1;
	myctrl->active = 0;
	return 0;
}

int sh_thread_ctrl_destroy(sh_thread_ctrl *myctrl)
{
	int mystatus;
	if (pthread_cond_destroy(&(myctrl->cond)))
		return 1;
	if (pthread_cond_destroy(&(myctrl->cond)))
		return 1;
	myctrl->active = 0;
	return 0;
}

int sh_thread_ctrl_activate(sh_thread_ctrl *myctrl)
{
	int mystatus;
	if (pthread_mutex_lock(&(myctrl->mutex)))
		return 0;
	myctrl->active = 1;
	pthread_mutex_unlock(&(myctrl->mutex));
	pthread_cond_broadcast(&(myctrl->cond));
	return 1;
}

int sh_thread_ctrl_deactivate(sh_thread_ctrl *myctrl)
{
	int mystatus;
	if (pthread_mutex_lock(&(myctrl->mutex)))
		return 0;
	myctrl->active = 0;
	pthread_mutex_unlock(&(myctrl->mutex));
	pthread_cond_broadcast(&(myctrl->cond));
	return 1;
}
*/


