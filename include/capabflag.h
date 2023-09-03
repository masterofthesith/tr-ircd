/************************************************************************
 *   IRC - Internet Relay Chat, include/capabflag.h
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

#define MAXPARA      	    15
#define HOSTLEN             63  /* Length of hostname. */

#define HOSTIPLEN	    53

#define NICKLEN             30
#define MAX_DATE_STRING     32  /* maximum string length for a date */

#define USERLEN             10
#define REALLEN             50
#define TOPICLEN            307
#define KILLLEN             400
#define HELPLEN             400
#define CHANNELLEN          32
#define PASSWDLEN           63
#define KEYLEN              23
#define BUFSIZE             512 /* WARNING: *DONT* CHANGE THIS!!!! */
#define MAXRECIPIENTS       20
#define MAXBANS             100
#define MAXINVITES          100
#define MAXEXCEPTIONS       100
#define MOTDLINELEN         90
#define MAXSILES            10
#define MAXSILELENGTH       128
#define MAXDCCALLOW         10
#define MAXCHANBANS         45
#define MAXSTOPMSGS         45   
#define MAXACCEPTED	    20
#define MAXKILLS            20  
#define MAXWATCH            128   
#define MAXBANLENGTH        1024

#define MODEBUFLEN          200 
#define REALMODEBUFLEN      512     /* max actual modebuf */
#define MAXMODEPARAMS       (MAXPARA-3)

#define USERHOST_REPLYLEN       (NICKLEN+HOSTLEN+USERLEN+5)

#define ID_MAP_SIZE 	    4096        /* size of the identity hashmap */

#define IP_HASH_SIZE 	    0x1000

#define MIN_SPAM_NUM 5
#define MIN_SPAM_TIME 60

#define CLIENT_LOCAL_SIZE sizeof(aClient)
#define CLIENT_REMOTE_SIZE offsetof(aClient, count)
// #define CLIENT_REMOTE_SIZE sizeof(aClient)

#define USERID_PART(x)          (x & 0xFFFFF)
#define SERVERID_PART(x)        ((x >> 24) & 0xFFF)
#define MAKE_ID(sid, uid)       (((sid << 24) & 0xFFF00000) | (uid & 0xFFFFF))


/* status information */

#define STAT_CONNECTING -4
#define STAT_HANDSHAKE  -3  
#define STAT_ME         -2
#define STAT_UNKNOWN    -1
#define STAT_SERVER     0
#define STAT_CLIENT     1
#define STAT_SERVICE    2

#define IsRegisteredUser(x)     ((x)->status == STAT_CLIENT)
#define IsRegistered(x)         ((x)->status >= STAT_SERVER)
#define IsConnecting(x)         ((x)->status == STAT_CONNECTING)
#define IsHandshake(x)          ((x)->status == STAT_HANDSHAKE)
#define IsMe(x)                 ((x)->status == STAT_ME)          
#define IsUnknown(x)            ((x)->status == STAT_UNKNOWN)
#define IsServer(x)             ((x)->status == STAT_SERVER)          
#define IsClient(x)             ((x)->status == STAT_CLIENT)
#define IsService(x)            ((x)->status == STAT_SERVICE)         

#define SetConnecting(x)        ((x)->status = STAT_CONNECTING)
#define SetHandshake(x)         ((x)->status = STAT_HANDSHAKE)
#define SetMe(x)                ((x)->status = STAT_ME)
#define SetUnknown(x)           ((x)->status = STAT_UNKNOWN)
#define SetServer(x)            ((x)->status = STAT_SERVER)
#define SetClient(x)            ((x)->status = STAT_CLIENT)
#define SetService(x)           ((x)->status = STAT_SERVICE)

#define MyConnect(x)            ((x)->fd >= 0)
#define MyClient(x)             (MyConnect(x) && IsClient(x))
#define IsPerson(x)             ((x)->user && IsClient(x))
#define IsPrivileged(x)         (IsAnOper(x) || IsServer(x) || IsService(x) || IsULine(x))


/* client flags */

#define FLAGS_PINGSENT     0x000001     /* Unreplied ping sent */
#define FLAGS_DEADSOCKET   0x000002     /* Local socket is dead */
#define FLAGS_KILLED       0x000004     /* Prevents "QUIT" from being
                                         * sent for this */
#define FLAGS_CLOSING      0x000008     /* set when closing to suppress
                                         * errors */
#define FLAGS_CHKACCESS    0x000010     /* ok to check clients access */
#define FLAGS_GOTID        0x000020     /* successful ident lookup
                                         * achieved */
