/*
This file is part of libperflogger.

Copyright (c) 2019 Jussi Kuokkanen

Libperflogger is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Libperflogger is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with libperflogger.  If not, see <https://www.gnu.org/licenses/>.
*/

#define _GNU_SOURCE

#include "symbol_resolver.h"
#include "libperflogger.h"
#include "glx_hooker.h"

#include <dlfcn.h>
#include <stddef.h>
#include <string.h>

extern void* __libc_dlsym( void *handle, const char* name);
void* dlsym( void*, const char* );

void* get_real_function(void *handle, const char* name)
{
	static void *(*real_dlsym)( void*, const char* );
	if (real_dlsym == NULL ) {
		void* libdl = dlopen( "libdl.so", RTLD_NOW | RTLD_LOCAL );
		real_dlsym = __libc_dlsym( libdl, "dlsym" );
	}
	return real_dlsym( handle, name );
}

#ifdef HOOK_DLSYM
PERFLOGGER_EXPORT
void* dlsym( void* handle, const char* name ) {
	if (!strcmp(name, "dlsym")) {
		return dlsym;
	}

	void* func = get_hooked_glx_function(name);
	if ( func != NULL ) {
		return func;
	}
	// Return other hooked functions here
	return get_real_function( handle, name );
}
#endif
