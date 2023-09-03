/************************************************************************
 *   IRC - Internet Relay Chat, include/struct.h
 *
 *   Copyright (C) 2000-2003 TR-IRCD Development
 *
 *   Copyright (C) 1990 Jarkko Oikarinen and
 *                      University of Oulu, Co Center
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
 *
 *
 */

/*
 * $Id: struct.h,v 1.9 2004/02/24 19:03:32 tr-ircd Exp $ 
 */

#ifndef	STRUCT_H
#define STRUCT_H 1

typedef struct Channel aChannel;
typedef struct ConnClass aClass;
typedef struct Client aClient;
typedef struct ClientIdentity CID;
typedef struct ConfEntry aConfEntry;
typedef struct ConfItem aConfItem;
typedef struct ConfList aConfList;
typedef struct hashentry aHashEntry;
typedef struct ListOptions LOpts;
typedef struct MaskItem aMaskItem;
typedef struct Motd aMotd;
typedef struct MotdItem aMotdItem;
typedef struct NUH aNUH;
typedef struct SearchOptions SOpts;
typedef struct Server aServer;
typedef struct Service aService;
typedef struct SMode Mode;
typedef struct User anUser;
typedef struct Watch aWatch;
typedef struct Whowas aWhowas;
typedef struct ModEvent modEvent;

typedef unsigned long IRCU32;
typedef unsigned char RC4BYTE;
typedef unsigned int RC4DWORD;


#include "config.h"
#include "sys.h"
#include "linebuf.h"
#include "tools.h"
#include "capabflag.h"
#include "ircd_defs.h"
#include "../lib/uuid/uuidP.h"

struct hashentry {
   int hits;
   int links;
   void *list;
};

struct ip_value {
    struct irc_inaddr ip;
    int ip_mask;
    int type;
};

#ifdef HAVE_ENCRYPTION_ON
struct rc4_state {
    RC4BYTE mstate[256];
    RC4BYTE x;
    RC4BYTE y;
};

struct dh_session {
   DH *dh;
   char *session_shared;
   int session_shared_length;
};
#endif

struct ConnClass {
   int number;
   int connect_frequency;
   int ping_frequency;
   int max_connections;
   long max_sendq;
   int links;
   aClass *next;
};

struct ConfList {
    unsigned int length;
    aConfEntry *conf_list;
};

struct ConfEntry {
    char *pattern;
    aConfItem *conf;
    aConfEntry *next;
    aConfList *sub;
};

struct MotdItem {
    char line[MOTDLINELEN];
    struct MotdItem *next;
};

struct Motd {
    char filename[MAXPATHLEN];
    int type;
    struct MotdItem *content;
    char lastchange[MAX_DATE_STRING];
};

struct capablist {
    char *name;
    unsigned int flag;
};

struct ConfigElement {
    char *lexer_string;
    int flags;
    void *element;
    char *desc;
};

struct InfoConf {
    char *name;
    struct ConfigElement *ce;
};

struct ClientIdentity {
    IRCU32 id;			/* client identity: highword server, loword user */
    char string[8];		/* base64 representation of id */
};

struct NUH {
    char *nuhstr;
    char *who;
    time_t when;
    u_char type;
};

struct SMode {
    IRCU32 mode;
    int limit;
    char key[KEYLEN + 1];
    int lines;
    int intime;
    char link[CHANNELLEN + 1];
    int joindelay;
};

struct PlusList {
    struct Client *cptr;
    int flags;
};

struct Watch {
    struct Watch *hnext;
    time_t lasttime;
    char watchnick[NICKLEN];
    dlink_list watched_by;
};

struct ListOptions {
    dlink_list yeslist, nolist;
    int starthash;
    short int showall;
    unsigned short usermin;
    int usermax;
    time_t currenttime;
    time_t chantimemin;
    time_t chantimemax;
    time_t topictimemin;
    time_t topictimemax;
};

struct Whowas {

    struct Whowas *next;	/* for hash table... */
    struct Whowas *prev;	/* for hash table... */
    struct Whowas *cnext;	/* for client struct linked list */
    struct Whowas *cprev;	/* for client struct linked list */

