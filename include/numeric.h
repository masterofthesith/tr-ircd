/*
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
 */

/* $Id: numeric.h,v 1.6 2004/02/28 22:23:34 tr-ircd Exp $ */

#define	RPL_WELCOME          001
#define	RPL_YOURHOST         002
#define	RPL_CREATED          003
#define	RPL_MYINFO           004
#define RPL_ISUPPORT         005

#define RPL_REDIR	     010

#define RPL_TRACELINK        200
#define RPL_TRACECONNECTING  201
#define RPL_TRACEHANDSHAKE   202
#define RPL_TRACEUNKNOWN     203
#define RPL_TRACEOPERATOR    204
#define RPL_TRACEUSER        205
#define RPL_TRACESERVER      206
#define RPL_TRACESERVICE     207	/* TRIRCD */
#define RPL_TRACENEWTYPE     208
#define RPL_TRACECLASS       209
#define RPL_MAP              210        /* TRIRCD */
#define RPL_ENDMAP           211        /* TRIRCD */
#define RPL_STATSCOMMANDS    212
#define RPL_STATSCLINE       213
#define RPL_STATSNLINE       214
#define RPL_STATSILINE       215
#define RPL_STATSKLINE       216
#define RPL_STATSQLINE       217
#define RPL_STATSYLINE       218
#define RPL_ENDOFSTATS       219
#define RPL_STATSPLINE	     220
#define RPL_UMODEIS          221
#define RPL_STATSBLINE	     222
#define RPL_STATSELINE	     223
#define RPL_STATSFLINE	     224
#define RPL_STATSZLINE	     225
#define RPL_STATSCOUNT	     226
#define RPL_STATSGLINE       227
#define RPL_STATSISLINE      228	/* TRIRCD */
#define RPL_STATSJLINE       229	/* TRIRCD */
#define RPL_MODLIST	     230	/* TRIRCD */
#define RPL_SERVICEINFO      231	/* TRIRCD */
#define RPL_ENDOFSERVICES    232	/* TRIRCD */
#define RPL_SERVICE          233	/* TRIRCD */
#define RPL_SERVLIST         234
#define RPL_SERVLISTEND      235
#define RPL_FDLIST	     236	/* TRIRCD */
#define RPL_HOOKLIST	     237	/* TRIRCD */
#define RPL_EVENTLIST 	     238	/* TRIRCD */
#define RPL_LOGENTLIST	     239	/* TRIRCD */
#define RPL_STATSWLINE	     240	/* TRIRCD */
#define	RPL_STATSLLINE       241
#define	RPL_STATSUPTIME      242
#define	RPL_STATSOLINE       243
#define	RPL_STATSHLINE       244
#define	RPL_STATSSLINE       245
#define RPL_STATSULINE	     246
#define RPL_STATSALINE       247 	/* TRIRCD */
#define RPL_SERVERPROTO	     248
#define	RPL_STATSDEBUG	     249
#define RPL_STATSCONN 	     250
#define	RPL_LUSERCLIENT      251
#define RPL_LUSEROP          252
#define	RPL_LUSERUNKNOWN     253
#define	RPL_LUSERCHANNELS    254
#define	RPL_LUSERME          255
#define	RPL_ADMINME          256
#define	RPL_ADMINLOC1        257
#define	RPL_ADMINLOC2        258
#define	RPL_ADMINEMAIL       259
#define RPL_STATSLINKINFO    260        /* TRIRCD */
#define	RPL_TRACELOG         261
#define RPL_ENDOFTRACE       262
#define RPL_LOAD2HI          263
#define RPL_LASTEVENT	     264	/* TRIRCD */
#define RPL_LOCALUSERS       265
#define RPL_GLOBALUSERS      266
#define RPL_ACCEPTSTATUS     267	/* TRIRCD */
#define RPL_ACCEPTLIST       268	/* TRIRCD */
#define RPL_ENDOFACCEPTLIST  269	/* TRIRCD */
#define RPL_ACCEPTINFO       270	/* TRIRCD */
#define RPL_SILELIST         271
#define RPL_ENDOFSILELIST    272

#define RPL_USINGSSL	     275	/* NOT MY IDEA: Taken from other sources */

#define	RPL_NONE             300
#define RPL_AWAY             301
#define RPL_USERHOST         302
#define RPL_ISON             303
#define RPL_USERIP           304
#define	RPL_UNAWAY           305
#define	RPL_NOWAWAY          306
#define RPL_WHOISREGNICK     307
#define RPL_WHOISADMIN       308
#define RPL_WHOISSADMIN      309
#define RPL_WHOISSVCMSG      310
#define RPL_WHOISUSER        311
#define RPL_WHOISSERVER      312
#define RPL_WHOISOPERATOR    313
#define RPL_WHOWASUSER       314
#define RPL_ENDOFWHO         315
#define RPL_WHOISCHANOP      316
#define RPL_WHOISIDLE        317
#define RPL_ENDOFWHOIS       318
#define RPL_WHOISCHANNELS    319
#define RPL_WHOWASREAL       320	/* TRIRCD */
#define RPL_LISTSTART        321
#define RPL_LIST             322
#define RPL_LISTEND          323
#define RPL_CHANNELMODEIS    324

