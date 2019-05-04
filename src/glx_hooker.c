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

#include "libperflogger.h"
#include "symbol_resolver.h"

#include <dlfcn.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

void glXSwapBuffers(void*,void*);
void glXSwapIntervalEXT(void*, void*, int);
int glXSwapIntervalSGI(int);
int glXSwapIntervalMESA(unsigned int);
bool glXMakeCurrent(void*, void*, void*);
void glFinish(void);

void* glXGetProcAddress(const unsigned char*);
void* glXGetProcAddressARB(const unsigned char*);

void *get_real_glx_function(const char *name) {
	void *(*real_glXGetProcAddress)(const unsigned char*) =
		get_real_function(RTLD_NEXT, "glXGetProcAddress");
	void *(*real_glXGetProcAddressARB)(const unsigned char*) =
		get_real_function(RTLD_NEXT, "glXGetProcAddressARB");

	void (*real_func)() = real_glXGetProcAddress((const unsigned char*) name);
	if (real_func != NULL) {
		return real_func;
	}

	real_func = real_glXGetProcAddressARB((const unsigned char*) name);
	if (real_func != NULL) {
		return real_func;
	}

	return get_real_function(RTLD_NEXT, name);
}

void *get_hooked_glx_function(const char *name) {
    if (!strcmp(name, "glXSwapBuffers")) {
        return (void*) glXSwapBuffers;
    } else if (!strcmp(name, "glXSwapIntervalEXT")) {
	    return (void*) glXSwapIntervalEXT;
    } else if (!strcmp(name, "glXGetProcAddressARB")) {
	    return (void*) glXGetProcAddressARB;
    } else if (!strcmp(name, "glXMakeCurrent")) {
	    return (void*) glXMakeCurrent;
    }
    return NULL;
}

PERFLOGGER_EXPORT
void glFinish() {
	void (*real_func)() = get_real_function(RTLD_NEXT, __func__);
	real_func();
}

PERFLOGGER_EXPORT
void glXSwapBuffers(void *dpy, void *drawable) {
	void (*real_func)(void*, void*) = get_real_glx_function(__func__);
	fps_logger();
	//exit(1);
	real_func(dpy, drawable);
}

PERFLOGGER_EXPORT
void glXSwapIntervalEXT(void *dpy, void *drawable, int interval) {
	void (*real_func)(void*, void*, int) = get_real_glx_function(__func__);
	real_func(dpy, drawable, interval);
}

PERFLOGGER_EXPORT
void *glXGetProcAddressARB(const unsigned char *procName) {
	void *(*real_func)(const unsigned char*) = get_real_function(RTLD_NEXT, __func__);

	void *func = get_hooked_glx_function((const char*) procName);
	if (func != NULL) {
		return func;
	}
	return real_func(procName);
}

PERFLOGGER_EXPORT
bool glXMakeCurrent(void *dpy, void *drawable, void *ctx) {
	bool (*real_func)(void*, void*, void*) = get_real_glx_function(__func__);
	return real_func(dpy, drawable, ctx);
}
