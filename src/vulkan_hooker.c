#define _GNU_SOURCE

#include "libperflogger.h"
#include "symbol_resolver.h"

#include <dlfcn.h>
#include <stdint.h>
//#include <vulkan/vulkan_core.h>

PERFLOGGER_EXPORT
void *vkQueueSubmit(void *queue, void  *submitCount, void *submits, void *fence) {
	void *(*real_func)(void*, void*, void*, void*) = 
		get_real_function(RTLD_NEXT, __func__);
	fps_logger();
	return real_func(queue, submitCount, submits, fence);
}