#define RPL_CREATIONTIME     329
#define RPL_PROXYSTATS	     330	/* TRIRCD */
#define RPL_NOTOPIC          331
#define RPL_TOPIC            332
#define RPL_TOPICWHOTIME     333
#define RPL_COMMANDSYNTAX    334
#define RPL_WHOISNOPRIVMSGS  335	/* TRIRCD */
#define RPL_WHOISMODES	     336	/* TRIRCD */
#define RPL_WHOISHELPFUL     337	/* TRIRCD */
#define RPL_WHOISACTUALLY    338
#define RPL_WHOISZOMBIE	     339	/* TRIRCD */
#define RPL_PROXYSTATSTEXT   340	/* TRIRCD */
#define RPL_INVITING         341
#define	RPL_SUMMONING        342
#define RPL_CBANLIST	     343	/* TRIRCD */
#define RPL_STOPLIST	     344	/* TRIRCD */
#define RPL_ENDOFSTOPLIST    345	/* TRIRCD */
#define RPL_INVITELIST       346	/* TRIRCD */
#define RPL_ENDOFINVITELIST  347	/* TRIRCD */
#define RPL_EXCEPTIONLIST    348	/* TRIRCD */
#define RPL_ENDOFEXCEPLIST   349	/* TRIRCD */
#define RPL_ENDOFCBANLIST    350	/* TRIRCD */
#define RPL_VERSION          351
#define RPL_WHOREPLY         352
#define RPL_NAMREPLY         353
#define RPL_PROXYHELP	     354	/* TRIRCD */
#define RPL_PROXY_NEGFAIL    355	/* TRIRCD */
#define RPL_PROXY_DETECT     356	/* TRIRCD */
#define RPL_PROXY_TIMEOUT    357	/* TRIRCD */
#define RPL_PROXY_END        358	/* TRIRCD */
#define RPL_OPERPRIVILEGES   359	/* TRIRCD */
#define RPL_ENDOFPRIVLIST    360	/* TRIRCD */
#define RPL_KILLDONE         361
#define	RPL_CLOSING          362
#define RPL_CLOSEEND         363
#define RPL_LINKS            364
#define RPL_ENDOFLINKS       365
#define RPL_ENDOFNAMES       366
#define RPL_BANLIST          367
#define RPL_ENDOFBANLIST     368
#define RPL_ENDOFWHOWAS      369
#define RPL_DISPLAY	     370	/* TRIRCD */
#define	RPL_INFO             371
#define	RPL_MOTD             372
#define	RPL_INFOSTART        373
#define	RPL_ENDOFINFO        374
#define	RPL_MOTDSTART        375
#define	RPL_ENDOFMOTD        376
#define	RPL_ENDOFDISPLAY     377	/* TRIRCD */
#define RPL_REALHOST         378	/* TRIRCD */
#define RPL_DISPLAYSTART     379	/* TRIRCD */
#define RPL_RPONG	     380 	/* TRIRCD */
#define RPL_YOUREOPER        381
#define RPL_REHASHING        382
#define RPL_YOURESERVICE     383	/* TRIRCD */
#define RPL_MYPORTIS         384
#define RPL_NOTOPERANYMORE   385
#define RPL_IRCOPSSTART      386	/* TRIRCD */
#define RPL_IRCOPS           387	/* TRIRCD */
#define RPL_ENDOFIRCOPS      388	/* TRIRCD */
#define RPL_HASH	     389	/* TRIRCD */
#define RPL_ENDOFHASH	     390	/* TRIRCD */
#define RPL_TIME             391
#define	RPL_USERSSTART       392
#define	RPL_USERS            393
#define	RPL_ENDOFUSERS       394
#define	RPL_NOUSERS          395

#define ERR_NOSUCHNICK       401
#define ERR_NOSUCHSERVER     402
#define ERR_NOSUCHCHANNEL    403
#define ERR_CANNOTSENDTOCHAN 404
#define ERR_TOOMANYCHANNELS  405
#define ERR_WASNOSUCHNICK    406
#define ERR_TOOMANYTARGETS   407
#define ERR_NOCOLORSONCHAN   408
#define	ERR_NOORIGIN         409
#define ERR_YOUAREZOMBIE     410	/* TRIRCD */
#define ERR_NORECIPIENT      411
#define ERR_NOTEXTTOSEND     412
#define ERR_NOTOPLEVEL       413
#define ERR_WILDTOPLEVEL     414
#define ERR_ISINBADCHAN      415	/* TRIRCD */

#define ERR_UNKNOWNCOMMAND   421
#define	ERR_NOMOTD           422
#define	ERR_NOADMININFO      423
#define	ERR_FILEERROR        424