    int hashv;

    char name[NICKLEN + 1];
    char username[USERLEN + 1];
    char hostname[HOSTLEN + 1];	/* we only show fakehost here */
    char morehost[HOSTLEN + 1];	/* shown to ircops, the realhost */
    char realname[REALLEN + 1];

    char *servername;

    time_t logoff;

    struct Client *online;	/* Pointer to new nickname for chasing
				 * or NULL */
};

struct ConfItem {

    struct ConfItem *next;	/* list node pointer */
    struct ConnClass *class;	/* Class of connection */
    struct DNSQuery *dns_query;

    struct irc_inaddr ipnum;

    IRCU32 status;		/* If CONF_ILLEGAL, delete when no clients */
    IRCU32 flags;
    IRCU32 ip;			/* only used for I D lines etc. */

    char *name;			/* IRC name, nick, server name, or original u@h */
    char *host;			/* host part of user@host */
    char *passwd;		/* password */
    char *spasswd;		/* password to send */
    char *user;			/* user part of user@host */
    char *localhost;

    time_t hold;		/* Hold action until this time (calendar time) */

    int port;
    int clients;		/* Number of *LOCAL* clients using this */
    int aftype;			/* makes life easier with ipv6 */

    uuid_t uuid;		/* The unique id for this confitem */
};

struct User {

    dlink_list channel;		/* chain of channel pointer blocks */
    dlink_list invited;		/* chain of invite pointer blocks */
    dlink_list silence;		/* chain of silenced users */
    dlink_list dccallow;	/* chain of dcc send allowed users */
    dlink_list allowed_by;	/* chain of users who dccallow this */
    dlink_list accepted;	
    dlink_list accepted_by;
    dlink_list accept_informed;
    dlink_list accept_informed_by;

    char username[USERLEN + 1];
    char host[HOSTLEN + 1];
    char fakehost[HOSTLEN + 1];

    /*
     * In a perfect world the 'server' name should not be needed, a
     * pointer to the client describing the server is enough.
     * Unfortunately, in reality, server may not yet be in links while
     * USER is introduced... --msa
     */

    char *server;		/* pointer to scached server name */
    char *away;			/* pointer to away message */
    char *real_oper_host;
    char *real_oper_ip;

    time_t last;

    int joined;			/* number of channels joined */
    int bans;			/* number of matching bans */

    IRCU32 servicestamp;	/* services id */

};

struct Server {

    anUser *user;

    char bynick[NICKLEN + 1];

    aConfItem *nline;		/* N-line pointer for this server */

    int dkey_flags;		/* dkey flags */

    struct Client *userid[ID_MAP_SIZE];	/* identity hash-map */
    struct Client *servers;	/* Servers on this server */
    struct Client *users;	/* Users on this server */

    int servercnt;		/* Number of connected servers */
    int usercnt;		/* Number of users */

#ifdef HAVE_ENCRYPTION_ON
    struct rc4_state *rc4_in;
    struct rc4_state *rc4_out;    
    struct dh_session *sin;	/* opaque sessioninfo structure */
    struct dh_session *sout;	/* opaque sessioninfo structure */
#endif

    z_stream *zin;
    z_stream *zout;

};

struct Service {
    int sflags;			/* Flags of this service */
    int enable;			/* Services can see these */

    char *server;		/* uplink server */

    aClient *scptr;		/* Client Structure the service belongs 
				 * to */
    struct Service *next, *prev;
};

struct Client {

    long tsval;			/* This value is required at this place for %T */

    struct Client *next;
    struct Client *prev;
    struct Client *hnext;
    struct Client *idhnext;

    struct Client *servptr;	/* Points to server this Client is on */

    struct Client *lnext;	/* Used for Server->servers/users */
    struct Client *lprev;	/* Used for Server->servers/users */
    struct Client *from;	/* == self, if Local Client, NEVERNULL! */

    anUser *user;		/* ...defined, if this is a User */
    aServer *serv;		/* ...defined, if this is a server */
    aService *service;		/* Guess what this is -TimeMr14C */

    aWhowas *whowas;		/* Pointers to whowas structs */

