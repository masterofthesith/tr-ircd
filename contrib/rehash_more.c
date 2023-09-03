#include "struct.h"
#include "common.h"
#include "sys.h"
#include "h.h"
#include "hook.h"

static int do_rehash_more(struct hook_data *data);

#ifndef STATIC_MODULES

void _modinit(void)
{
    hook_add_hook("doing rehash", (hookfn *)do_rehash_more);
}

void _moddeinit(void)
{
    hook_del_hook("doing rehash", (hookfn *)do_rehash_more);
}

char *_version = "$Revision: 1.2 $";

#else

void rehash_more_init(void)
{
    hook_add_hook("doing rehash", (hookfn *)do_rehash_more);
}

#endif

static int do_rehash_more(struct hook_data *data)
{
    if (!IsAnOper(data->source_p))
	return 0;
    else
	sendto_ops("%C is doing rehash for %s", data->source_p, data->data);
    return 0;
}