#define FLAGS_SOBSENT      0x000040     /* we've sent an SOB, just have
                                         * to send an EOB */
#define FLAGS_EOBRECV      0x000080     /* we're waiting on an EOB */

#define FLAGS_DOID         0x000100     /* I-lines say must use ident */
#define FLAGS_NORMALEX     0x000200     /* Client exited normally */
#define FLAGS_SENDQEX      0x000400     /* Sendq exceeded */
#define FLAGS_HTTPCLIENT   0x000800	/* This link belongs to a httpd connection */
#define FLAGS_SERV_NEGO    0x001000     /* This is a server that has
                                         * passed connection tests, but
                                         * is a stat < 0 for handshake
                                         * purposes */
#define FLAGS_USERBURST    0x002000     /* server in nick/channel netburst */
#define FLAGS_TOPICBURST   0x004000     /* server in topic netburst */
#define FLAGS_RC4IN        0x008000     /* This link is rc4 encrypted. */
#define FLAGS_RC4OUT       0x010000     /* This link is rc4 encrypted. */
#define FLAGS_ZIPPED_IN    0x020000     /* This link is gzipped. */
#define FLAGS_ZIPPED_OUT   0x040000     /* This link is gzipped. */
#define FLAGS_EXITED	   0x080000	/* This client's exit has been completed */

#define FLAGS_SSL	   0x100000	/* This link is ssl encrypted. */

#define FLAGS_BURST     (FLAGS_USERBURST | FLAGS_TOPICBURST)

#define SetAccess(x)            ((x)->flags |= FLAGS_CHKACCESS)            
#define SetNegoServer(x)        ((x)->flags |= FLAGS_SERV_NEGO)            
#define SetRC4OUT(x)            ((x)->flags |= FLAGS_RC4OUT)
#define SetRC4IN(x)             ((x)->flags |= FLAGS_RC4IN)
#define SetZipIn(x)             ((x)->flags |= FLAGS_ZIPPED_IN)
#define SetZipOut(x)            ((x)->flags |= FLAGS_ZIPPED_OUT)
#define SetGotId(x)             ((x)->flags |= FLAGS_GOTID)
#define SetNeedId(x)            ((x)->flags |= FLAGS_DOID)
#define SetExited(x)		((x)->flags |= FLAGS_EXITED)
#define SetSSL(x)		((x)->flags |= FLAGS_SSL)
#define SetHttpClient(x)	((x)->flags |= FLAGS_HTTPCLIENT)
#define SetClosing(x)		((x)->flags |= FLAGS_CLOSING)

#define IsSSL(x)		(((x)->flags & FLAGS_SSL) && (x)->ssl)
#define IsGotId(x)              ((x)->flags & FLAGS_GOTID)
#define IsNeedId(x)             ((x)->flags & FLAGS_DOID)
#define IsListening(x)          ((x)->flags & FLAGS_LISTEN)
#define IsDead(x)               ((x)->flags & FLAGS_DEADSOCKET)
#define IsNegoServer(x)         ((x)->flags & FLAGS_SERV_NEGO)
#define IsRC4OUT(x)             ((x)->flags & FLAGS_RC4OUT)
#define IsRC4IN(x)              ((x)->flags & FLAGS_RC4IN)
#define ZipIn(x)                ((x)->flags & FLAGS_ZIPPED_IN)
#define ZipOut(x)               ((x)->flags & FLAGS_ZIPPED_OUT)
#define IsExited(x)		((x)->flags & FLAGS_EXITED)
#define IsHttpClient(x)		((x)->flags & FLAGS_HTTPCLIENT)
#define IsClosing(x)		((x)->flags & FLAGS_CLOSING)

#define ClearNegoServer(x)      ((x)->flags &= ~FLAGS_SERV_NEGO)
#define ClearAccess(x)          ((x)->flags &= ~FLAGS_CHKACCESS)

#define RC4EncLink(x)           (((x)->flags & (FLAGS_RC4IN|FLAGS_RC4OUT)) == (FLAGS_RC4IN|FLAGS_RC4OUT))

