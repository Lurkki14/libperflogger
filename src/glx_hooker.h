#ifndef GLX_HOOKER_H_
#define GLX_HOOKER_H_

void *get_hooked_glx_function(const char *name);
void *get_real_glx_function(const char *name);

#endif
