/************************************************************************
 *   IRC - Internet Relay Chat, src/protocol.c
 *
 *   Copyright (C)2000-2003 TR-IRCD Development Team
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

#include "struct.h"
#include "common.h"
#include "sys.h"
#include "h.h"
#include "comply.h"
#include "s_conf.h"

#define COMPLY_FAILED 0
#define COMPLY_SUCCEEDED 1

#ifndef STATIC_MODULES

int (*comply_unload_modules) ();
int (*comply_ignore_messages) ();
int (*comply_untokenize) ();
int (*comply_retokenize) ();
int (*comply_unuse_messages) ();
int (*comply_rehash) ();

struct module *pmodule = NULL;

#ifndef HAVE_SHL_LOAD
static char *get_protocol_name(int p)
{
    return (IRCD_PREFIX_LIB "/protocol/native.so");
}
#else
static char *get_protocol_name(int p)
{
    return (IRCD_PREFIX_LIB "/protocol/native.sl");
}
#endif

static int load_protocol_module(char *path)
{
    DYNLINK_TYPE(tmpptr, path);
    char *mod_basename;
    void (*initfunc) (void) = NULL;

    mod_basename = irc_basename(path);

    if (tmpptr == NULL) {
        const char *err = DYNLINK_ERROR;
        logevent_personal("Error [%s] for module %s", err, path);
        MyFree(mod_basename);
        return -1;
    }

    DYNLINK_EXTRACT_SYMBOL(tmpptr, "proto_modinit", "_proto_modinit", (void (*)(void)), initfunc);

    if (!initfunc) {
        logevent_call(LogSys.modnoinit, path);
        sendto_ops("Module %s has no _modinit() function", path);
        MyFree(mod_basename);
        return -1;
    }

    pmodule = (struct module *) MyMalloc(sizeof(struct module));
    pmodule->address = tmpptr;
    pmodule->version = strdup(mod_basename);
    pmodule->name = strdup(mod_basename);
    pmodule->flags = MODULE_PERSISTENT;
    initfunc();

    MyFree(mod_basename);
    return 1;
}

static int do_nothing()
{
    return 1;
}

int init_protocol(int p)
{

    int (*func) (void) = NULL;
    char *name;

    name = get_protocol_name(p);

    if (!load_protocol_module(name)) {
	logevent_call(LogSys.noprotocol, name);
	return COMPLY_FAILED;
    }

    DYNLINK_EXTRACT_SYMBOL(pmodule->address, "_comply_unload_modules", "__comply_unload_module", (int (*)(void)), func);

    if (func)
	comply_unload_modules = func;
    else
	comply_unload_modules = do_nothing;

    DYNLINK_EXTRACT_SYMBOL(pmodule->address, "_comply_ignore_messages", "__comply_ignore_messages", (int (*)(void)), func);

    if (func)
	comply_ignore_messages = func;
    else
	comply_ignore_messages = do_nothing;

    DYNLINK_EXTRACT_SYMBOL(pmodule->address, "_comply_untokenize", "__comply_untokenize", (int (*)(void)), func);

    if (func)
	comply_untokenize = func;
    else
	comply_untokenize = do_nothing;

    DYNLINK_EXTRACT_SYMBOL(pmodule->address, "_comply_retokenize", "__comply_retokenize", (int (*)(void)), func);

    if (func)
	comply_retokenize = func;
    else
	comply_retokenize = do_nothing;

    DYNLINK_EXTRACT_SYMBOL(pmodule->address, "_comply_unuse_messages", "__comply_unused_messages", (int (*)(void)), func);

    if (func)
	comply_unuse_messages = func;
    else
	comply_unuse_messages = do_nothing;

    DYNLINK_EXTRACT_SYMBOL(pmodule->address, "_comply_rehash", "__comply_rehash", (int (*)(void)), func);

    if (func)
	comply_rehash = func;
    else
	comply_rehash = do_nothing;

    return protocol_comply();
}

int protocol_comply()
{

    if (!comply_unload_modules()) {
	logevent_call(LogSys.generalerror, "Failed in unloading modules to comply");
	return COMPLY_FAILED;
    }

    if (!comply_ignore_messages()) {
	logevent_call(LogSys.generalerror, "Failed in creating ignore messages to comply");
	return COMPLY_FAILED;
    }

    if (!comply_untokenize()) {
	logevent_call(LogSys.generalerror, "Failed in untokenization to comply");
	return COMPLY_FAILED;
    }

    if (!comply_retokenize()) {
	logevent_call(LogSys.generalerror, "Failed in retokenization to comply");
	return COMPLY_FAILED;
    }

    if (!comply_unuse_messages()) {
	logevent_call(LogSys.generalerror, "Failed in unusing messages to comply");
	return COMPLY_FAILED;
    }

    return COMPLY_SUCCEEDED;
}

int protocol_rehash()
{
    comply_rehash();
    return 1;
}

#else

extern void proto_modinit(void);

extern int _comply_unload_modules(void);
extern int _comply_ignore_messages(void);
extern int _comply_retokenize(void);
extern int _comply_untokenize(void);
extern int _comply_unuse_messages(void);
extern int _comply_rehash(void);

int init_protocol(int p)
{
    proto_modinit();

    return protocol_comply();
}

int protocol_comply()
{

    if (!_comply_unload_modules()) {
	logevent_call(LogSys.generalerror, "Failed in disabling messages to comply");
	return COMPLY_FAILED;
    }

    if (!_comply_ignore_messages()) {
	logevent_call(LogSys.generalerror, "Failed in creating ignore messages to comply");
	return COMPLY_FAILED;
    }

    if (!_comply_untokenize()) {
	logevent_call(LogSys.generalerror, "Failed in untokenization to comply");
	return COMPLY_FAILED;
    }

    if (!_comply_retokenize()) {
	logevent_call(LogSys.generalerror, "Failed in retokenization to comply");
	return COMPLY_FAILED;
    }

    if (!_comply_unuse_messages()) {
	logevent_call(LogSys.generalerror, "Failed in unusing messages to comply");
	return COMPLY_FAILED;
    }

    return COMPLY_SUCCEEDED;
}

int protocol_rehash()
{
    _comply_rehash();
    return 1;
}

#endif