/* Protoflags - additional client flags */
#define PFLAGS_TS7         	0x000001
#define PFLAGS_ULINE       	0x000002     	/* client is U-lined */       
#define PFLAGS_ISHUB       	0x000004     	/* this link is a hub */
#define PFLAGS_HASIDENTITY 	0x000008     	/* cptr->id.id is a valid sid/uid pair */
#define PFLAGS_TIME_IS_SET 	0x000010     	/* This link received SVINFO TIME */
#define PFLAGS_DODKEY      	0x000020     	/* This link is willing Diffie Hellman */
#define PFLAGS_DOZIP       	0x000040     	/* This link is willing gzip */
#define PFLAGS_DOHIDENAME  	0x000080     	/* This link is willing its name hidden */
#define PFLAGS_FLOODDONE	0x000100	/* This client exited floodding session */
#define PFLAGS_NICKISSENT	0x000200	/* This client has been introduced */
#define PFLAGS_ZOMBIE	   	0x000400	/* This client is a zombie */
#define PFLAGS_ANONYMOUS   	0x000800	/* This client is an anonymous client */
#define PFLAGS_OPERMODE	   	0x001000	/* This client used an /operdo or /samode */
#define PFLAGS_ALIASED	   	0x002000	/* This client wants the aliasname */
#define PFLAGS_DCCNOTICE   	0x004000	/* This client has seen the DCC notice */
#define PFLAGS_IPV6HOST	   	0x008000	/* This client has Ipv6 Ip */
#define PFLAGS_NONRESOLVED 	0x010000	/* This clients hostname is not resolved */
#define PFLAGS_HASRESTBUF  	0x020000	/* This client has the restbuf pointer */
#define PFLAGS_LOCALCLIENT 	0x040000	/* This client is a local client */
#define PFLAGS_OPERFAKEHOST	0x080000	/* This client uses oper fakehosts */

/* The following items are also set in the ->protoflags member of a client
 * structure. Please keep this in mind when adding more PFLAGS_ 
 * -TimeMr14C
 */

#define DO_USER_DEOP	   	0x40000000
#define DO_USER_JOIN	   	0x80000000

#define SetHasID(x)             ((x)->protoflags |= PFLAGS_HASIDENTITY)
#define SetOperMode(x)		((x)->protoflags |= PFLAGS_OPERMODE)

#define HasID(x)                ((x)->protoflags & PFLAGS_HASIDENTITY)
#define IsHub(x)                ((x)->protoflags & PFLAGS_ISHUB)
#define IsULine(x)              ((x)->protoflags & PFLAGS_ULINE)
#define DoZipThis(x)            ((x)->protoflags & PFLAGS_DOZIP)
#define WantDKEY(x)             ((x)->protoflags & PFLAGS_DODKEY)
#define IsTS7(x)                ((x)->protoflags & PFLAGS_TS7)
#define WillHideName(x)		((x)->protoflags & PFLAGS_DOHIDENAME)
#define IsZombie(x)             ((x)->protoflags & PFLAGS_ZOMBIE)
#define IsAnon(x)		((x)->protoflags & PFLAGS_ANONYMOUS)
#define UsesAlias(x)		((x)->protoflags & PFLAGS_ALIASED)
#define SeenDCCNotice(x)        ((x)->protoflags & PFLAGS_DCCNOTICE)
#define IsClientIpv6(x)		((x)->protoflags & PFLAGS_IPV6HOST)
#define IsNonResolved(x)	((x)->protoflags & PFLAGS_NONRESOLVED)
#define HasRestbuf(x)		((x)->protoflags & PFLAGS_HASRESTBUF)
#define IsLocalClient(x)	((x)->protoflags & PFLAGS_LOCALCLIENT)
#define IsOperFakehost(x)	((x)->protoflags & PFLAGS_OPERFAKEHOST)
#define IsFloodDone(x)		((x)->protoflags & PFLAGS_FLOODDONE)
#define NickIsSent(x)		((x)->protoflags & PFLAGS_NICKISSENT)

#define UnsetHasID(x)           ((x)->protoflags &= ~PFLAGS_HASIDENTITY)
#define ClearZombie(x)          ((x)->protoflags &= ~PFLAGS_ZOMBIE)
#define ClearOperMode(x)	((x)->protoflags &= ~PFLAGS_OPERMODE)

#define IsOperMode(x)           (((x)->protoflags & PFLAGS_OPERMODE) || (IsAnOper(x) && !MyClient(x)))

	/* The line above IS evil magic (tm). We trust remote when receiving MODE
	 * from an operator. (/operdo MODE sends only MODE)
	 * -TimeMr14C
	 */

/* Capabilities from Bahamut */

#define CAPAB_DKEY      0x0000001       /* server supports dh-key exchange */
#define CAPAB_ZIP       0x0000002       /* server supports gz'd links */
#define CAPAB_NICKIP    0x0000004       /* IP in the NICK line? */

/* Capabilities from TR-IRCD4/Chimera */

