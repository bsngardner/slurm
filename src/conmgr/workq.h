/*****************************************************************************\
 *  workq.h - declarations for work queue manager
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

#ifndef SLURMRESTD_WORKQ_H
#define SLURMRESTD_WORKQ_H

/*
 * Call back for generic work
 *
 * IN arg pointer to data for function
 */
typedef void (*work_func_t)(void *arg);

/*
 * Initialize workq members
 * IN count - number of workers to add
 */
extern void workq_init(int count);

/*
 * Release workq members.
 * Will stop all workers (eventually).
 */
extern void workq_fini(void);

/*
 * Stop all work (eventually) and reject new requests
 * This will block until all work is complete.
 */
extern void workq_quiesce(void);

/*
 * Add work to workq
 * IN func - function pointer to run work
 * IN arg - arg to hand to function pointer
 * IN tag - tag used in logging this function
 * NOTE: never add a thread that will never return or workq_free() will never
 * return either.
 * RET SLURM_SUCCESS or error if workq already shutdown
 */
extern int workq_add_work(work_func_t func, void *arg, const char *tag);

/*
 * Grab copy of the workq active count
 */
extern int workq_get_active(void);

#endif /* SLURMRESTD_WORKQ_H */
