/************************************************************************
 *   IRC - Internet Relay Chat, include/dh.h
 *   Copyright (C) 2000
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2, or (at your option)
 *   any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifdef HAVE_ENCRYPTION_ON

/*
 * Do not change these unless
 * you also change the prime below
 */

#define KEY_BITS 512

#define RAND_BITS KEY_BITS
#define RAND_BYTES (RAND_BITS / 8)
#define RAND_BYTES_HEX ((RAND_BYTES * 2) + 1)

#define PRIME_BITS 1024
#define PRIME_BYTES (PRIME_BITS / 8)
#define PRIME_BYTES_HEX ((PRIME_BYTES * 2) + 1)


extern int dh_init(void);
extern int dh_generate_shared(struct dh_session *, char *);
extern int dh_get_s_shared(char *, int *, struct dh_session *);
extern int dh_hexstr_to_raw(char *, unsigned char *, int *);
extern void dh_end_session(struct dh_session *);
extern struct dh_session *dh_start_session(void);
extern char *dh_get_s_secret(char *, int, struct dh_session *);
extern char *dh_get_s_public(char *, int, struct dh_session *);

extern void rc4_process_stream_to_buf(struct rc4_state *, const unsigned char *, unsigned char *, unsigned int);
extern void rc4_process_stream(struct rc4_state *, unsigned char *, unsigned int);
extern void rc4_destroystate(struct rc4_state *);
extern struct rc4_state *rc4_initstate(unsigned char *, int);

/* Taken from Azzurranet Bahamut SSL Patch */

extern int safe_SSL_read(aClient *, void *, int);
extern int safe_SSL_write(aClient *, const void *, int);
extern int safe_SSL_accept(aClient *, int);
extern int SSL_smart_shutdown(SSL *);
extern int initssl(void);
extern SSL_CTX *ircdssl_ctx;

#endif

extern void ircd_init_ssl(void);