#define CAPAB_SERVICES  0x0000008       /* Server uses SERVICE */

/* Capabilities from TR-IRCD5/Kenora */

#define CAPAB_TOKEN1    0x0000010       /* Token V1 Protocol change */  
#define CAPAB_IDENT     0x0000020       /* base64 !prefixed idents */
#define CAPAB_HIDENAME  0x0000040	/* server wants hidden names */
#define CAPAB_EXCAP	0x0000080	/* server understands EXCAP */

#define IsIDCapable(x)  ((x)->capabilities & CAPAB_IDENT)
#define CanDoDKEY(x)    ((x)->capabilities & CAPAB_DKEY)
#define IsZipCapable(x) ((x)->capabilities & CAPAB_ZIP)
#define IsNICKIP(x)     ((x)->capabilities & CAPAB_NICKIP)
#define HasServices(x)  ((x)->capabilities & CAPAB_SERVICES)
#define IsToken1(x)     ((x)->capabilities & CAPAB_TOKEN1)
#define IsExcap(x)	((x)->capabilities & CAPAB_EXCAP)

#define CAPAB_JOINDLY	0x0000100	/* server understands chanmode +j */

#define UseJoinDelay(x)	((x)->capabilities & CAPAB_JOINDLY)

/*
 * defines for check_ctcp results 
 */
#define CTCP_NONE       0
#define CTCP_YES        1
#define CTCP_DCC        2
#define CTCP_DCCSEND    3

/* usermodes */

#define UMODE_o     	0x0001      	/* umode +o - Oper */
#define UMODE_n     	0x0002      	/* Umode +n - routing messages */
#define UMODE_i     	0x0004      	/* umode +i - Invisible */
#define UMODE_w     	0x0008      	/* umode +w - hide channels in whois */
#define UMODE_r     	0x0010      	/* umode +r - registered nick */
#define UMODE_a     	0x0020      	/* umode +a - Services Admin */
#define UMODE_A     	0x0040      	/* umode +A - Server Admin */
#define UMODE_x     	0x0080      	/* umode +x - hidden hostnames */
#define UMODE_R     	0x0100      	/* umode +R - listen to registered */
#define UMODE_t	    	0x0200	    	/* umode +t - Translate into greeklish */
#define UMODE_h	    	0x0400	    	/* umode +h - helper */
#define UMODE_H	    	0x0800	    	/* umode +H - oper sees hidden things */
#define UMODE_p	    	0x1000	    	/* umode +p - hide idle time and signon from /whois */
#define UMODE_c		0x2000	    	/* umode +c - Do not receive colorful messages */
#define UMODE_C		0x4000		/* umode +C - Do not accept ctcp messages */
#define UMODE_P		0x8000		/* umode +P - No private messages */
#define UMODE_z		0x10000		/* umode +z - Dccallow everyone */

#define SEND_UMODES	0x17FFD
#define OPER_UMODES 	0x000B
#define USER_UMODES	0x1FFFF

#define IsInvisible(x)          ((x)->umode & UMODE_i)
#define IsAnOper(x)             ((x)->umode & UMODE_o)            
#define IsRegNick(x)            ((x)->umode & UMODE_r)
#define IsWhoisHideChan(x)	((x)->umode & UMODE_w)
#define IsSAdmin(x)             ((x)->umode & UMODE_a)
#define IsAdmin(x)              ((x)->umode & UMODE_A)
#define IsNoNonReg(x)           ((x)->umode & UMODE_R)
#define IsHelpop(x)		((x)->umode & UMODE_h)
#define IsFake(x)               ((x)->umode & UMODE_x)
#define IsRouting(x)		((x)->umode & UMODE_n)
#define IsForeigner(x)		((x)->umode & UMODE_t)
#define IsPrivate(x)		((x)->umode & UMODE_p)
#define IsNoColor(x)		((x)->umode & UMODE_c)
#define IsNoCTCP(x)		((x)->umode & UMODE_C)
#define IsDenyPrivmsg(x)	((x)->umode & UMODE_P)
#define IsDccallowAll(x)	((x)->umode & UMODE_z)

#define IsSeeHidden(x)		(((x)->umode & UMODE_H) || IsSAdmin(x) || IsAdmin(x))

#define SetOper(x)              ((x)->umode |= UMODE_o)

/* operator flags */

