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
