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

#ifndef __SH_ERROR_H
#define __SH_ERROR_H

#define		E_OK 					(0x00000000)
#define 	E_ERROR					(0x80000000)

#define		E_GENERIC_ERROR			(E_ERROR | 0x10000)
#define		E_NULL_POINTER 			(E_ERROR | 0x0001)
#define 	E_CANNOT_INIT			(E_ERROR | 0x0002)
#define 	E_CANNOT_DESTROY		(E_ERROR | 0x0003)
#define 	E_CANNOT_INVOKE			(E_ERROR | 0x0004)

#define 	E_SOCKET_ERROR			(E_ERROR | 0x20000)
#define 	E_SOCKET_PEER_GONE		(E_SOCKET_ERROR | 0x0101)
#define 	E_SOCKET_CANNOT_SEND	(E_SOCKET_ERROR | 0x0102)
#define 	E_SOCKET_CANNOT_RECV	(E_SOCKET_ERROR | 0x0103)


#define		E_MEMORY_ERROR			(E_ERROR | 0xF0000)
#define 	E_OUT_OF_MEM			(E_MEMORY_ERROR | 0x1001)
#define 	E_OVERFLOW				(E_MEMORY_ERROR | 0x1002)


#endif /* __SH_ERROR_H */