#define OFLAG_DIE       0x0001      	/* Oper can /die the server */     
#define OFLAG_RESTART   0x0002      	/* Oper can /restart */    
#define OFLAG_ADMIN     0x0004      	/* Oper is the server admin */
#define OFLAG_SADMIN    0x0008      	/* Oper is a services admin */
#define OFLAG_GKLINE	0x0010		/* Oper can do /gline or /kline */
#define OFLAG_OPKILL	0x0020		/* Oper can do /kill */
#define OFLAG_OPERDO	0x0040		/* Oper can do /operdo */
#define OFLAG_SEEHIDDEN	0x0080		/* Oper can see hidden things: realhost, secret channels */

#define OPCanDie(x)             ((x)->oflag & OFLAG_DIE)
#define OPCanRestart(x)         ((x)->oflag & OFLAG_RESTART)
#define OPIsAdmin(x)            ((x)->oflag & OFLAG_ADMIN)
#define OPIsSAdmin(x)           ((x)->oflag & OFLAG_SADMIN)
#define OPIsGKLine(x)		((x)->oflag & OFLAG_GKLINE)
#define OPIsOpKill(x)		((x)->oflag & OFLAG_OPKILL)
#define OPIsOperdo(x)		((x)->oflag & OFLAG_OPERDO)
#define OPIsSeeHidden(x)	((x)->oflag & OFLAG_SEEHIDDEN)

/*
 * for sendto_lev
 */

#define CCONN_LEV       1
#define REJ_LEV         2
#define SKILL_LEV       3
#define SPY_LEV         4
#define DEBUG_LEV       5
#define FLOOD_LEV       6
#define SPAM_LEV        7
#define DCCSEND_LEV     8
#define SERVICE_LEV     9       /* TRIRCD */
#define SNOTICE_LEV     10      /* TRIRCD */
#define PROXY_LEV	11	/* TRIRCD */
#define KLINE_LEV	12	/* TRIRCD */
#define QLINE_LEV	13	/* TRIRCD */
#define ULINE_LEV       14      /* TRIRCD */

#define NUM_LEV         16      /* TRIRCD - +2 to the last _LEV */

#define T_CCONN_LEV     "SERVER Channel: Clients connecting and exiting"
#define T_REJ_LEV       "SERVER Channel: Rejected connections"
#define T_SKILL_LEV     "SERVER Channel: Server Kills"
#define T_SPY_LEV       "SERVER Channel: Issued server query commands"
#define T_DCCSEND_LEV   "SERVER Channel: Blocked dcc sends"
#define T_FLOOD_LEV     "SERVER Channel: Flood notices"
#define T_SPAM_LEV      "SERVER Channel: Possible Spambots"
#define T_DEBUG_LEV     "SERVER Channel: Debug information notices"
#define T_NOTICE_LEV    "SERVER Channel: Server notices - former umode +s"
#define T_SERVICE_LEV   "SERVER Channel: Notices about connecting services"
#define T_SNOTICE_LEV   "SERVER Channel: Server notices - former umode +so"
#define T_PROXY_LEV	"SERVER Channel: Notices from the Proxy Scan Monitor"
#define T_KLINE_LEV     "SERVER Channel: Kill Lines"
#define T_QLINE_LEV     "SERVER Channel: Rejected quarantined nicknames"

#define N_NOTICE        "&NOTICE"
#define N_CONNECT       "&CONNECTS"
#define N_REJECT        "&REJECTED"
#define N_KILL          "&KILLS"
#define N_STAT          "&STATS"      
#define N_DCCSEND       "&DCCSEND"
#define N_FLOOD         "&FLOOD"
#define N_SPAMBOT       "&SPAMBOTS"
#define N_DEBUG         "&DEBUG"
#define N_SERVICE       "&SERVICES"
#define N_SNOTICE       "&SNOTICE"
#define N_PROXY		"&PROXYMON"
#define N_KLINE         "&KLINES"
#define N_QLINE         "&QLINES"
#define N_ULINE         "#services"

/* aconfitem -> status */

#define CONF_CLIENT             0x0001
#define CONF_SERVER             0x0002
#define CONF_SERVICE            0x0004
#define CONF_OPERATOR           0x0008

#define CONF_ILLEGAL            0x80000000

#define CONF_CLIENT_MASK        (CONF_CLIENT|CONF_SERVICE|CONF_SERVER)

#define IsIllegal(x)            ((x)->status & CONF_ILLEGAL)

/* confitem flags */

