/*
 * Copyright 2000, 2001 Chip Norkus
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 * 2a. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 * 2b. Redistribution in binary form requires specific prior written
 *     authorization of the maintainer.
 * 
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgement:
 *    This product includes software developed by Chip Norkus.
 * 
 * 4. The names of the maintainer, developers and contributors may not be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE MAINTAINER, DEVELOPERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
 * THE DEVELOPERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/* define functions with throttling enabled, then add other definitions later
 * in case throttling was removed at compile time to speed along the system ;)
 * the functions are pretty simple, externally.  throttle_check is given
 * an IP in string dotted quad form, and returns 1 if it should be allowed,
 * or 0 if it is to be throttled and dropped.  this should be done at the same
 * time as the z:line check.  throttle_timer() should be called once per i/o
 * loop to expire throttles and Z:lines.  All other structures and functions
 * can be found in src/throttle.c as they should not be accessed outside of it.
 *
 * additionally, throttle_init() should be called once at initialization stage
 * to setup hash tables and what-have-you
 */

#ifndef THROTTLE_H
#define THROTTLE_H

/* setting bits */

/*
 * THROTTLE_LENGTH    - length to Z:line throttled connections
 * THROTTLE_HASHSIZE  - size of the throttle hashtable, also tuneable
 * -wd 
 */

#define THROTTLE_LENGTH 900
#define THROTTLE_HASHSIZE 25147

extern int throttle_tcount, throttle_ttime, throttle_ztime;

int throttle_check(char *ip, int fd, time_t sotime);

void throttle_remove(char *host);

void throttle_timer_zapped(void *unused);
void throttle_timer_nonzapped(void *unused);

void throttle_init(void);
void throttle_rehash(void);
void throttle_resize(int size);
void throttle_stats(aClient *cptr, char *name);

#endif