#define ERR_ALREADYAWAY	     428
#define ERR_TOOMANYAWAY      429
#define ERR_OLDSERVER	     430	/* TRIRCD */
#define ERR_NONICKNAMEGIVEN  431
#define ERR_ERRONEUSNICKNAME 432
#define ERR_NICKNAMEINUSE    433
#define ERR_EVALUATINGQLINE  434	/* TRIRCD */
#define ERR_BANONCHAN        435
#define	ERR_NICKCOLLISION    436
#define ERR_BANNICKCHANGE    437
#define ERR_PLEASEWAIT	     438	/* TRIRCD */

#define ERR_SERVICESDOWN     440
#define ERR_USERNOTINCHANNEL 441
#define ERR_NOTONCHANNEL     442
#define	ERR_USERONCHANNEL    443
#define ERR_NOLOGIN          444
#define	ERR_SUMMONDISABLED   445
#define ERR_USERSDISABLED    446
#define ERR_PROXY_MAX_READ   447	/* TRIRCD */
#define ERR_PROXY_NOT_BIND   448	/* TRIRCD */
#define ERR_PROXY_NO_FDS     449	/* TRIRCD */
#define ERR_PROXY_UNKNOWN    450	/* TRIRCD */
#define ERR_NOTREGISTERED    451

#define ERR_NEEDMOREPARAMS   461
#define ERR_ALREADYREGISTRED 462
#define ERR_NOPERMFORHOST    463
#define ERR_PASSWDMISMATCH   464
#define ERR_YOUREBANNEDCREEP 465
#define ERR_YOUWILLBEBANNED  466
#define	ERR_KEYSET           467
#define ERR_ONLYSERVERSCANCHANGE 468

#define ERR_CHANNELISFULL    471
#define ERR_UNKNOWNMODE      472
#define ERR_INVITEONLYCHAN   473
#define ERR_BANNEDFROMCHAN   474
#define	ERR_BADCHANNELKEY    475
#define	ERR_BADCHANMASK      476
#define ERR_NEEDREGGEDNICK   477
#define ERR_BANLISTFULL      478
#define ERR_BADCHANNAME      479
#define ERR_CHANISJUPE       480	/* TRIRCD */
#define ERR_NOPRIVILEGES     481
#define ERR_CHANOPRIVSNEEDED 482
#define	ERR_CANTKILLSERVER   483
#define ERR_DESYNC	     484
#define ERR_IPNOTRESOLVED    485	/* TRIRCD */
#define ERR_NONONREG	     486	/* TRIRCD */
#define ERR_HOSTMODERATED    487	/* TRIRCD */
#define ERR_BANNEDINCHAN     488	/* TRIRCD */
#define ERR_NOCTCPINCHAN     489	/* TRIRCD */
#define ERR_PROXY_ERROR	     490        /* TRIRCD */
#define ERR_NOOPERHOST       491

#define ERR_UMODEUNKNOWNFLAG 501
#define ERR_USERSDONTMATCH   502
#define ERR_GHOSTEDCLIENT    503
#define ERR_LAST_ERR_MSG     504

#define ERR_SILELISTFULL     511
#define ERR_TOOMANYWATCH     512
#define ERR_TOOMANYACCEPT    513	/* TRIRCD */
#define ERR_TOOMANYDCC       514

#define ERR_LISTSYNTAX       521
#define ERR_WHOSYNTAX        522
#define ERR_WHOLIMEXCEED     523
#define ERR_HELPNOTFOUND     524

#define RPL_LOGON            600
#define RPL_LOGOFF           601
#define RPL_WATCHOFF         602
#define RPL_WATCHSTAT        603
#define RPL_NOWON            604
#define RPL_NOWOFF           605
#define RPL_WATCHLIST        606
#define RPL_ENDOFWATCHLIST   607

#define RPL_DCCSTATUS        617
#define RPL_DCCLIST          618
#define RPL_ENDOFDCCLIST     619
#define RPL_DCCINFO          620

#define RPL_EWHOISUSER       660
#define RPL_EWHOISFHOST	     661  
#define RPL_EWHOISIP  	     662
#define RPL_EWHOISGCOS       663
#define RPL_EWHOISMODES      664
#define RPL_EWHOISSERVER     665
#define RPL_EWHOISACTUALLY   666
#define RPL_EWHOISCHANNELS   667
#define RPL_EWHOISREGNICK    668
#define RPL_EWHOISZOMBIE     669
#define RPL_EWHOISAWAY       670
#define RPL_EWHOISSSL        671
#define RPL_EWHOISOPERATOR   672
#define RPL_EWHOISIDLE       673
#define RPL_EWHOISMEMORY     674
#define RPL_EWHOISPREV       675
#define RPL_EWHOISNEXT       676
#define RPL_EWHOISCPORT      677
#define RPL_EWHOISLPORT      678
#define RPL_EWHOISWATCHED    679
#define RPL_EWHOISWATCHES    680
#define RPL_EWHOISSILENCES   681
#define RPL_EWHOISCLASS      682

#define RPL_HELPSTART        704
#define RPL_HELPTXT          705
#define RPL_ENDOFHELP        706

#define ERR_NUMERIC_ERR      999