#define CONF_FLAGS_EXEMPTKLINE          0x00000001
#define CONF_FLAGS_NOLIMIT              0x00000002
#define CONF_FLAGS_SPOOF_IP             0x00000004
#define CONF_FLAGS_SPOOF_NOTICE         0x00000008
#define CONF_FLAGS_REDIR                0x00000010
#define CONF_FLAGS_ULTIMATE             0x00000020
#define CONF_FLAGS_ALLOW_AUTO_CONN      0x00000040
#define CONF_FLAGS_ENCRYPTED            0x00000080
#define CONF_FLAGS_COMPRESSED           0x00000100
#define CONF_FLAGS_HUB                  0x00000200
#define CONF_FLAGS_WEBLOGIN		0x00000400
#define CONF_FLAGS_ALLOW_FAKEHOST	0x00000800

#define IsConfExemptKline(x)    	((x)->flags & CONF_FLAGS_EXEMPTKLINE)
#define IsConfExemptLimits(x)   	((x)->flags & CONF_FLAGS_NOLIMIT)
#define IsConfDoSpoofIp(x)      	((x)->flags & CONF_FLAGS_SPOOF_IP)
#define IsConfCanChooseFakehost(x)	((x)->flags & CONF_FLAGS_ALLOW_FAKEHOST)
#define IsConfSpoofNotice(x)   		((x)->flags & CONF_FLAGS_SPOOF_NOTICE)   
#define IsConfEncrypted(x)      	((x)->flags & CONF_FLAGS_ENCRYPTED)
#define IsConfCompressed(x)     	((x)->flags & CONF_FLAGS_COMPRESSED)
#define IsConfHub(x)            	((x)->flags & CONF_FLAGS_HUB)
#define IsConfUltimate(x)       	((x)->flags & CONF_FLAGS_ULTIMATE)  
#define IsConfWeblogin(x)		((x)->flags & CONF_FLAGS_WEBLOGIN)

/* service's */

#define SMODE_U         0x001
#define SMODE_A         0x002

#define SERVICE_SEE_PREFIX      0x000001
#define SERVICE_SEE_OPERS       0x000002
#define SERVICE_SEE_NICKS       0x000004
#define SERVICE_SEE_QUITS       0x000008
#define SERVICE_SEE_KILLS       0x000010
#define SERVICE_SEE_SERVERS     0x000020
#define SERVICE_SEE_SERVICES    0x000040
#define SERVICE_SEE_SQUITS      0x000080
#define SERVICE_SEE_RNICKS      0x000100                    
#define SERVICE_SEE_UMODES      0x000200
#define SERVICE_SEE_JOINS       0x000400                 
#define SERVICE_SEE_KICKS       0x000800
#define SERVICE_SEE_PARTS       0x001000
#define SERVICE_SEE_MODES       0x002000
#define SERVICE_SEE_TOPIC       0x004000

/* TS7 and Timestamp flags */

#define TS_CURRENT      7       /* current TS protocol version */
#define TS_MIN          3       /* minimum supported TS protocol */
#define TS_DOESTS       0x20000000      /* What the hell is that ?
                                         * -TimeMr14C */
#define DoesTS(x)       ((x)->tsval == TS_DOESTS)


/* channel user modes */

#define CHFL_CHANOP     0x0001
#define CHFL_VOICE      0x0002
#define CHFL_PROTECT    0x0004
#define CHFL_OWNER      0x0008
#define CHFL_HALFOP     0x0010
#define CHFL_RESANON	0x8000	/* required to reverse the effect of +x */

/* chanmode command */ 

#define CMODE_LIST 0
#define CMODE_ADD  1
#define CMODE_DEL  2

/* parametric mode types */

#define MDFL_BAN	0x0001 
#define MDFL_EXCEPT	0x0002 
#define MDFL_INVITE	0x0004 
#define MDFL_STOPMSG	0x0008
#define MDFL_CHANBAN	0x0010

/* parameter types */

#define MTYP_FULL      0x01     /* mask is nick!user@host */
#define MTYP_USERHOST  0x02     /* mask is user@host */
#define MTYP_HOST      0x04     /* mask is host only */
#define MTYP_CHANNEL   0x08     /* mask is a #channel */

/* single letter modes */

