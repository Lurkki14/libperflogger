#ifndef VULKAN_HOOKER_H_
#define VULKAN_HOOKER_H_

void *get_real_vk_function(const char *name);
void *get_hooked_vk_function(const char *name);

#endif
