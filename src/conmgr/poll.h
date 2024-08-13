/*****************************************************************************\
 *  poll.h - Internal declarations for poll() handlers
 *****************************************************************************
 *  Copyright (C) SchedMD LLC.
 *
 *  This file is part of Slurm, a resource management program.
 *  For details, see <https://slurm.schedmd.com/>.
 *  Please also read the included file: DISCLAIMER.
 *
 *  Slurm is free software; you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License as published by the Free
 *  Software Foundation; either version 2 of the License, or (at your option)
 *  any later version.
 *
 *  In addition, as a special exception, the copyright holders give permission
 *  to link the code of portions of this program with the OpenSSL library under
 *  certain conditions as described in each individual source file, and
 *  distribute linked combinations including the two. You must obey the GNU
 *  General Public License in all respects for all of the code used other than
 *  OpenSSL. If you modify file(s) with this exception, you may extend this
 *  exception to your version of the file(s), but you are not obligated to do
 *  so. If you do not wish to do so, delete this exception statement from your
 *  version.  If you delete this exception statement from all source files in
 *  the program, then also delete it here.
 *
 *  Slurm is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with Slurm; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA.
\*****************************************************************************/

#ifndef _CONMGR_POLL_H
#define _CONMGR_POLL_H

#include <stdbool.h>
#include <stdint.h>

/* typedef for the type of events returned by epoll */
typedef uint32_t pollctl_events_t;

/* If events indicate connection is ready for READ operation */
extern bool pollctl_events_can_read(pollctl_events_t events);
/* If events indicate connection is ready for WRITE operation */
extern bool pollctl_events_can_write(pollctl_events_t events);
/* If events indicate connection has active ERROR state */
extern bool pollctl_events_has_error(pollctl_events_t events);
/* If events indicate connection has received HANGUP notification */
extern bool pollctl_events_has_hangup(pollctl_events_t events);

/*
 * Create new polling controller.
 * IN max_connections - Max connections same as mgr.max_connections
 * RET ptr to polling controller (ptcl). Must be released via pollctl_fini().
 */
extern void pollctl_init(const int max_connections);

/*
 * Release memory and resources of polling controller
 */
extern void pollctl_fini(void);

typedef enum {
	PCTL_TYPE_INVALID = 0,
	/*
	 * Not possible to poll this file descriptor type.
	 * Files and directories can not be epoll()ed.
	 */
	PCTL_TYPE_UNSUPPORTED,
	PCTL_TYPE_NONE, /* Stop polling connection */
	PCTL_TYPE_CONNECTED, /* only watch for connection to hangup/close */
	PCTL_TYPE_READ_ONLY,
	PCTL_TYPE_READ_WRITE,
	PCTL_TYPE_WRITE_ONLY,
	PCTL_TYPE_LISTEN,
	PCTL_TYPE_INVALID_MAX /* place holder */
} pollctl_fd_type_t;

extern const char *pollctl_type_to_string(pollctl_fd_type_t type);

/*
 * Add new connection to monitor via poll()
 * IN fd - file descriptor to start polling
 * IN type - type of file descriptor
 * IN con_name - connection name for logging
 * IN caller - __func__ from caller
 * RET SLURM_SUCCESS or error
 */
extern int pollctl_link_fd(int fd, pollctl_fd_type_t type, const char *con_name,
			   const char *caller);

/*
 * Update existing connection type of monitoring via poll()
 * IN fd - file descriptor to start polling
 * IN type - type of file descriptor
 * IN con_name - connection name for logging
 * IN caller - __func__ from caller
 */
extern void pollctl_relink_fd(int fd, pollctl_fd_type_t type,
			      const char *con_name, const char *caller);

/*
 * Remove connection from monitoring via poll()
 * IN fd - file descriptor to stop polling
 * IN con_name - connection name for logging
 * IN caller - __func__ from caller
 */
extern void pollctl_unlink_fd(int fd, const char *con_name, const char *caller);

/*
 * Run poll() against array of file descriptors
 * WARNING: will lock and unlock mgr.mutex during execution
 * IN caller - __func__ from caller
 * RET SLURM_SUCCESS or error
 */
extern int pollctl_poll(const char *caller);

/*
 * Callback function prototype for walking all events
 * IN fd - file descriptor that had events
 * IN events - EPOLL* events from epoll_wait()
 * IN arg - arbitrary pointer handed to pollctl_for_each_event
 * RET SLURM_SUCCESS or error to stop for each loop
 */
typedef int (*pollctl_event_func_t)(int fd, pollctl_events_t events, void *arg);

/*
 * Walk every event and call func(arg)
 * Designed to be called after pollctl_poll() with locks active as this function
 * will not block for a lock
 *
 * Warning: Must be called after pollctl_poll() before next call of
 *	pollctl_poll()
 *
 * IN func - function to call
 * IN arg - arbitrary pointer handed to func
 * IN func_name - XSTRINGIFY(func) for logging
 * IN caller - __func__ from caller
 * RET SLURM_SUCCESS or error (could be propagated from func)
 */
extern int pollctl_for_each_event(pollctl_event_func_t func, void *arg,
				  const char *func_name, const char *caller);

/*
 * Send interrupt (via pipe()) to poll()
 * Note: Ignored if poll() is not running
 * WARNING: This is called while holding mgr.mutex. Do not wait in this
 * function.
 * IN caller - __func__ from caller
 */
extern void pollctl_interrupt(const char *caller);

#endif /* _CONMGR_POLL_H */