#define MODE_PRIVATE    	0x0001		/* +p : hidden in /list */
#define MODE_SECRET     	0x0002		/* +s : hidden in /whois */
#define MODE_MODERATED  	0x0004		/* +m : moderated */
#define MODE_TOPICLIMIT 	0x0008		/* +t : only ops can /topic */
#define MODE_INVITEONLY 	0x0010		/* +i : invite only */
#define MODE_NOPRIVMSGS 	0x0020		/* +n : only members can /msg */
#define MODE_KEY        	0x0040		/* +k : need key to /join */
#define MODE_FLOOD		0x0080		/* +f : flood protection in /msg */
#define MODE_LIMIT      	0x0100		/* +l : limited number of users */
#define MODE_REGISTERED 	0x0200		/* +r : registered by services */
#define MODE_REGONLY    	0x0400		/* +R : only registered nicks allowed */
#define MODE_NOCOLOR    	0x0800		/* +c : no color chars allowed */
#define MODE_OPERONLY   	0x1000		/* +O : only operators allowed */
#define MODE_EXTOPIC    	0x2000		/* +T : only protected can /topic */
#define MODE_HIDEOPS    	0x4000		/* +x : no chanmber prefixes (.@%+) */
#define MODE_ANONYMOUS  	0x8000		/* +q : channel is anonymous and quiet */
#define MODE_NONONRES		0x10000		/* +N : only users with resolved hostnames allowed */
#define MODE_NOCTCP		0x20000		/* +C : no ctcp can be directed to the channel */
#define MODE_LINKED		0x40000		/* +L : channel is linked to another */
#define MODE_NOFILTER   	0x80000 	/* +w : channel does not use wordfilters */
#define MODE_SENDREGONLY	0x100000	/* +g : only +r users can send to channel */
#define MODE_NAMESONLYMEMBERS	0x200000	/* +j : only members can use /names or /who +c */
#define MODE_HIDEPARTQUIT	0x400000	/* +d : hide part and quit messages in channel */
#define MODE_NOKNOCK		0x800000	/* +K : no /knock allowed */
#define MODE_JOINDELAY		0x1000000	/* +J : Users that parted are required to wait jd seconds */

#define CHFL_SPEAKABLE  (CHFL_CHANOP|CHFL_VOICE|CHFL_OWNER|CHFL_PROTECT|CHFL_HALFOP)
#define CHFL_GETSETTER  (CHFL_CHANOP|CHFL_OWNER|CHFL_PROTECT|CHFL_HALFOP)

#define MODE_CHANMODE		0x1FFFFFF

/* modeitem flags */

#define MFLG_LISTABLE		0x0001		/* mode can be used to list the setted */
#define MFLG_PARAMSTR		0x0002		/* mode has a string parameter */
#define MFLG_PARAMNUM		0x0004		/* mode has a number parameter */
#define MFLG_PARAMDBL		0x0008		/* mode has a flood parameter */
#define MFLG_PARAMCHN		0x0010		/* mode has a channel as a parameter */
#define MFLG_IGNORE		0x0020		/* mode should not appear in /mode #channel */

/* chanmode types */

#define CMTYPE_SINGLE		0x01		/* mode letter is single */
#define CMTYPE_PARAMETRIC	0x02		/* mode letter is parametric */
#define CMTYPE_REMWPARA		0x04		/* mode letter is removed with parametres */
#define CMTYPE_REMWOPARA	0x08		/* mode letter is removed without params */
#define CMTYPE_LIST		0x10		/* mode letter is for a list */

/* channel types */

#define SecretChannel(x)        ((x) && ((x)->mode.mode & MODE_SECRET))
#define HiddenChannel(x)        ((x) && ((x)->mode.mode & MODE_PRIVATE))
#define NamesChannel(x)		((x) && ((x)->mode.mode & MODE_NAMESONLYMEMBERS))
#define PubChannel(x)           (!(x) || ((x)->mode.mode & (MODE_PRIVATE | MODE_SECRET)) == 0)
#define PubChannelNames(x)	(!(x) || ((x)->mode.mode & (MODE_PRIVATE | MODE_SECRET | MODE_NAMESONLYMEMBERS)) == 0)
#define ShowChannel(v,c)        (PubChannel(c) || IsMember((v),(c)))
#define ShowChannelNames(v,c)	(PubChannelNames(c) || IsMember((v),(c)))