    time_t lasttime;		/* ...should be only LOCAL clients? */
    time_t firsttime;		/* time client was created */
    time_t since;		/* last time we parsed something */

    CID id;			/* Client numeric identity */

    short status;		/* Client type */

    long flags;			/* client flags */
    long umode;			/* We can illeviate overflow this way */
    long protoflags;		/* Additional client flags */

    char name[HOSTLEN + 1];	/* Unique name of the client, nick or
				 * host */
    char info[REALLEN + 1];	/* additional client info */
    char hostip[HOSTIPLEN + 1];	/* Keep real ip as string too */
    char username[USERLEN + 1];	/* username here now for auth stuff */
    char sockhost[HOSTLEN + 1];

    int fd;			/* >= 0, for local clients */
    int hopcount;		/* number of servers to this 0 = local */
    int capabilities;		/* what this server/client supports */
    int lang;			/* language hash value */
    int oflag;			/* Operator Flags */

    struct irc_inaddr ip;	/* keep real ip# too */

    dlink_list confs;		/* Configuration record associated */

    /*
     * ABOVE IS THE LAST ITEM IN THE GLOBAL; REMOTE CLIENT STRUCTURE 
     * FROM HERE ON, THE LOCAL CLIENT STRUCTURE INFORMATION IS CREATED.
     * VALUES FOR THE FIELDS FROM "int count" TO THE END ARE ONLY AVAILABLE
     * IN A LOCAL CLIENT STRUCTURE.
     */

    int count;			/* Amount of data in buffer */

    int authfd;			/* fd for rfc931 authentication */
    int watches;		/* how many watches this user has set */
    int pingval;		/* cache client class ping value here */
    int sendqlen;		/* cache client max sendq here */
    int aftype;			/* Makes life easier for DNS res in IPV6 */
    int connectport;		/* The port on the client side */
    int waitlen;

    char passwd[PASSWDLEN + 1];
    char fakehost_override[63];	/* the new fake hostname for the operator */
    char nspass[32];		/* the nickserv password */
    char langstring[16];	/* language declaration string */

    char *restbuf;

    buf_head_t sendQ;		/* send message queue */
    buf_head_t recvQ;		/* receive message queue to parse */

    long sendM;			/* Statistics: protocol messages send */
    long sendK;			/* Statistics: total k-bytes send */
    long receiveM;		/* Statistics: protocol messages */
    long receiveK;		/* Statistics: total k-bytes received */
    long lastrecvM;		/* to check for activity --Mika */
    long len_to_parse;		/* Content-Length of the text to parse from the client */

    unsigned short sendB;	/* counters to count upto 1-k lots */
    unsigned short receiveB;	/* sent and received. */

    dlink_list watchlist;	/* user's watch list */

    struct DNSQuery *dns_query;	/* result returned from resolver query */
    struct Listener *listener;	/* listener accepted from */
    struct ListOptions *lopts;	/* Options the client chose for /list */

    time_t last_nick_time;	/* last time when NICK used */
    time_t last_away_time;	/* last time when AWAY used */
    time_t last_join_time;	/* last time when JOIN used */
    time_t last_part_time;	/* last time when PART used */
    time_t last_knock;		/* last time when KNOCK used */

    int count_away;		/* count of /away */
    int count_nick;		/* count of /nick */
    int count_join_part;	/* count of /join | /part */
    int oper_warn_count_down;

    /*
     * Anti-flood stuff. We track how many messages were parsed and how   
     * many we were allowed in the current second, and apply a simple decay
     * to avoid flooding.
     *   -- adrian
     */

    int allow_read;		/* how many we're allowed to read in this second */
    int actually_read;		/* how many we've actually read in this second */
    int sent_parsed;		/* how many messages we've parsed in this second */

#ifdef HAVE_ENCRYPTION_ON 	
    SSL *ssl;			
#endif

    int httpflags;		/* flags of the http clients */
    int (*hparsefunc)(aClient *sptr, char *u, char *data);
    char hurl[MAXPATHLEN + 1];	/* url to be parsed */
    
    uuid_t uuid;
};

