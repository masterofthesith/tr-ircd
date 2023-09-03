/*
 *
 *   Copyright (C) 2000-2003 TR-IRCD Development
 *
 * $Id: resnew.h,v 1.9 2005/06/19 09:52:46 tr-ircd Exp $ 
 * New res.h
 * Aaron Sethman <androsyn@ratbox.org>
 */

#ifndef RESNEW_H
#define RESNEW_H 1

#include "fileio.h"
#include "../lib/adns/adns.h"
#include "../lib/adns/internal.h"
#include "ircd_defs.h"

#define DNS_BLOCK_SIZE 64

struct DNSQuery {
	void *ptr;
	adns_query query;
	adns_answer answer;
	void (*callback)(void* vptr, adns_answer *reply);
};

void init_resolver(void);
void restart_resolver(void);
void timeout_adns (void * );
void dns_writeable (int fd , void *ptr );
void dns_readable (int fd , void *ptr );
void dns_do_callbacks(void);
void dns_select (void);
void adns_gethost (const char *name , int aftype , struct DNSQuery *req );
void adns_getaddr (struct irc_inaddr *addr , int aftype , struct DNSQuery *req );
void delete_adns_queries(struct DNSQuery *q);
void report_adns_servers(struct Client *);
#endif