#define IsChanModerated(x)      ((x)->mode.mode & MODE_MODERATED)
#define IsChanOpTopic(x)        ((x)->mode.mode & MODE_TOPICLIMIT)
#define IsChanInviteonly(x)     ((x)->mode.mode & MODE_INVITEONLY)
#define IsChanNoPriv(x)         ((x)->mode.mode & MODE_NOPRIVMSGS)
#define IsChanKey(x)            ((x)->mode.mode & MODE_KEY)
#define IsChanLimited(x)        ((x)->mode.mode & MODE_LIMIT)
#define IsChanRegistered(x)     ((x)->mode.mode & MODE_REGISTERED)
#define IsChanRegOnly(x)        ((x)->mode.mode & MODE_REGONLY)
#define IsChanSendRegOnly(x)	((x)->mode.mode & MODE_SENDREGONLY)
#define IsChanNoColor(x)        ((x)->mode.mode & MODE_NOCOLOR)
#define IsChanOperOnly(x)       ((x)->mode.mode & MODE_OPERONLY)
#define IsChanAnon(x)           ((x)->mode.mode & MODE_ANONYMOUS)
#define IsChanHideOps(x)        ((x)->mode.mode & MODE_HIDEOPS)
#define IsChanExTopic(x)        ((x)->mode.mode & MODE_EXTOPIC)
#define IsChanFlood(x)		((x)->mode.mode & MODE_FLOOD)
#define IsChanNoNonres(x)	((x)->mode.mode & MODE_NONONRES)
#define IsChanNoCTCP(x)		((x)->mode.mode & MODE_NOCTCP)
#define IsChanLinked(x)		((x)->mode.mode & MODE_LINKED)
#define IsChanNoFilter(x)       ((x)->mode.mode & MODE_NOFILTER)
#define IsChanHidePartQuit(x)	((x)->mode.mode & MODE_HIDEPARTQUIT)
#define IsChanNoKnock(x)	((x)->mode.mode & MODE_NOKNOCK)
#define IsChanJoindelay(x)	((x)->mode.mode & MODE_JOINDELAY)

#define IsChannelName(name)     ((name) && (*(name) == '#' || *(name) == '&'))
#define IsLocalChan(x)          ((x) && (*(x) == '&'))
#define IsGlobalChan(x)         ((x) && (*(x) == '#'))

/*
 * return values for hunt_server() 
 */

#define HUNTED_NOSUCH   (-1)    /* if the hunted server is not found */
#define HUNTED_ISME     0       /* if this server should execute the
                                 * command */
#define HUNTED_PASS     1       /* if message passed onwards
                                 * successfully */

/*
 * used when sending to #mask or $mask 
 */

#define MATCH_SERVER  1
#define MATCH_HOST    2

/*
 * misc defines 
 */

#define ZIP_NEXT_BUFFER -5
#define RC4_NEXT_BUFFER -4
#define FLUSH_BUFFER    -3
#define CLIENT_EXITED    -2

/*
 * ConfigElement flags
 */

#define CE_STRING	0x001
#define CE_DECIMAL	0x002
#define CE_DATE		0x004
#define CE_BOOLEAN	0x008
#define CE_BOOLEAN_YN	0x010

/* Proxymon defines */

#define PROXYMON_KLINE          0x01
#define PROXYMON_AUTOKILL       0x02
#define PROXYMON_GNOTICE        0x04

/*
 * definitions for get_client_name
 */

#define HIDE_IP 0
#define SHOW_IP 1
#define MASK_IP 2

/* definitions for check functions */

#define NOT_AUTHORIZED          (-1)
#define SOCKET_ERROR            (-2)
#define I_LINE_FULL             (-3)
#define TOO_MANY                (-4)
#define BANNED_CLIENT           (-5)
#define TOO_FAST                (-6)
#define INVALID_CONNECTION      (-7)

/* maskfile flags */

#define MASKITEM_KLINE			1
#define MASKITEM_AUTOKILL		2
#define MASKITEM_QUARANTINE		3
#define MASKITEM_GECOS			4
#define MASKITEM_ZAPLINE		5
#define MASKITEM_EXCLUDE		6
#define MASKITEM_JUPITER		7
#define MASKITEM_KLINE_CONFIG		8
#define MASKITEM_QUARANTINE_CONFIG 	9
#define MASKITEM_GECOS_CONFIG		10
#define MASKITEM_ZAPLINE_CONFIG		11
#define MASKITEM_JUPITER_CONFIG		12
#define MASKITEM_KLINE_REGEX		13
#define MASKITEM_QUARANTINE_REGEX	14
#define MASKITEM_GECOS_REGEX		15
#define MASKITEM_JUPITER_REGEX		16

#define MASKITEM_MASK			16	/* num of maskitems */

/* hash tables */
#define FNV1_32_INIT 0x811c9dc5
#define FNV1_32_BITS 16
#define FNV1_32_SIZE (1 << FNV1_32_BITS)  /* 2^16 = 65536 */
#define HASHSIZE FNV1_32_SIZE

#define MASK_MAX 	65536		/* maskitem */
#define WATCHHASHSIZE   10007		/* watch */

/* socket errors */

#define IRCERR_ZIP	-12
#define IRCERR_SSL	-13

