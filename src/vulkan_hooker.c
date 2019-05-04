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
#include "vulkan_hooker.h"

#include <dlfcn.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *vkQueueSubmit(void*, void*, void*, void*);

void *get_real_vk_function(const char *name) {
	void *handle = dlopen("libvulkan.so", RTLD_NOW);
	if (handle == NULL) {
		handle = dlopen("libvulkan.so.1", RTLD_NOW);
	}
	if (handle == NULL) {
		fprintf(stderr, "perflogger: failed to load libvulkan.so\n");
	}

	void *real_func = get_real_function(handle, name);
	if (real_func == NULL) {
		fprintf(stderr, "perflogger: failed to get function %s\n", name);
		exit(EXIT_FAILURE);
	}
	return real_func;
}

void *get_hooked_vk_function(const char *name) {
	if (!strcmp(name, "vkQueueSubmit")) {
		return (void*) vkQueueSubmit;
	}

	return NULL;
}


PERFLOGGER_EXPORT
void *vkQueueSubmit(void *queue, void  *submitCount, void *submits, void *fence) {
	void *(*real_func)(void*, void*, void*, void*) =
		get_real_vk_function(__func__);
	fps_logger();
	return real_func(queue, submitCount, submits, fence);
}

PERFLOGGER_EXPORT
void *vkGetInstanceProcAddr(void *instance, void *pName) {
	void *(*real_func)(void*, void*) = get_real_vk_function(__func__);

	void *hooked_func = get_hooked_vk_function((const char*) pName);
	if (hooked_func != NULL) {
		return hooked_func;
	}
	return real_func(instance, pName);
}
