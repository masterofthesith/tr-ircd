/*
 *   IRC - Internet Relay Chat, src/dlcompat.c
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
 * $Id: dlcompat.c,v 1.3 2003/06/14 13:55:52 tr-ircd Exp $
 */

/*
 * ** jmallett's dl*(3) shims for NSModule(3) systems.
 */

#ifndef HAVE_DLOPEN
#ifndef RTLD_LAZY
#define RTLD_LAZY 2185		/* built-in dl*(3) don't care */
#endif

void undefinedErrorHandler(const char *);
NSModule multipleErrorHandler(NSSymbol, NSModule, NSModule);
void linkEditErrorHandler(NSLinkEditErrors, int, const char *, const char *);
char *dlerror(void);
void *dlopen(char *, int);
int dlclose(void *);
void *dlsym(void *, char *);

static int firstLoad = TRUE;
static int myDlError;
static char *myErrorTable[] = { "Loading file as object failed\n",
    "Loading file as object succeeded\n",
    "Not a valid shared object\n",
    "Architecture of object invalid on this architecture\n",
    "Invalid or corrupt image\n",
    "Could not access object\n",
    "NSCreateObjectFileImageFromFile failed\n",
    NULL
};

void undefinedErrorHandler(const char *symbolName)
{
    sendto_ops("Undefined symbol: %s", symbolName);
    logevent_call(undef_symbol, symbolName);
    return;
}

NSModule multipleErrorHandler(NSSymbol s, NSModule old, NSModule new)
{
    /* XXX
     * ** This results in substantial leaking of memory... Should free one
     * ** module, maybe?
     */
    sendto_ops("Symbol `%s' found in `%s' and `%s'",
	       NSNameOfSymbol(s), NSNameOfModule(old), NSNameOfModule(new));
    logevent_call(toomany_symbols, NSNameOfSymbol(s), NSNameOfModule(old), NSNameOfModule(new));
    /* We return which module should be considered valid, I believe */
    return new;
}

void linkEditErrorHandler(NSLinkEditErrors errorClass, int errnum,
			  const char *fileName, const char *errorString)
{
    sendto_ops("Link editor error: %s for %s", errorString, fileName);
    logevent_call(link_editor, errorString, fileName);
    return;
}

char *dlerror(void)
{
    return myDlError == NSObjectFileImageSuccess ? NULL : myErrorTable[myDlError % 7];
}

void *dlopen(char *filename, int unused)
{
    NSObjectFileImage myImage;
    NSModule myModule;

    if (firstLoad) {
	/*
	 * ** If we are loading our first symbol (huzzah!) we should go ahead
	 * ** and install link editor error handling!
	 */
    NSLinkEditErrorHandlers linkEditorErrorHandlers;

	linkEditorErrorHandlers.undefined = undefinedErrorHandler;
	linkEditorErrorHandlers.multiple = multipleErrorHandler;
	linkEditorErrorHandlers.linkEdit = linkEditErrorHandler;
	NSInstallLinkEditErrorHandlers(&linkEditorErrorHandlers);
	firstLoad = FALSE;
    }
    myDlError = NSCreateObjectFileImageFromFile(filename, &myImage);
    if (myDlError != NSObjectFileImageSuccess) {
	return NULL;
    }
    myModule = NSLinkModule(myImage, filename, NSLINKMODULE_OPTION_PRIVATE);
    return (void *) myModule;
}

int dlclose(void *myModule)
{
    NSUnLinkModule(myModule, FALSE);
    return 0;
}

void *dlsym(void *myModule, char *mySymbolName)
{
    NSSymbol mySymbol;

    mySymbol = NSLookupSymbolInModule((NSModule) myModule, mySymbolName);
    return NSAddressOfSymbol(mySymbol);
}
#endif /* HAVE_DLOPEN */
