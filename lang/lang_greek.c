#include "struct.h"
#include "numeric.h"
#include "h.h"
#include "language.h"

char *lang_greek[] = {
    /*
     * 000 
     */ NULL,
    /*
     * 001 RPL_WELCOME 
     */ ":%C 001 %s :���������� ��� %s IRC ������ %s!%s@%s",
    /*
     * 002 RPL_YOURHOST 
     */ ":%C 002 %s :������� ������������ ���� ���������� %s, � ������ ��� ������ ����� %s",
    /*
     * 003 RPL_CREATED 
     */ ":%C 003 %s :� ����������� ����� ������������� %s",
    /*
     * 004 RPL_MYINFO 
     */ NULL,
    /*
     * 005 RPL_ISUPPORT
     */ ":%C 005 %s %s :����� ��������� ���� ������������ ����������",
    /*
     * 006 
     */ NULL,
    /*
     * 007 
     */ NULL,
    /*
     * 008 
     */ NULL,
    /*
     * 009 
     */ NULL,
    /*
     * 010 
     */ NULL,
    /*
     * 011 
     */ NULL,
    /*
     * 012 
     */ NULL,
    /*
     * 013 
     */ NULL,
    /*
     * 014 
     */ NULL,
    /*
     * 015 
     */ NULL,
    /*
     * 016 
     */ NULL,
    /*
     * 017 
     */ NULL,
    /*
     * 018 
     */ NULL,
    /*
     * 019 
     */ NULL,
    /*
     * 020 
     */ NULL,
    /*
     * 021 
     */ NULL,
    /*
     * 022 
     */ NULL,
    /*
     * 023 
     */ NULL,
    /*
     * 024 
     */ NULL,
    /*
     * 025 
     */ NULL,
    /*
     * 026 
     */ NULL,
    /*
     * 027 
     */ NULL,
    /*
     * 028 
     */ NULL,
    /*
     * 029 
     */ NULL,
    /*
     * 030 
     */ NULL,
    /*
     * 031 
     */ NULL,
    /*
     * 032 
     */ NULL,
    /*
     * 033 
     */ NULL,
    /*
     * 034 
     */ NULL,
    /*
     * 035 
     */ NULL,
    /*
     * 036 
     */ NULL,
    /*
     * 037 
     */ NULL,
    /*
     * 038 
     */ NULL,
    /*
     * 039 
     */ NULL,
    /*
     * 040 
     */ NULL,
    /*
     * 041 
     */ NULL,
    /*
     * 042 
     */ NULL,
    /*
     * 043 
     */ NULL,
    /*
     * 044 
     */ NULL,
    /*
     * 045 
     */ NULL,
    /*
     * 046 
     */ NULL,
    /*
     * 047 
     */ NULL,
    /*
     * 048 
     */ NULL,
    /*
     * 049 
     */ NULL,
    /*
     * 050 
     */ NULL,
    /*
     * 051 
     */ NULL,
    /*
     * 052 
     */ NULL,
    /*
     * 053 
     */ NULL,
    /*
     * 054 
     */ NULL,
    /*
     * 055 
     */ NULL,
    /*
     * 056 
     */ NULL,
    /*
     * 057 
     */ NULL,
    /*
     * 058 
     */ NULL,
    /*
     * 059 
     */ NULL,
    /*
     * 060 
     */ NULL,
    /*
     * 061 
     */ NULL,
    /*
     * 062 
     */ NULL,
    /*
     * 063 
     */ NULL,
    /*
     * 064 
     */ NULL,
    /*
     * 065 
     */ NULL,
    /*
     * 066 
     */ NULL,
    /*
     * 067 
     */ NULL,
    /*
     * 068 
     */ NULL,
    /*
     * 069 
     */ NULL,
    /*
     * 070 
     */ NULL,
    /*
     * 071 
     */ NULL,
    /*
     * 072 
     */ NULL,
    /*
     * 073 
     */ NULL,
    /*
     * 074 
     */ NULL,
    /*
     * 075 
     */ NULL,
    /*
     * 076 
     */ NULL,
    /*
     * 077 
     */ NULL,
    /*
     * 078 
     */ NULL,
    /*
     * 079 
     */ NULL,
    /*
     * 080 
     */ NULL,
    /*
     * 081 
     */ NULL,
    /*
     * 082 
     */ NULL,
    /*
     * 083 
     */ NULL,
    /*
     * 084 
     */ NULL,
    /*
     * 085 
     */ NULL,
    /*
     * 086 
     */ NULL,
    /*
     * 087 
     */ NULL,
    /*
     * 088 
     */ NULL,
    /*
     * 089 
     */ NULL,
    /*
     * 090 
     */ NULL,
    /*
     * 091 
     */ NULL,
    /*
     * 092 
     */ NULL,
    /*
     * 093 
     */ NULL,
    /*
     * 094 
     */ NULL,
    /*
     * 095 
     */ NULL,
    /*
     * 096 
     */ NULL,
    /*
     * 097 
     */ NULL,
    /*
     * 098 
     */ NULL,
    /*
     * 099 
     */ NULL,
    /*
     * 100 
     */ NULL,
    /*
     * 101 
     */ NULL,
    /*
     * 102 
     */ NULL,
    /*
     * 103 
     */ NULL,
    /*
     * 104 
     */ NULL,
    /*
     * 105 
     */ NULL,
    /*
     * 106 
     */ NULL,
    /*
     * 107 
     */ NULL,
    /*
     * 108 
     */ NULL,
    /*
     * 109 
     */ NULL,
    /*
     * 110 
     */ NULL,
    /*
     * 111 
     */ NULL,
    /*
     * 112 
     */ NULL,
    /*
     * 113 
     */ NULL,
    /*
     * 114 
     */ NULL,
    /*
     * 115 
     */ NULL,
    /*
     * 116 
     */ NULL,
    /*
     * 117 
     */ NULL,
    /*
     * 118 
     */ NULL,
    /*
     * 119 
     */ NULL,
    /*
     * 120 
     */ NULL,
    /*
     * 121 
     */ NULL,
    /*
     * 122 
     */ NULL,
    /*
     * 123 
     */ NULL,
    /*
     * 124 
     */ NULL,
    /*
     * 125 
     */ NULL,
    /*
     * 126 
     */ NULL,
    /*
     * 127 
     */ NULL,
    /*
     * 128 
     */ NULL,
    /*
     * 129 
     */ NULL,
    /*
     * 130 
     */ NULL,
    /*
     * 131 
     */ NULL,
    /*
     * 132 
     */ NULL,
    /*
     * 133 
     */ NULL,
    /*
     * 134 
     */ NULL,
    /*
     * 135 
     */ NULL,
    /*
     * 136 
     */ NULL,
    /*
     * 137 
     */ NULL,
    /*
     * 138 
     */ NULL,
    /*
     * 139 
     */ NULL,
    /*
     * 140 
     */ NULL,
    /*
     * 141 
     */ NULL,
    /*
     * 142 
     */ NULL,
    /*
     * 143 
     */ NULL,
    /*
     * 144 
     */ NULL,
    /*
     * 145 
     */ NULL,
    /*
     * 146 
     */ NULL,
    /*
     * 147 
     */ NULL,
    /*
     * 148 
     */ NULL,
    /*
     * 149 
     */ NULL,
    /*
     * 150 
     */ NULL,
    /*
     * 151 
     */ NULL,
    /*
     * 152 
     */ NULL,
    /*
     * 153 
     */ NULL,
    /*
     * 154 
     */ NULL,
    /*
     * 155 
     */ NULL,
    /*
     * 156 
     */ NULL,
    /*
     * 157 
     */ NULL,
    /*
     * 158 
     */ NULL,
    /*
     * 159 
     */ NULL,
    /*
     * 160 
     */ NULL,
    /*
     * 161 
     */ NULL,
    /*
     * 162 
     */ NULL,
    /*
     * 163 
     */ NULL,
    /*
     * 164 
     */ NULL,
    /*
     * 165 
     */ NULL,
    /*
     * 166 
     */ NULL,
    /*
     * 167 
     */ NULL,
    /*
     * 168 
     */ NULL,
    /*
     * 169 
     */ NULL,
    /*
     * 170 
     */ NULL,
    /*
     * 171 
     */ NULL,
    /*
     * 172 
     */ NULL,
    /*
     * 173 
     */ NULL,
    /*
     * 174 
     */ NULL,
    /*
     * 175 
     */ NULL,
    /*
     * 176 
     */ NULL,
    /*
     * 177 
     */ NULL,
    /*
     * 178 
     */ NULL,
    /*
     * 179 
     */ NULL,
    /*
     * 180 
     */ NULL,
    /*
     * 181 
     */ NULL,
    /*
     * 182 
     */ NULL,
    /*
     * 183 
     */ NULL,
    /*
     * 184 
     */ NULL,
    /*
     * 185 
     */ NULL,
    /*
     * 186 
     */ NULL,
    /*
     * 187 
     */ NULL,
    /*
     * 188 
     */ NULL,
    /*
     * 189 
     */ NULL,
    /*
     * 190 
     */ NULL,
    /*
     * 191 
     */ NULL,
    /*
     * 192 
     */ NULL,
    /*
     * 193 
     */ NULL,
    /*
     * 194 
     */ NULL,
    /*
     * 195 
     */ NULL,
    /*
     * 196 
     */ NULL,
    /*
     * 197 
     */ NULL,
    /*
     * 198 
     */ NULL,
    /*
     * 199 
     */ NULL,
    /*
     * 200 RPL_TRACELINK 
     */ ":%C 200 %s ���������� ������ %s%s %s %s",
    /*
     * 201 RPL_TRACECONNECTING 
     */ ":%C 201 %s ���������� %d %s",
    /*
     * 202 RPL_TRACEHANDSHAKE 
     */ ":%C 202 %s �������� %d %s",
    /*
     * 203 RPL_TRACEUNKNOWN 
     */ ":%C 203 %s �������� %d %s %d",
    /*
     * 204 RPL_TRACEOPERATOR 
     */ ":%C 204 %s IRC ������������ %d %s %ld",
    /*
     * 205 RPL_TRACEUSER 
     */ ":%C 205 %s ������� %d %s %ld",
    /*
     * 206 RPL_TRACESERVER 
     */ ":%C 206 %s ����������� %d %dS %dC %s %s %ld",
    /*
     * 207 RPL_TRACESERVICE 
     */ ":%C 207 %s �������� %d %s %d %ld :%s",
    /*
     * 208 RPL_TRACENEWTYPE 
     */ ":%C 208 %s <����������� �����> 0 %s",
    /*
     * 209 RPL_TRACECLASS 
     */ ":%C 209 %s ���� %d %d",
    /*
     * 210 RPL_MAP
     */ ":%C 210 %s :%s",
    /*
     * 211 RPL_MAPEND
     */ ":%C 211 %s :����� ��� /MAP",
    /*
     * 212 RPL_STATSCOMMANDS 
     */ NULL,
    /*
     * 213 RPL_STATSCLINE 
     */ NULL,
    /*
     * 214 RPL_STATSNLINE 
     */ ":%C 214 %s %c %s * %s %d %d",
    /*
     * 215 RPL_STATSILINE 
     */ ":%C 215 %s %c %s * %s %d %d",
    /*
     * 216 RPL_STATSKLINE 
     */ ":%C 216 %s %c %s * %s %d :%s",
    /*
     * 217 RPL_STATSQLINE 
     */ ":%C 217 %s %c %s * %s %d %d",
    /*
     * 218 RPL_STATSYLINE 
     */ ":%C 218 %s %c %d %d %d %d %ld",
    /*
     * 219 RPL_ENDOFSTATS 
     */ ":%C 219 %s %c :����� ��� �������� /STATS .",
    /*
     * 220 RPK_STATSPLINE
     */ NULL,
    /*
     * 221 RPL_UMODEIS 
     */ ":%C 221 %s %s",
    /*
     * 222 
     */ ":%C 222 %s %c %s * %s %d %d",
    /*
     * 223 
     */ ":%C 223 %s %c %s * %s %d %d",
    /*
     * 224 
     */ ":%C 224 %s %c %s * %s %d %d",
    /*
     * 225 RPL_STATSZLINE 
     */ ":%C 225 %s %c %s %s",
    /*
     * 226 RPL_STATSCOUNT 
     */ ":%C 226 %s %s %l",
    /*
     * 227 RPL_STATSGLINE 
     */ ":%C 227 %s %c %s * %s %d %d",
    /*
     * 228 RPL_STATSISLINE 
     */ ":%C 228 %s %c %s * %s %d %d",
    /*
     * 229 RPL_STATSJLINE 
     */ ":%C 229 %s %c %s * %s %d %d",
    /*
     * 230 RPL_MODLIST
     */ ":%C 230 %s %s 0x%x %s",
    /*
     * 231 RPL_SERVICEINFO 
     */ ":%C 231 %s :����������� ��� ��� ���������...",
    /*
     * In use by Undernet 
     */
    /*
     * 232 RPL_ENDOFSERVICES 
     */ ":%C 232 %s :����� ��� ��������� ",
    /*
     * In use by Undernet 
     */
    /*
     * 233 RPL_SERVICE 
     */ ":%C 233 %s :� �������� ��������",
    /*
     * 234 RPL_SERVLIST 
     */ ":%C 234 %s �������� %s (%s@%s) ��������������� %s %d :%s",
    /*
     * 235 RPL_SERVLISTEND 
     */ ":%C 235 %s :����� ��� ������ ���������",
    /*
     * 236 
     */ NULL,
    /*
     * 237 
     */ NULL,
    /*
     * 238 
     */ NULL,
    /*
     * 239 
     */ NULL,
    /*
     * 240 RPL_STATSWLINE 
     */ ":%C 240 %s %c %s * %s %d %d",
    /*
     * 241 RPL_STATSLLINE 
     */ ":%C 241 %s %c %s * %s %d %d",
    /*
     * 242 RPL_STATSUPTIME 
     */ ":%C 242 %s :� ����������� ����� ���������� ��� ��� %d �����, %d:%02d:%02d",
    /*
     * 243 RPL_STATSOLINE 
     */ ":%C 243 %s %c %s * %s %lu %d",
    /*
     * 244 RPL_STATSHLINE 
     */ ":%C 244 %s %c %s * %s %d %d",
    /*
     * 245 RPL_STATSSLINE 
     */ ":%C 245 %s %c %s * %s %d %d",
    /*
     * 246 RPL_STATSXLINE 
     */ ":%C 246 %s %c %s * %s %d %d",
    /*
     * 247 
     */ NULL,
    /*
     * Undernet's STATSGLINE 
     */
    /*
     * 248 
     */ NULL,
    /*
     * Undernet's STATSULINE 
     */
    /*
     * 249 RPL_STATSDEBUG 
     */ ":%C 249 %s :%s",
    /*
     * 250 
     */ NULL,
    /*
     * 251 RPL_LUSERCLIENT 
     */ ":%C 251 %s :�������� %d ������� ��� ���� ������� �� %d ����� ������� ���� ��  %d �����������",
    /*
     * 252 RPL_LUSEROP 
     */ ":%C 252 %s %d :IRC ������������ ����� online",
    /*
     * 253 RPL_LUSERUNKNOWN 
     */ ":%C 253 %s %d :�������� ���������",
    /*
     * 254 RPL_LUSERCHANNELS 
     */ ":%C 254 %s %d :������� ����� ������������",
    /*
     * 255 RPL_LUSERME 
     */ ":%C 255 %s :�������� %d clients ��� %d �����������",
    /*
     * 256 RPL_ADMINME 
     */ ":%C 256 %s :����������� ���  %s",
    /*
     * 257 RPL_ADMINLOC1 
     */ ":%C 257 %s :%s",
    /*
     * 258 RPL_ADMINLOC2 
     */ ":%C 258 %s :%s",
    /*
     * 259 RPL_ADMINEMAIL 
     */ ":%C 259 %s :%s",
    /*
     * 260 
     */ NULL,
    /*
     * 261 RPL_TRACELOG 
     */ ":%C 261 %s ������ %s %d",
    /*
     * 262 
     */ ":%C 262 %s %s :����� ��� TRACE",
    /*
     * 263 
     */ ":%C 263 %s :� ����������� ����� ���� ���������� ���� �� ������. �������� ��������� ��������.",
    /*
     * 264 
     */ NULL,
    /*
     * 265 RPL_LOCALUSERS 
     */ ":%C 265 %s :������� ������� ���� �� ������: %d  ������������ ������� ��� ���� ����: %d",
    /*
     * 266 RPL_GLOBALUSERS  
     */ ":%C 266 %s :������� ��� ������� ���� �� ������: %d ������������ ������� ��� ���� �� ������ ����: %d",
    /*
     * 267 
     */ NULL,
    /*
     * 268 
     */ NULL,
    /*
     * 269 
     */ NULL,
    /*
     * 270 
     */ NULL,
    /*
     * 271 RPL_SILELIST 
     */ ":%C 271 %s %s %s",
    /*
     * 272 RPL_ENDOFSILELIST
     */ ":%C 272 %s :����� ��� ������ ��� �� /SILENCE.",
    /*
     * 273 
     */ NULL,
    /*
     * 274 
     */ NULL,
    /*
     * 275 
     */ NULL,
    /*
     * In use by Undernet 
     */
    /*
     * 276 
     */ NULL,
    /*
     * 277 
     */ NULL,
    /*
     * 278 
     */ NULL,
    /*
     * 279 
     */ NULL,
    /*
     * 280 
     */ NULL,
    /*
     * In use by Undernet 
     */
    /*
     * 281 
     */ NULL,
    /*
     * In use by Undernet 
     */
    /*
     * 282 
     */ NULL,
    /*
     * 283 
     */ NULL,
    /*
     * 284 
     */ NULL,
    /*
     * 285 
     */ NULL,
    /*
     * 286 
     */ NULL,
    /*
     * 287 
     */ NULL,
    /*
     * 288 
     */ NULL,
    /*
     * 289 
     */ NULL,
    /*
     * 290 
     */ NULL,
    /*
     * 291 
     */ NULL,
    /*
     * 292 
     */ NULL,
    /*
     * 293 
     */ NULL,
    /*
     * 294 
     */ NULL,
    /*
     * 295 
     */ NULL,
    /*
     * 296 
     */ NULL,
    /*
     * 297 
     */ NULL,
    /*
     * 298 
     */ NULL,
    /*
     * 299 
     */ NULL,
    /*
     * 300 
     */ NULL,
    /*
     * 301 RPL_AWAY 
     */ ":%C 301 %s %s :%s",
    /*
     * 302 RPL_USERHOST 
     */ ":%C 302 %s :",
    /*
     * 303 RPL_ISON 
     */ ":%C 303 %s :",
    /*
     * 304 RPL_USERIP
     */ ":%C 304 %s :",
    /*
     * 305 RPL_UNAWAY 
     */ ":%C 305 %s :� ��������� ��� �������������",
    /*
     * 306 RPL_NOWAWAY 
     */ ":%C 306 %s :��������� �� ����������� �������",
    /*
     * 307 RPL_WHOISREGNICK 
     */ ":%C 307 %s %s :���� ������������ � ��������� ���",
    /*
     * 308 RPL_WHOISADMIN 
     */ ":%C 308 %s %s :����� IRCTR Server Administrator",
    /*
     * 309 RPL_WHOISSADMIN 
     */ ":%C 309 %s %s :����� IRCTR Services Administrator",
    /*
     * 310 RPL_WHOISSVCMSG 
     */ ":%C 310 %s %s",
    /*
     * 311 RPL_WHOISUSER 
     */ ":%C 311 %s %s %s %s * :%s",
    /*
     * 312 RPL_WHOISSERVER 
     */ ":%C 312 %s %s %s :%s",
    /*
     * 313 RPL_WHOISOPERATOR 
     */ ":%C 313 %s %s :is %s",
    /*
     * 314 RPL_WHOWASUSER 
     */ ":%C 314 %s %s %s %s * :%s",
    /*
     * 315 RPL_ENDOFWHO 
     */ ":%C 315 %s %s :����� ��� ������ /WHO.",
    /*
     * 316 RPL_WHOISCHANOP 
     */ NULL,
    /*
     * 317 RPL_WHOISIDLE 
     */ ":%C 317 %s %s %ld %ld :������������ ���������, ��� ��������",
    /*
     * 318 RPL_ENDOFWHOIS 
     */ ":%C 318 %s %s :����� ��� ������ /WHOIS.",
    /*
     * 319 RPL_WHOISCHANNELS 
     */ ":%C 319 %s %s :%s",
    /*
     * 320 RPL_WHOWASREAL 
     */ ":%C 320 %s %s ���������� ���  %s",
    /*
     * 321 RPL_LISTSTART 
     */ ":%C 321 %s ������ :����� ������",
    /*
     * 322 RPL_LIST 
     */ ":%C 322 %s %s %d :%s",
    /*
     * 323 RPL_LISTEND 
     */ ":%C 323 %s :����� ��� /LIST",
    /*
     * 324 RPL_CHANNELMODEIS 
     */ ":%C 324 %s %H %s%s",
    /*
     * 325 
     */ NULL,
    /*
     * 326 
     */ NULL,
    /*
     * 327 
     */ NULL,
    /*
     * 328 
     */ NULL,
    /*
     * 329 RPL_CREATIONTIME 
     */ ":%C 329 %s %H %lu",
    /*
     * 330 
     */ NULL,
    /*
     * 331 RPL_NOTOPIC 
     */ ":%C 331 %s %s :��� ������� topic.",
    /*
     * 332 RPL_TOPIC 
     */ ":%C 332 %s %s :%s",
    /*
     * 333 RPL_TOPICWHOTIME 
     */ ":%C 333 %s %s %s %lu",
    /*
     * 334 RPL_COMMANDSYNTAX 
     */ ":%C 334 %s :%s",
    /*
     * 335 
     */ NULL,
    /*
     * 336 
     */ NULL,
    /*
     * 337 
     */ NULL,
    /*
     * 338 RPL_WHOISACTUALLY 
     */ ":%C 338 %s :%s ����� ��� �������������� %s@%s [%s]",
    /*
     * 339 
     */ NULL,
    /*
     * 340 
     */ NULL,
    /*
     * 341 RPL_INVITING 
     */ ":%C 341 %s %s %s",
    /*
     * 342 RPL_SUMMONING 
     */ ":%C 342 %s %s :User summoned to irc",
    /*
     * 343 RPL_CBANLIST 
     */ ":%C 343 %s %H %s %s %lu",
    /*
     * 344 RPL_STOPLIST
     */ ":%C 344 %s %H %s %s %lu",
    /*
     * 345 RPL_ENDOFSTOPLIST
     */ ":%C 345 %s %H :����� ��� Moderated Hosts List ��� ��������",
    /*
     * 346 RPL_INVITELIST 
     */ ":%C 346 %s %H %s %s %lu",
    /*
     * 347 RPL_ENDOFINVITELIST 
     */ ":%C 347 %s %H :����� ��� Invite List ��� ��������",
    /*
     * 348 RPL_EXCEPTIONLIST 
     */ ":%C 348 %s %H %s %s %lu",
    /*
     * 349 RPL_ENDOFEXCEPLIST 
     */ ":%C 349 %s %H :����� ��� Exception List ��� ��������",
    /*
     * 350 RPL_ENDOFCBANLIST
     */ ":%C 350 %s %H :����� ��� Banned Channels List ��� ��������",
    /*
     * 351 RPL_VERSION 
     */ ":%C 351 %s %s.%s %s :%s",
    /*
     * 352 RPL_WHOREPLY 
     */ ":%C 352 %s %s %s %s %s %s %s :%d %s",
    /*
     * 353 RPL_NAMREPLY 
     */ ":%C 353 %s %s",
    /*
     * 354 
     */ NULL,
    /*
     * In use by Undernet 
     */
    /*
     * 355 
     */ NULL,
    /*
     * 356 
     */ NULL,
    /*
     * 357 
     */ NULL,
    /*
     * 358 
     */ NULL,
    /*
     * 359 
     */ NULL,
    /*
     * 360 
     */ NULL,
    /*
     * 361 
     */ NULL,
    /*
     * 362 RPL_CLOSING 
     */ ":%C 362 %s %s :�������. ������ status = %d",
    /*
     * 363 RPL_CLOSEEND 
     */ ":%C 363 %s %d: �������� ���������",
    /*
     * 364 RPL_LINKS 
     */ ":%C 364 %s %s %s :%d %s",
    /*
     * 365 RPL_ENDOFLINKS 
     */ ":%C 365 %s %s :����� ��� ������ /LINKS .",
    /*
     * 366 RPL_ENDOFNAMES 
     */ ":%C 366 %s %s :����� ��� ������ /NAMES.",
    /*
     * 367 RPL_BANLIST 
     */ ":%C 367 %s %H %s %s %lu",
    /*
     * 368 RPL_ENDOFBANLIST 
     */ ":%C 368 %s %H :����� ��� Ban List ��� ��������",
    /*
     * 369 RPL_ENDOFWHOWAS 
     */ ":%C 369 %s %s :����� ��� WHOWAS",
    /*
     * 370 RPL_DISPLAY
     */ ":%C 370 %s :%s",
    /*
     * 371 RPL_INFO 
     */ ":%C 371 %s :%s",
    /*
     * 372 RPL_MOTD 
     */ ":%C 372 %s :- %s",
    /*
     * 373 RPL_INFOSTART 
     */ ":%C 373 %s :Server INFO",
    /*
     * 374 RPL_ENDOFINFO 
     */ ":%C 374 %s :����� ��� ������ /INFO.",
    /*
     * 375 RPL_MOTDSTART 
     */ ":%C 375 %s :- %s ������ ��� ������ - ",
    /*
     * 376 RPL_ENDOFMOTD 
     */ ":%C 376 %s :����� ��� ������� /MOTD.",
    /*
     * 377 RPL_ENDOFDISPLAY 
     */ ":%C 377 %s :����� ��� ������� /DISPLAY.",
    /*
     * 378 RPL_REALHOST  
     */ ":%C 378 %s %s :���������� ��� %s",
    /*
     * 379 RPL_DISPLAYSTART
     */ ":%C 379 %s %s :���������� �������",
    /*
     * 380 RPL_RPONG
     */ ":%C 380 %s :Ping ��� %s ���� %s ����� %lu",
    /*
     * 381 RPL_YOUREOPER 
     */ ":%C 381 %s :����� ����� ���� ������������ ��� IRC - �������!",
    /*
     * 382 RPL_REHASHING 
     */ ":%C 382 %s %s :Rehashing",
    /*
     * 383 RPL_YOURESERVICE 
     */ ":%C 383 %s :����� ���� ����� ��� Services",
    /*
     * 384 RPL_MYPORTIS 
     */ ":%C 384 %s %d :� ����� ��� ��� ������ ���������� �����",
    /*
     * 385 
     */ NULL,
    /*
     * 386 RPL_IRCOPSSTART 
     */ ":%C 386 :������������ IRC ����� ���� ���� ��� ������",
    /*
     * 387 RPL_IRCOPS 
     */ ":%C 387 :%s",
    /*
     * 388 RPL_ENDOFIRCOPS 
     */ ":%C 388 :����� ��� /IRCOPS ������.",
    /*
     * 389 RPL_HASH
     */ ":%C 389 %s :%s",
    /*
     * 390 RPL_ENDOFHASH
     */ ":%C 390 %s :����� ��� /HASH ������.",
    /*
     * 391 RPL_TIME 
     */ ":%C 391 %s %s :%s",
    /*
     * 392 
     */ NULL,
    /*
     * 393 
     */ NULL,
    /*
     * 394 
     */ NULL,
    /*
     * 395 
     */ NULL,
    /*
     * 396 
     */ NULL,
    /*
     * 397 
     */ NULL,
    /*
     * 398 
     */ NULL,
    /*
     * 399 
     */ NULL,
    /*
     * 400 
     */ NULL,
    /*
     * 401 ERR_NOSUCHNICK 
     */ ":%C 401 %s %s :��� ������� ������ ���������/������",
    /*
     * 402 ERR_NOSUCHSERVER 
     */ ":%C 402 %s %s :��� ������� ������� �����������",
    /*
     * 403 ERR_NOSUCHCHANEL 
     */ ":%C 403 %s %s :��� ������� ������ ������",
    /*
     * 404 ERR_CANNOTSENDTOCHAN 
     */ ":%C 404 %s %s :��� ����� �� ������ ��� ������",
    /*
     * 405 ERR_TOOMANYCHANNELS 
     */ ":%C 405 %s %s :����� ��������� �� ���� ��� �������� ��� �������� �� ������",
    /*
     * 406 ERR_WASNOSUCHNICK 
     */ ":%C 406 %s %s :��� ������ ������ ���������",
    /*
     * 407 ERR_TOOMANYTARGETS 
     */ ":%C 407 %s %s :���� ��� ���� ������� ����������. �� ������ ��� �������",
    /*
     * 408 ERR_NOCOLORSONCHAN 
     */ ":%C 408 %s %s :��� ����������� � ����� �������� ��� ������. ��� �������: %s",
    /*
     * 409 ERR_NOORIGIN 
     */ ":%C 409 %s :��� �������������� � ����",
    /*
     * 410 ERR_YOUAREZOMBIE 
     */ ":%C 410 %s :��� ����� �� ������ ��� %s (Zombie mode)",
    /*
     * 411 ERR_NORECIPIENT 
     */ ":%C 411 %s :��� ���� ����� ���������� (%s)",
    /*
     * 412 ERR_NOTEXTOSEND 
     */ ":%C 412 %s :��� ���� ����� ������",
    /*
     * 413 ERR_NOTOPLEVEL 
     */ ":%C 413 %s %s :��� ���� ����� toplevel �������",
    /*
     * 414 ERR_WILDTOPLEVEL 
     */ ":%C 414 %s %s :Wildcard ��� toplevel �������",
    /*
     * 415 ERR_NOCHANTOWATCH 
     */ ":%C 415 %s %s :��� ����� �� ��� ��� ������. ��� ������� ������������ ��� ������.",
    /*
     * 416 
     */ NULL,
    /*
     * 417 
     */ NULL,
    /*
     * 418 
     */ NULL,
    /*
     * 419 
     */ NULL,
    /*
     * 420 
     */ NULL,
    /*
     * 421 ERR_UNKNOWNCOMMAND 
     */ ":%C 421 %s %s :������� ������",
    /*
     * 422 ERR_NOMOTD 
     */ ":%C 422 %s :������ �� MOTD ������",
    /*
     * 423 ERR_NOADMININFO 
     */ ":%C 423 %s %s :��� ������� administrative info",
    /*
     * 424 ERR_FILEERROR 
     */ ":%C 424 %s :����� ������� �������� %s ��� %s",
    /*
     * 425 
     */ NULL,
    /*
     * 426 
     */ NULL,
    /*
     * 427 
     */ NULL,
    /*
     * 428 
     */ NULL,
    /*
     * 429 ERR_TOOMANYAWAY 
     */ ":%C 429 %s :���� ����� aways - �������������� � ��������� ��� flood",
    /*
     * 430 ERR_OLDSERVER
     */ ":%C 430 %s %s :O client ������������ ����� ������ ��� server. � NICKIP �������������� �������",
    /*
     * 431 ERR_NONICKNAMEGIVEN 
     */ ":%C 431 %s :��� ���� ����� ���������.",
    /*
     * 432 ERR_ERRONEOUSNICKNAME 
     */ ":%C 432 %s %s :���� �� ��������� ��� �����������, ��������� ���� ����.",
    /*
     * 433 ERR_NICKNAMEINUSE 
     */ ":%C 433 %s %s :�� ������������ ��������� ��������������� ���. ��������� ���� ����.",
    /*
     * 434 ERR_EVALUATINGQLINE 
     */ ":%C 434 %s :Qline %s ������������ ���� �� ������� ��� (%s)",
    /*
     * 435 ERR_BANONCHAN 
     */ ":%C 435 %s %s %s :��� �������� �� �������� �� ��������� ��� ���� ����� ban",
    /*
     * 436 ERR_NICKCOLLISION 
     */ ":%C 436 %s %s :KILL ���� ����������� ����� ����������",
    /*
     * 437 ERR_BANNICKCHANGE 
     */ ":%C 437 %s %s :��� ������ �� ����� ������ ���������� ���� ������� �� banned � moderated ������",
    /*
     * 438 
     */ NULL,
    /*
     * In use by Undernet - ERR_NICKTOOFAST 
     */
    /*
     * 439 
     */ NULL,
    /*
     * In use by Undernet - ERR_TARGETTOOFAST 
     */
    /*
     * 440 ERR_SERVICESDOWN 
     */ ":%C 440 %s %s :�� Services ���� �� ������ ��� ����� ���������. �������� ��������� ��������.",
    /*
     * 441 ERR_USERNOTINCHANNEL 
     */ ":%C 441 %s %s %H :��� ���������� �� ���� �� ������",
    /*
     * 442 ERR_NOTONCHANNEL 
     */ ":%C 442 %s %s :��� ��������� �� ���� �� ������",
    /*
     * 443 ERR_USERONCHANNEL 
     */ ":%C 443 %s %s %s :��������� ��� ��� ������",
    /*
     * 444 ERR_NOLOGIN 
     */ ":%C 444 %s %s :� ������� ��� ����� ��� ������",
    /*
     * 445 ERR_SUMMONDISABLED 
     */ ":%C 445 %s :�� SUMMON ����������",
    /*
     * 446 ERR_USERSDISABLED 
     */ ":%C 446 %s :�� USERS ����������",
    /*
     * 447 
     */ NULL,
    /*
     * 448 
     */ NULL,
    /*
     * 449 
     */ NULL,
    /*
     * 450 
     */ NULL,
    /*
     * 451 ERR_NOTREGISTERED 
     */ ":%C 451 %s :��� ����� ����� register",
    /*
     * 452 
     */ NULL,
    /*
     * 453 
     */ NULL,
    /*
     * 454 
     */ NULL,
    /*
     * 455 
     */ NULL,
    /*
     * 456 
     */ NULL,
    /*
     * 457 
     */ NULL,
    /*
     * 458 
     */ NULL,
    /*
     * 459 
     */ NULL,
    /*
     * 460 
     */ NULL,
    /*
     * 461 ERR_NEEDMOREPARAMS 
     */ ":%C 461 %s %s :�� ���������� ��� ����� �������",
    /*
     * 462 ERR_ALREADYREGISTERED 
     */ ":%C 462 %s :��� �������� �� ���������� register",
    /*
     * 463 ERR_NOPERMFORHOST 
     */ ":%C 463 %s :�� host ��� ��� ���� �� ���������� ��������",
    /*
     * 464 ERR_PASSWDMISMATCH 
     */ ":%C 464 %s :����� �������",
    /*
     * 465 ERR_YOUREBANNEDCREEP 
     */ ":%C 465 %s :������ %s.",
    /*
     * 466 
     */ NULL,
    /*
     * 467 ERR_KEYSET 
     */ ":%C 467 %s %s :������� ��� ������ ��� �� ������������ ������.",
    /*
     * 468 ERR_ONLYSERVERSCANCHANGE 
     */ ":%C 468 %s %H :���� �� ����������� ������� �� �������� ���� �� mode",
    /*
     * 469 
     */ NULL,
    /*
     * 470 
     */ NULL,
    /*
     * 471 ERR_CHANNELISFULL 
     */ ":%C 471 %s %s :��� �������� �� ������ ��� ������ (+l)",
    /*
     * 472 ERR_UNKNOWNMODE 
     */ ":%C 472 %s %c :��� ����� �� �� ����������",
    /*
     * 473 ERR_INVITEONLYCHAN 
     */ ":%C 473 %s %s :��� �������� �� ������ ��� ������ (+i)",
    /*
     * 474 ERR_BANNEDFROMCHAN 
     */ ":%C 474 %s %s :��� �������� �� ������ ��� ������ (+b)",
    /*
     * 475 ERR_BADCHANNELKEY 
     */ ":%C 475 %s %s :��� �������� �� ������ ��� ������ (+k)",
    /*
     * 476 ERR_BADCHANMASK 
     */ ":%C 476 %s %s :����� maska ��� �� ������",
    /*
     * 477 ERR_NEEDREGGEDNICK 
     */ ":%C 477 %s %s :�� ��������� ��� ������ �� ����� registered ��� �� ������ ��� ������.",
    /*
     * 478 ERR_BANLISTFULL 
     */ ":%C 478 %s %H %s :� ban ����� ��� �������� ����� ������",
    /*
     * 479 ERR_BADCHANNAME 
     */ ":%C 479 %s %s :�� ����� ��� �������� �������� �� ���������� ����������.",
    /*
     * 480 ERR_CHANISJUPED 
     */ ":%C 480 %s :����������� ����� �������� %s. � ����� ��� �������� %s ��� ������ �� �������������� (%s)",
    /*
     * 481 ERR_NOPRIVILEGES 
     */ ":%C 481 %s :��� �������� �� �� ������ ����, ��� ����� �� �������� ��� ����������� IRC",
    /*
     * 482 ERR_CHANOPRIVSNEEDED 
     */ ":%C 482 %s %H :��� ������� ������������ ��� ��������",
    /*
     * 483 ERR_CANTKILLSERVER 
     */ ":%C 483 %s :��� �������� �� ������ kill ��� ����������!",
    /*
     * 484 - In Use by Undernet
     */ NULL,
    /*
     * 485 
     */ NULL,
    /*
     * 486 ERR_NONONREG 
     */ ":%C 486 %s :������ �� ����� registered ��������� ��� �� �������� �� �������� prive ������ ����/���� %s",
    /*
     * 487 
     */ NULL,
    /*
     * 488 
     */ NULL,
    /*
     * 489 - In use by Undernet
     */ NULL,
    /*
     * 490 
     */ NULL,
    /*
     * 491 ERR_NOOPERHOST 
     */ ":%C 491 %s :��� ����� �� ��� O-lines ��� ��� host ���.",
    /*
     * 492 
     */ NULL,
    /*
     * 493 
     */ NULL,
    /*
     * 494 
     */ NULL,
    /*
     * 495 
     */ NULL,
    /*
     * 496 
     */ NULL,
    /*
     * 497 
     */ NULL,
    /*
     * 498 
     */ NULL,
    /*
     * 499 
     */ NULL,
    /*
     * 500 
     */ NULL,
    /*
     * 501 ERR_UMODEUNKNOWNFLAG 
     */ ":%C 501 %s :�� ������������ MODE",
    /*
     * 502 ERR_USERSDONTMATCH 
     */ ":%C 502 %s :��� ������ �� ����� ������ ��� mode ��� ������ �������.",
    /*
     * 503 
     */ ":%C 503 %s :�� ������ *���* �������� ����/���� %s",
    /*
     * 504 
     */ NULL,
    /*
     * 505 
     */ NULL,
    /*
     * 506 
     */ NULL,
    /*
     * 507 
     */ NULL,
    /*
     * 508 
     */ NULL,
    /*
     * 509 
     */ NULL,
    /*
     * 510 
     */ NULL,
    /*
     * 511 ERR_SILELISTFULL 
     */ ":%C 511 %s %s :� silence ����� ��� ����� ������",
    /*
     * 512 ERR_TOOMANYWATCH 
     */ ":%C 512 %s %s :�� ������� ��������� ������� ��� ��� WATCH ����� ����� 128 ��������",
    /*
     * 513 - In use by Undernet
     */ NULL,
    /*
     * 514 ERR_TOOMANYDCC 
     */ ":%C 514 %s %s :� dcc allow ����� ��� ����� ������. �� ������� ��������� ������� ����� %d ��������",
    /*
     * 515 
     */ NULL,
    /*
     * 516 
     */ NULL,
    /*
     * 517 
     */ NULL,
    /*
     * 518 
     */ NULL,
    /*
     * 519 
     */ NULL,
    /*
     * 520 
     */ NULL,
    /*
     * 521 ERR_LISTSYNTAX 
     */ ":%C 521 %s :����� ������� ��� ������, ������ /quote list ? � /raw list ?",
    /*
     * 522 ERR_WHOSYNTAX 
     */ ":%C 522 %s :/WHO ����� �������, ������ /who ? ��� ������������ �����������",
    /*
     * 523 ERR_WHOLIMEXCEED 
     */ ":%C 523 %s :�����, �� ���� %d ��� /WHO �����������."
	"  �������� ������ ��� ������������� ���� ��������� ���",
    /*
     * 524 ERR_HELPNOTFOUND 
     */ ":%C 524 %s %s :��� �������� �����������",
    /*
     * 525 
     */ NULL,
    /*
     * 526 
     */ NULL,
    /*
     * 527 
     */ NULL,
    /*
     * 528 
     */ NULL,
    /*
     * 529 
     */ NULL,
    /*
     * 530 
     */ NULL,
    /*
     * 531 
     */ NULL,
    /*
     * 532 
     */ NULL,
    /*
     * 533 
     */ NULL,
    /*
     * 534 
     */ NULL,
    /*
     * 535 
     */ NULL,
    /*
     * 536 
     */ NULL,
    /*
     * 537 
     */ NULL,
    /*
     * 538 
     */ NULL,
    /*
     * 539 
     */ NULL,
    /*
     * 540 
     */ NULL,
    /*
     * 541 
     */ NULL,
    /*
     * 542 
     */ NULL,
    /*
     * 543 
     */ NULL,
    /*
     * 544 
     */ NULL,
    /*
     * 545 
     */ NULL,
    /*
     * 546 
     */ NULL,
    /*
     * 547 
     */ NULL,
    /*
     * 548 
     */ NULL,
    /*
     * 549 
     */ NULL,
    /*
     * 550 
     */ NULL,
    /*
     * 551 
     */ NULL,
    /*
     * 552 
     */ NULL,
    /*
     * 553 
     */ NULL,
    /*
     * 554 
     */ NULL,
    /*
     * 555 
     */ NULL,
    /*
     * 556 
     */ NULL,
    /*
     * 557 
     */ NULL,
    /*
     * 558 
     */ NULL,
    /*
     * 559 
     */ NULL,
    /*
     * 560 
     */ NULL,
    /*
     * 561 
     */ NULL,
    /*
     * 562 
     */ NULL,
    /*
     * 563 
     */ NULL,
    /*
     * 564 
     */ NULL,
    /*
     * 565 
     */ NULL,
    /*
     * 566 
     */ NULL,
    /*
     * 567 
     */ NULL,
    /*
     * 568 
     */ NULL,
    /*
     * 569 
     */ NULL,
    /*
     * 570 
     */ NULL,
    /*
     * 571 
     */ NULL,
    /*
     * 572 
     */ NULL,
    /*
     * 573 
     */ NULL,
    /*
     * 574 
     */ NULL,
    /*
     * 575 
     */ NULL,
    /*
     * 576 
     */ NULL,
    /*
     * 577 
     */ NULL,
    /*
     * 578 
     */ NULL,
    /*
     * 579 
     */ NULL,
    /*
     * 580 
     */ NULL,
    /*
     * 581 
     */ NULL,
    /*
     * 582 
     */ NULL,
    /*
     * 583 
     */ NULL,
    /*
     * 584 
     */ NULL,
    /*
     * 585 
     */ NULL,
    /*
     * 586 
     */ NULL,
    /*
     * 587 
     */ NULL,
    /*
     * 588 
     */ NULL,
    /*
     * 589 
     */ NULL,
    /*
     * 590 
     */ NULL,
    /*
     * 591 
     */ NULL,
    /*
     * 592 
     */ NULL,
    /*
     * 593 
     */ NULL,
    /*
     * 594 
     */ NULL,
    /*
     * 595 
     */ NULL,
    /*
     * 596 
     */ NULL,
    /*
     * 597 
     */ NULL,
    /*
     * 598 
     */ NULL,
    /*
     * 599 
     */ NULL,
    /*
     * 600 RPL_LOGON 
     */ ":%C 600 %s %s %s %s %d :����������� online",
    /*
     * 601 RPL_LOGOFF 
     */ ":%C 601 %s %s %s %s %d :����������� offline",
    /*
     * 602 RPL_WATCHOFF 
     */ ":%C 602 %s %s %s %s %d :��������� �� ������������",
    /*
     * 603 RPL_WATCHSTAT 
     */ ":%C 603 %s :����� %d ��� ������� ��� %d WATCH ��������",
    /*
     * 604 RPL_NOWON 
     */ ":%C 604 %s %s %s %s %d :����� online",
    /*
     * 605 RPL_NOWOFF 
     */ ":%C 605 %s %s %s %s %d :����� offline",
    /*
     * 606 RPL_WATCHLIST 
     */ ":%C 606 %s :%s",
    /*
     * 607 RPL_ENDOFWATCHLIST 
     */ ":%C 607 %s :����� ��� WATCH %c",
    /*
     * 608 - Do not use -- Reserved for WATCH -Rak
     */ NULL,
    /*
     * 609 
     */ NULL,
    /*
     * 610 
     */ NULL,
    /*
     * 611 
     */ NULL,
    /*
     * 612 
     */ NULL,
    /*
     * 613 
     */ NULL,
    /*
     * 614 
     */ NULL,
    /*
     * 615 
     */ NULL,
    /*
     * 616 
     */ NULL,
    /*
     * 617 RPL_DCCSTATUS 
     */ ":%C 617 %s :%s ����� %s ���� DCC allow �����",
    /*
     * 618 RPL_DCCLIST 
     */ ":%C 618 %s :%s",
    /*
     * 619 RPL_ENDOFDCCLIST 
     */ ":%C 619 %s :����� ��� DCCALLOW %s",
    /*
     * 620 RPL_DCCINFO 
     */ ":%C 620 %s :%s",
    /*
     * 621 
     */ NULL,
    /*
     * 622 
     */ NULL,
    /*
     * 623 
     */ NULL,
    /*
     * 624 
     */ NULL,
    /*
     * 625 
     */ NULL,
    /*
     * 626 
     */ NULL,
    /*
     * 627 
     */ NULL,
    /*
     * 628 
     */ NULL,
    /*
     * 629 
     */ NULL,
    /*
     * 630 
     */ NULL,
    /*
     * 631 
     */ NULL,
    /*
     * 632 
     */ NULL,
    /*
     * 633 
     */ NULL,
    /*
     * 634 
     */ NULL,
    /*
     * 635 
     */ NULL,
    /*
     * 636 
     */ NULL,
    /*
     * 637 
     */ NULL,
    /*
     * 638 
     */ NULL,
    /*
     * 639 
     */ NULL,
    /*
     * 640 
     */ NULL,
    /*
     * 641 
     */ NULL,
    /*
     * 642 
     */ NULL,
    /*
     * 643 
     */ NULL,
    /*
     * 644 
     */ NULL,
    /*
     * 645 
     */ NULL,
    /*
     * 646 
     */ NULL,
    /*
     * 647 
     */ NULL,
    /*
     * 648 
     */ NULL,
    /*
     * 649 
     */ NULL,
    /*
     * 650 
     */ NULL,
    /*
     * 651 
     */ NULL,
    /*
     * 652 
     */ NULL,
    /*
     * 653 
     */ NULL,
    /*
     * 654 
     */ NULL,
    /*
     * 655 
     */ NULL,
    /*
     * 656 
     */ NULL,
    /*
     * 657 
     */ NULL,
    /*
     * 658 
     */ NULL,
    /*
     * 659 
     */ NULL,
    /*
     * 660 
     */ NULL,
    /*
     * 661 
     */ NULL,
    /*
     * 662 
     */ NULL,
    /*
     * 663 
     */ NULL,
    /*
     * 664 
     */ NULL,
    /*
     * 665 
     */ NULL,
    /*
     * 666 
     */ NULL,
    /*
     * 667 
     */ NULL,
    /*
     * 668 
     */ NULL,
    /*
     * 669 
     */ NULL,
    /*
     * 670 
     */ NULL,
    /*
     * 671 
     */ NULL,
    /*
     * 672 
     */ NULL,
    /*
     * 673 
     */ NULL,
    /*
     * 674 
     */ NULL,
    /*
     * 675 
     */ NULL,
    /*
     * 676 
     */ NULL,
    /*
     * 677 
     */ NULL,
    /*
     * 678 
     */ NULL,
    /*
     * 679 
     */ NULL,
    /*
     * 680 
     */ NULL,
    /*
     * 681 
     */ NULL,
    /*
     * 682 
     */ NULL,
    /*
     * 683 
     */ NULL,
    /*
     * 684 
     */ NULL,
    /*
     * 685 
     */ NULL,
    /*
     * 686 
     */ NULL,
    /*
     * 687 
     */ NULL,
    /*
     * 688 
     */ NULL,
    /*
     * 689 
     */ NULL,
    /*
     * 690 
     */ NULL,
    /*
     * 691 
     */ NULL,
    /*
     * 692 
     */ NULL,
    /*
     * 693 
     */ NULL,
    /*
     * 694 
     */ NULL,
    /*
     * 695 
     */ NULL,
    /*
     * 696 
     */ NULL,
    /*
     * 697 
     */ NULL,
    /*
     * 698 
     */ NULL,
    /*
     * 699 
     */ NULL,
    /*
     * 700 
     */ NULL,
    /*
     * 701 
     */ NULL,
    /*
     * 702 
     */ NULL,
    /*
     * 703 
     */ NULL,
    /* 
     * 704 RPL_HELPSTART 
     */ ":%C 704 %s %s :%s",
    /* 
     * 705 RPL_HELPTXT
     */ ":%C 705 %s %s :%s",
    /* 
     * 706 RPL_ENDOFHELP,
     */ ":%C 706 %s %s :����� ��� /HELP.",
    /*
     * 707 
     */ NULL,
    /*
     * 708 
     */ NULL,
    /*
     * 709 
     */ NULL,
    /*
     * 710 
     */ NULL,
    /*
     * 711 
     */ NULL,
    /*
     * 712 
     */ NULL,
    /*
     * 713 
     */ NULL,
    /*
     * 714 
     */ NULL,
    /*
     * 715 
     */ NULL,
    /*
     * 716 
     */ NULL,
    /*
     * 717 
     */ NULL,
    /*
     * 718 
     */ NULL,
    /*
     * 719 
     */ NULL,
    /*
     * 720 
     */ NULL,
    /*
     * 721 
     */ NULL,
    /*
     * 722 
     */ NULL,
    /*
     * 723 
     */ NULL,
    /*
     * 724 
     */ NULL,
    /*
     * 725 
     */ NULL,
    /*
     * 726 
     */ NULL,
    /*
     * 727 
     */ NULL,
    /*
     * 728 
     */ NULL,
    /*
     * 729 
     */ NULL,
    /*
     * 730 
     */ NULL,
    /*
     * 731 
     */ NULL,
    /*
     * 732 
     */ NULL,
    /*
     * 733 
     */ NULL,
    /*
     * 734 
     */ NULL,
    /*
     * 735 
     */ NULL,
    /*
     * 736 
     */ NULL,
    /*
     * 737 
     */ NULL,
    /*
     * 738 
     */ NULL,
    /*
     * 739 
     */ NULL,
    /*
     * 740 
     */ NULL,
    /*
     * 741 
     */ NULL,
    /*
     * 742 
     */ NULL,
    /*
     * 743 
     */ NULL,
    /*
     * 744 
     */ NULL,
    /*
     * 745 
     */ NULL,
    /*
     * 746 
     */ NULL,
    /*
     * 747 
     */ NULL,
    /*
     * 748 
     */ NULL,
    /*
     * 749 
     */ NULL,
    /*
     * 750 
     */ NULL,
    /*
     * 751 
     */ NULL,
    /*
     * 752 
     */ NULL,
    /*
     * 753 
     */ NULL,
    /*
     * 754 
     */ NULL,
    /*
     * 755 
     */ NULL,
    /*
     * 756 
     */ NULL,
    /*
     * 757 
     */ NULL,
    /*
     * 758 
     */ NULL,
    /*
     * 759 
     */ NULL,
    /*
     * 760 
     */ NULL,
    /*
     * 761 
     */ NULL,
    /*
     * 762 
     */ NULL,
    /*
     * 763 
     */ NULL,
    /*
     * 764 
     */ NULL,
    /*
     * 765 
     */ NULL,
    /*
     * 766 
     */ NULL,
    /*
     * 767 
     */ NULL,
    /*
     * 768 
     */ NULL,
    /*
     * 769 
     */ NULL,
    /*
     * 770 
     */ NULL,
    /*
     * 771 
     */ NULL,
    /*
     * 772 
     */ NULL,
    /*
     * 773 
     */ NULL,
    /*
     * 774 
     */ NULL,
    /*
     * 775 
     */ NULL,
    /*
     * 776 
     */ NULL,
    /*
     * 777 
     */ NULL,
    /*
     * 778 
     */ NULL,
    /*
     * 779 
     */ NULL,
    /*
     * 780 
     */ NULL,
    /*
     * 781 
     */ NULL,
    /*
     * 782 
     */ NULL,
    /*
     * 783 
     */ NULL,
    /*
     * 784 
     */ NULL,
    /*
     * 785 
     */ NULL,
    /*
     * 786 
     */ NULL,
    /*
     * 787 
     */ NULL,
    /*
     * 788 
     */ NULL,
    /*
     * 789 
     */ NULL,
    /*
     * 790 
     */ NULL,
    /*
     * 791 
     */ NULL,
    /*
     * 792 
     */ NULL,
    /*
     * 793 
     */ NULL,
    /*
     * 794 
     */ NULL,
    /*
     * 795 
     */ NULL,
    /*
     * 796 
     */ NULL,
    /*
     * 797 
     */ NULL,
    /*
     * 798 
     */ NULL,
    /*
     * 799 
     */ NULL,
    /*
     * 800 
     */ NULL,
    /*
     * 801 
     */ NULL,
    /*
     * 802 
     */ NULL,
    /*
     * 803 
     */ NULL,
    /*
     * 804 
     */ NULL,
    /*
     * 805 
     */ NULL,
    /*
     * 806 
     */ NULL,
    /*
     * 807 
     */ NULL,
    /*
     * 808 
     */ NULL,
    /*
     * 809 
     */ NULL,
    /*
     * 810 
     */ NULL,
    /*
     * 811 
     */ NULL,
    /*
     * 812 
     */ NULL,
    /*
     * 813 
     */ NULL,
    /*
     * 814 
     */ NULL,
    /*
     * 815 
     */ NULL,
    /*
     * 816 
     */ NULL,
    /*
     * 817 
     */ NULL,
    /*
     * 818 
     */ NULL,
    /*
     * 819 
     */ NULL,
    /*
     * 820 
     */ NULL,
    /*
     * 821 
     */ NULL,
    /*
     * 822 
     */ NULL,
    /*
     * 823 
     */ NULL,
    /*
     * 824 
     */ NULL,
    /*
     * 825 
     */ NULL,
    /*
     * 826 
     */ NULL,
    /*
     * 827 
     */ NULL,
    /*
     * 828 
     */ NULL,
    /*
     * 829 
     */ NULL,
    /*
     * 830 
     */ NULL,
    /*
     * 831 
     */ NULL,
    /*
     * 832 
     */ NULL,
    /*
     * 833 
     */ NULL,
    /*
     * 834 
     */ NULL,
    /*
     * 835 
     */ NULL,
    /*
     * 836 
     */ NULL,
    /*
     * 837 
     */ NULL,
    /*
     * 838 
     */ NULL,
    /*
     * 839 
     */ NULL,
    /*
     * 840 
     */ NULL,
    /*
     * 841 
     */ NULL,
    /*
     * 842 
     */ NULL,
    /*
     * 843 
     */ NULL,
    /*
     * 844 
     */ NULL,
    /*
     * 845 
     */ NULL,
    /*
     * 846 
     */ NULL,
    /*
     * 847 
     */ NULL,
    /*
     * 848 
     */ NULL,
    /*
     * 849 
     */ NULL,
    /*
     * 850 
     */ NULL,
    /*
     * 851 
     */ NULL,
    /*
     * 852 
     */ NULL,
    /*
     * 853 
     */ NULL,
    /*
     * 854 
     */ NULL,
    /*
     * 855 
     */ NULL,
    /*
     * 856 
     */ NULL,
    /*
     * 857 
     */ NULL,
    /*
     * 858 
     */ NULL,
    /*
     * 859 
     */ NULL,
    /*
     * 860 
     */ NULL,
    /*
     * 861 
     */ NULL,
    /*
     * 862 
     */ NULL,
    /*
     * 863 
     */ NULL,
    /*
     * 864 
     */ NULL,
    /*
     * 865 
     */ NULL,
    /*
     * 866 
     */ NULL,
    /*
     * 867 
     */ NULL,
    /*
     * 868 
     */ NULL,
    /*
     * 869 
     */ NULL,
    /*
     * 870 
     */ NULL,
    /*
     * 871 
     */ NULL,
    /*
     * 872 
     */ NULL,
    /*
     * 873 
     */ NULL,
    /*
     * 874 
     */ NULL,
    /*
     * 875 
     */ NULL,
    /*
     * 876 
     */ NULL,
    /*
     * 877 
     */ NULL,
    /*
     * 878 
     */ NULL,
    /*
     * 879 
     */ NULL,
    /*
     * 880 
     */ NULL,
    /*
     * 881 
     */ NULL,
    /*
     * 882 
     */ NULL,
    /*
     * 883 
     */ NULL,
    /*
     * 884 
     */ NULL,
    /*
     * 885 
     */ NULL,
    /*
     * 886 
     */ NULL,
    /*
     * 887 
     */ NULL,
    /*
     * 888 
     */ NULL,
    /*
     * 889 
     */ NULL,
    /*
     * 890 
     */ NULL,
    /*
     * 891 
     */ NULL,
    /*
     * 892 
     */ NULL,
    /*
     * 893 
     */ NULL,
    /*
     * 894 
     */ NULL,
    /*
     * 895 
     */ NULL,
    /*
     * 896 
     */ NULL,
    /*
     * 897 
     */ NULL,
    /*
     * 898 
     */ NULL,
    /*
     * 899 
     */ NULL,
    /*
     * 900 
     */ NULL,
    /*
     * 901 
     */ NULL,
    /*
     * 902 
     */ NULL,
    /*
     * 903 
     */ NULL,
    /*
     * 904 
     */ NULL,
    /*
     * 905 
     */ NULL,
    /*
     * 906 
     */ NULL,
    /*
     * 907 
     */ NULL,
    /*
     * 908 
     */ NULL,
    /*
     * 909 
     */ NULL,
    /*
     * 910 
     */ NULL,
    /*
     * 911 
     */ NULL,
    /*
     * 912 
     */ NULL,
    /*
     * 913 
     */ NULL,
    /*
     * 914 
     */ NULL,
    /*
     * 915 
     */ NULL,
    /*
     * 916 
     */ NULL,
    /*
     * 917 
     */ NULL,
    /*
     * 918 
     */ NULL,
    /*
     * 919 
     */ NULL,
    /*
     * 920 
     */ NULL,
    /*
     * 921 
     */ NULL,
    /*
     * 922 
     */ NULL,
    /*
     * 923 
     */ NULL,
    /*
     * 924 
     */ NULL,
    /*
     * 925 
     */ NULL,
    /*
     * 926 
     */ NULL,
    /*
     * 927 
     */ NULL,
    /*
     * 928 
     */ NULL,
    /*
     * 929 
     */ NULL,
    /*
     * 930 
     */ NULL,
    /*
     * 931 
     */ NULL,
    /*
     * 932 
     */ NULL,
    /*
     * 933 
     */ NULL,
    /*
     * 934 
     */ NULL,
    /*
     * 935 
     */ NULL,
    /*
     * 936 
     */ NULL,
    /*
     * 937 
     */ NULL,
    /*
     * 938 
     */ NULL,
    /*
     * 939 
     */ NULL,
    /*
     * 940 
     */ NULL,
    /*
     * 941 
     */ NULL,
    /*
     * 942 
     */ NULL,
    /*
     * 943 
     */ NULL,
    /*
     * 944 
     */ NULL,
    /*
     * 945 
     */ NULL,
    /*
     * 946 
     */ NULL,
    /*
     * 947 
     */ NULL,
    /*
     * 948 
     */ NULL,
    /*
     * 949 
     */ NULL,
    /*
     * 950 
     */ NULL,
    /*
     * 951 
     */ NULL,
    /*
     * 952 
     */ NULL,
    /*
     * 953 
     */ NULL,
    /*
     * 954 
     */ NULL,
    /*
     * 955 
     */ NULL,
    /*
     * 956 
     */ NULL,
    /*
     * 957 
     */ NULL,
    /*
     * 958 
     */ NULL,
    /*
     * 959 
     */ NULL,
    /*
     * 960 
     */ NULL,
    /*
     * 961 
     */ NULL,
    /*
     * 962 
     */ NULL,
    /*
     * 963 
     */ NULL,
    /*
     * 964 
     */ NULL,
    /*
     * 965 
     */ NULL,
    /*
     * 966 
     */ NULL,
    /*
     * 967 
     */ NULL,
    /*
     * 968 
     */ NULL,
    /*
     * 969 
     */ NULL,
    /*
     * 970 
     */ NULL,
    /*
     * 971 
     */ NULL,
    /*
     * 972 
     */ NULL,
    /*
     * 973 
     */ NULL,
    /*
     * 974 
     */ NULL,
    /*
     * 975 
     */ NULL,
    /*
     * 976 
     */ NULL,
    /*
     * 977 
     */ NULL,
    /*
     * 978 
     */ NULL,
    /*
     * 979 
     */ NULL,
    /*
     * 980 
     */ NULL,
    /*
     * 981 
     */ NULL,
    /*
     * 982 
     */ NULL,
    /*
     * 983 
     */ NULL,
    /*
     * 984 
     */ NULL,
    /*
     * 985 
     */ NULL,
    /*
     * 986 
     */ NULL,
    /*
     * 987 
     */ NULL,
    /*
     * 988 
     */ NULL,
    /*
     * 989 
     */ NULL,
    /*
     * 990 
     */ NULL,
    /*
     * 991 
     */ NULL,
    /*
     * 992 
     */ NULL,
    /*
     * 993 
     */ NULL,
    /*
     * 994 
     */ NULL,
    /*
     * 995 
     */ NULL,
    /*
     * 996 
     */ NULL,
    /*
     * 997 
     */ NULL,
    /*
     * 998 
     */ NULL,
    /*
     * 999 
     */ ":%C 999 %s ���������� �����! ",
    /*
     * 1000 
     */ NULL
};

static char *_langname = "greek";
static char *_helpdir = "gr";

#ifndef STATIC_MODULES

char *_version = "$Revision: 1.2 $";

void _modinit(void)
{
    lang_add_table(_langname, _helpdir, lang_greek);
}

void _moddeinit(void)
{
    lang_del_table(_langname);
}

#else 

void lang_greek_init(void)
{
    lang_add_table(_langname, _helpdir, lang_greek);
}

#endif