struct stats {
    unsigned short is_cbs;	/* bytes sent to clients */
    unsigned short is_cbr;	/* bytes received to clients */
    unsigned short is_sbs;	/* bytes sent to servers */
    unsigned short is_sbr;	/* bytes received to servers */

    unsigned long is_cks;	/* k-bytes sent to clients */
    unsigned long is_ckr;	/* k-bytes received to clients */
    unsigned long is_sks;	/* k-bytes sent to servers */
    unsigned long is_skr;	/* k-bytes received to servers */

    time_t is_cti;		/* time spent connected by clients */
    time_t is_sti;		/* time spent connected by servers */

    IRCU32 is_cl;		/* number of client connections */
    IRCU32 is_sv;		/* number of server connections */
    IRCU32 is_ni;		/* connection but no idea who it was */
    IRCU32 is_fake;		/* MODE 'fakes' */
    IRCU32 is_asuc;		/* successful auth requests */
    IRCU32 is_abad;		/* bad auth requests */
    IRCU32 is_udp;		/* packets recv'd on udp port */
    IRCU32 is_loc;		/* local connections made */
    IRCU32 is_ac;		/* connections accepted */
    IRCU32 is_ref;		/* accepts refused */
    IRCU32 is_unco;		/* unknown commands */
    IRCU32 is_wrdi;		/* command going in wrong direction */
    IRCU32 is_unpf;		/* unknown prefix */
    IRCU32 is_empt;		/* empty message */
    IRCU32 is_num;		/* numeric message */
    IRCU32 is_kill;		/* number of kills generated on
				 * collisions */
};

struct ChanMember {
    aClient *client_p;
    int flags;
    int bans;
};

struct Channel {

    long tsval;			/* This value is required at this place of %T */

    struct Channel *nextch;
    struct Channel *prevch;
    struct Channel *hnextch;

    int hashv;			/* raw hash value */
    int users;

    Mode mode;

    char chname[CHANNELLEN + 1];
    char topic[TOPICLEN + 1];
    char topic_nick[NICKLEN + 1];

    time_t topic_time;

    dlink_list members;

    dlink_list invites;

    dlink_list banlist;
    dlink_list invitelist;
    dlink_list banexlist;
    dlink_list stoplist;
    dlink_list chanbanlist;

    time_t firsttime_priv_recv;	/* first time when PRIVMSG received */

    int count_priv_recv;	/* received number of PRIVMSG */
    int flood_noticed;		/* Origin is informed about flood */

};

struct SearchOptions {
    int umodes;
    int langnum;
    char *nick;
    char *user;
    char *host;
    char *gcos;
    char *ip;
    char *fakehost;
    char *lang;
    aChannel *channel;
    aClient *server;
    char umode_plus:1;
    char nick_plus:1;
    char user_plus:1;
    char host_plus:1;
    char gcos_plus:1;
    char ip_plus:1;
    char fhost_plus:1;
    char lang_plus:1;
    char chan_plus:1;
    char serv_plus:1;
    char away_plus:1;
    char check_away:1;
    char check_umode:1;
};

struct Counter {
    int server;			/* servers */
    int service;		/* services */
    int myserver;		/* my servers */
    int myulined;		/* my ulined servers */
    int myservice;		/* services connected to me */
    int oper;			/* Opers */
    int chan;			/* Channels */
    int local;			/* Local Clients */
    int total;			/* total clients */
    int invisi;			/* invisible clients */
    int unknown;		/* unknown connections */
    int max_loc;		/* MAX local clients */
    int max_tot;		/* MAX global clients */
    long start;			/* when we started collecting info */
    unsigned long today;	/* Client Connections today */
    long day;			/* when today started */
    unsigned long weekly;	/* connections this week */
    long week;			/* when this week started */
    unsigned long monthly;	/* connections this month */
    long month;			/* when this month started */
    unsigned long yearly;	/* this is gonna be big */
    long year;			/* when this year started (HEH!) */
};

/* this is a struct for the module event/pathname
 * map.
 * -dsginosa
 */

struct ModEvent {
	const char *event;
	const char *modpath;
	int exists;
};

#endif /* STRUCT_H */
