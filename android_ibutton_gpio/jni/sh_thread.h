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

#ifndef __SH_THREAD_H
#define __SH_THREAD_H

typedef struct sh_signal_ctrl {
	pthread_mutex_t mutex;
	pthread_cond_t cond;
} sh_signal_ctrl;


int sh_signal_init(sh_signal_ctrl * signal);

int sh_signal_destroy(sh_signal_ctrl * signal);

int sh_signal_wait(sh_signal_ctrl * signal);

int sh_signal_notify(sh_signal_ctrl * signal);


#endif /* __SH_THREAD_H */
