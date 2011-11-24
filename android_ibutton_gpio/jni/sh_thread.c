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

#include "sh_thread.h"
#include "sh_error.h"


int sh_signal_init(sh_signal_ctrl * signal)
{
	if(NULL == signal)	return E_NULL_POINTER;

	if(0 != pthread_mutex_init(signal->mutex, NULL))
	{
		return E_CANNOT_INIT;
	}

	if(0 != pthread_cond_init(signal->cond, NULL))
	{
		pthread_mutex_destroy(signal->mutex);
		return E_CANNOT_INIT;
	}

	return E_OK;
}


int sh_signal_destroy(sh_signal_ctrl * signal)
{
	if(NULL == signal) return E_NULL_POINTER;

	int ret = E_OK;

	if(0 != pthread_mutex_destroy(signal->mutex))
	{
		ret = E_CANNOT_DESTROY;
	}

	if(0 != pthread_cond_destroy(signal->cond))
	{
		ret = E_CANNOT_DESTROY;
	}

	return ret;
}

void sh_signal_wait(sh_signal_ctrl * signal)
{
	//OK will return 0
	return pthread_cond_wait(&signal.cond, &signal.mutex);
}

void sh_signal_notify(sh_signal_ctrl * signal)
{
	//OK will return 0

	//it will notify the first thread waiting for this signal
	//pthread_cond_signal(&signal.cond);

	//it will notify all the threads waiting for this signal
	return pthread_cond_broadcast(&signal.cond);
}






/*
int sh_thread_ctrl_init(sh_signal_ctrl * myctrl)
{
	int mystatus;
	if (pthread_mutex_init(&(myctrl->mutex), NULL))
		return 1;
	if (pthread_cond_init(&(myctrl->cond), NULL))
		return 1;
	myctrl->active = 0;
	return 0;
}

int sh_signal_ctrl_destroy(sh_signal_ctrl *myctrl)
{
	int mystatus;
	if (pthread_cond_destroy(&(myctrl->cond)))
		return 1;
	if (pthread_cond_destroy(&(myctrl->cond)))
		return 1;
	myctrl->active = 0;
	return 0;
}

int sh_signal_ctrl_activate(sh_signal_ctrl *myctrl)
{
	int mystatus;
	if (pthread_mutex_lock(&(myctrl->mutex)))
		return 0;
	myctrl->active = 1;
	pthread_mutex_unlock(&(myctrl->mutex));
	pthread_cond_broadcast(&(myctrl->cond));
	return 1;
}

int sh_signal_ctrl_deactivate(sh_signal_ctrl *myctrl)
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


