/************************************************************************
 *   IRC - Internet Relay Chat, src/ssl.c
 *
 *   Copyright (C) 2000-2003 TR-IRCD Development
 *   Copyright (C) 2002 vejeta
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

/*
 * This code has been adapted from the SSL Patch for Bahamut.
 * -TimeMr14C / 19/11/2002
 */

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "s_conf.h"
#include "h.h"
#include "dh.h"

#define SAFE_SSL_READ	1
#define SAFE_SSL_WRITE	2
#define SAFE_SSL_ACCEPT	3

void ircd_init_ssl(void)
{
#ifdef HAVE_ENCRYPTION_ON
    fprintf(stderr, "SSL: Initialize\n");
    if (!(GeneralOpts.doing_ssl = initssl()))
	fprintf(stderr, "SSL: Failed. Password/Certificate error\n"
			"SSL: Client based SSL connections will be unavailable\n");
    else
	fprintf(stderr, "SSL: Client based SSL connections are enabled.\n");
#endif
    return;
}

#ifdef HAVE_ENCRYPTION_ON

SSL_CTX *ircdssl_ctx;

static int fatal_ssl_error(int, int, aClient *);

int initssl(void)
{
    SSL_load_error_strings();
    SSLeay_add_ssl_algorithms();
    ircdssl_ctx = SSL_CTX_new(SSLv23_server_method());
    if (!ircdssl_ctx) {
	ERR_print_errors_fp(stderr);
	return 0;
    }
    if (SSL_CTX_use_certificate_file(ircdssl_ctx, SSL_CERTIFICATE, SSL_FILETYPE_PEM) <= 0) {
	ERR_print_errors_fp(stderr);
	SSL_CTX_free(ircdssl_ctx);
	return 0;
    }
    if (SSL_CTX_use_PrivateKey_file(ircdssl_ctx, SSL_KEY, SSL_FILETYPE_PEM) <= 0) {
	ERR_print_errors_fp(stderr);
	SSL_CTX_free(ircdssl_ctx);
	return 0;
    }
    if (!SSL_CTX_check_private_key(ircdssl_ctx)) {
	fprintf(stderr, "Server certificate does not match Server key");
	SSL_CTX_free(ircdssl_ctx);
	return 0;
    }
    return 1;
}

int safe_SSL_read(aClient *acptr, void *buf, int sz)
{
    int len, ssl_err;

    len = SSL_read(acptr->ssl, buf, sz);

    if (len <= 0) {
	switch (ssl_err = SSL_get_error(acptr->ssl, len)) {
	    case SSL_ERROR_SYSCALL:
		if (errno == EWOULDBLOCK || errno == EAGAIN || errno == EINTR) {
	    case SSL_ERROR_WANT_READ:
		    errno = EWOULDBLOCK;
		    return -1;
		}
	    case SSL_ERROR_SSL:
		if (errno == EAGAIN)
		    return -1;
	    default:
		return fatal_ssl_error(ssl_err, SAFE_SSL_READ, acptr);
	}
    }
    return len;
}

int safe_SSL_write(aClient *acptr, const void *buf, int sz)
{
    int len, ssl_err;

    len = SSL_write(acptr->ssl, buf, sz);
    if (len <= 0) {
	switch (ssl_err = SSL_get_error(acptr->ssl, len)) {
	    case SSL_ERROR_SYSCALL:
		if (errno == EWOULDBLOCK || errno == EAGAIN || errno == EINTR) {
	    case SSL_ERROR_WANT_WRITE:
		    errno = EWOULDBLOCK;
		    return 0;
		}
	    case SSL_ERROR_SSL:
		if (errno == EAGAIN)
		    return 0;
	    default:
		return fatal_ssl_error(ssl_err, SAFE_SSL_WRITE, acptr);
	}
    }
    return len;
}

int safe_SSL_accept(aClient *acptr, int fd)
{

    int ssl_err;

    if ((ssl_err = SSL_accept(acptr->ssl)) <= 0) {
	switch (ssl_err = SSL_get_error(acptr->ssl, ssl_err)) {
	    case SSL_ERROR_SYSCALL:
		if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)
	    case SSL_ERROR_WANT_READ:
	    case SSL_ERROR_WANT_WRITE:
		    /* handshake will be completed later . . */
		    return 1;
	    default:
		return fatal_ssl_error(ssl_err, SAFE_SSL_ACCEPT, acptr);

	}
	/* NOTREACHED */
	return 0;
    }
    return 1;
}

int SSL_smart_shutdown(SSL * ssl)
{
int i;
int rc;

    rc = 0;
    for (i = 0; i < 4; i++) {
	if ((rc = SSL_shutdown(ssl)))
	    break;
    }

    return rc;
}

static int fatal_ssl_error(int ssl_error, int where, aClient *sptr)
{
    /* don`t alter errno */
    int errtmp = errno;
    char *errstr = strerror(errtmp);
    char *ssl_errstr, *ssl_func;

    switch (where) {
	case SAFE_SSL_READ:
	    ssl_func = "SSL_read()";
	    break;
	case SAFE_SSL_WRITE:
	    ssl_func = "SSL_write()";
	    break;
	case SAFE_SSL_ACCEPT:
	    ssl_func = "SSL_accept()";
	    break;
	default:
	    ssl_func = "undefined SSL func [this is a bug]";
    }

    switch (ssl_error) {
	case SSL_ERROR_NONE:
	    ssl_errstr = "No error";
	    break;
	case SSL_ERROR_SSL:
	    ssl_errstr = "Internal OpenSSL error or protocol error";
	    break;
	case SSL_ERROR_WANT_READ:
	    ssl_errstr = "OpenSSL functions requested a read()";
	    break;
	case SSL_ERROR_WANT_WRITE:
	    ssl_errstr = "OpenSSL functions requested a write()";
	    break;
	case SSL_ERROR_WANT_X509_LOOKUP:
	    ssl_errstr = "OpenSSL requested a X509 lookup which didn`t arrive";
	    break;
	case SSL_ERROR_SYSCALL:
	    ssl_errstr = "Underlying syscall error";
	    break;
	case SSL_ERROR_ZERO_RETURN:
	    ssl_errstr = "Underlying socket operation returned zero";
	    break;
	case SSL_ERROR_WANT_CONNECT:
	    ssl_errstr = "OpenSSL functions wanted a connect()";
	    break;
	default:
	    ssl_errstr = "Unknown OpenSSL error (huh?)";
    }

    sendto_lev(DEBUG_LEV, "%s to %C aborted with%serror (%s). [%s]",
	       ssl_func, sptr, (errno > 0) ? " " : " no ", errstr, ssl_errstr);

    logevent_call(LogSys.ssl_error, ssl_func, errstr, ssl_errstr);

    /* if we reply() something here, we might just trigger another
     * fatal_ssl_error() call and loop until a stack overflow... 
     * the client won`t get the ERROR : ... string, but this is
     * the only way to do it.
     * IRC protocol wasn`t SSL enabled .. --vejeta
     */

    errno = errtmp ? errtmp : EIO;	/* Stick a generic I/O error */
    sptr->flags |= FLAGS_DEADSOCKET;
    return 0;
}
#endif
