#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/time.h>

#include <GL/glx.h>
#include <vulkan/vulkan_core.h>

pid_t pid;
char *log_dir;
char log_location[512];
char prog_name[128];
unsigned long long prev_time = 0;
unsigned short frames = 0;
static uint64_t tot_frames = 0;
uint64_t start_time = 0;

uint64_t prev_mtime = 0;

volatile sig_atomic_t done = 0;


// Function to handle exits - print the total statistics
void on_terminate();
void on_terminate() {
	uint64_t end_time = time(NULL);
	uint64_t tot_time = end_time - start_time;

	fprintf(stdout, "Exiting...\n");
	fprintf(stdout, "Statistics for pid %d, name %s:\n\n", pid, prog_name);
	fprintf(stdout, "Frames\t Time\t Avg. FPS\n");
	fprintf(stdout, "%d\t %d\t %3.3f\n", tot_frames, tot_time, (float)tot_frames / tot_time);
	printf("\n"); 
	done = 1;
	exit(EXIT_SUCCESS);
}

__attribute__ ((constructor))
void init() {
	// Get the startup time
	start_time = time(NULL);
	struct tm tm = *localtime(&start_time);
	
	struct timeval tv;
	gettimeofday(&tv, NULL);
	uint64_t prev_mtime = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);

	
	// Get formatted date and time
	char date[64];
	strftime(date, 64, "%F", &tm);
	printf("%s\n", date);

	char time[64];
	strftime(time, 64, "%X", &tm);
	printf("%s\n", time);

	// Set the signal handler
	struct sigaction action;
	memset(&action, 0, sizeof(struct sigaction));
	action.sa_handler = on_terminate; 	
	sigaction(SIGTERM, &action, NULL);
	sigaction(SIGINT, &action, NULL);

	fprintf(stdout, "Startup...\n");
	log_dir = getenv("LOG_DIR");

	pid = getpid();
	char pid_str[16];
	sprintf(pid_str, "%d", pid);
	
	//Get the process name
	char proc_path[256] = "/proc/";
	strcat(proc_path, pid_str);
	strcat(proc_path, "/comm");

	FILE *proc_file = fopen(proc_path, "r");
	char prog_name_tmp[128];
	
        fscanf(proc_file, "%s", prog_name_tmp); 
	strcpy(prog_name, prog_name_tmp);	

	printf("%s\n", prog_name);

	// Append the name and date to the file name
	// Don't log to file if log_dir is NULL
	if (log_dir != NULL) {
		strcpy(log_location, log_dir);
		strcat(log_location, "/vklogger.");
		strcat(log_location, date);
		strcat(log_location, "_");
		strcat(log_location, time); 
		strcat(log_location, ".csv");	
		fprintf(stdout, "Logging to file: %s\n", log_location);
	} else printf("Not logging to a file.\n");

	fprintf(stdout, "PID: %d\n", pid);
	//fprintf(stdout, "Log location: %s\n", log_location);
}

extern void* __libc_dlsym( void* handle, const char* name );

void fps_logger();
void fps_logger() {
	tot_frames++;
	frames++;

	uint64_t cur_time = time(NULL);
	struct timeval tv;
	gettimeofday(&tv, NULL);

	uint64_t cur_mtime = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
	printf("Frametime: %llu\n", cur_mtime - prev_mtime);
	

	if (log_dir != NULL) {
		printf("Logging..\n");
		FILE *logfile = fopen(log_location, "a");
       	 	fprintf(logfile, "%d,\n", cur_mtime - prev_mtime);
       	 	fclose(logfile);
     	}
	prev_mtime = cur_mtime; 

        if (cur_time > prev_time) {
       	         prev_time = cur_time;
                 fprintf(stdout, "FPS: %d\n", frames);
        	 // Print the fps to a file
		 //printf("%s\n", lod_dir);
		 
		 frames = 0;
	}
}


void* real_dlsym( void* handle, const char* name )
{
	static void *(*the_real_dlsym)( void*, const char* );
	if ( the_real_dlsym == NULL ) {
		void* libdl = dlopen( "libdl.so", RTLD_NOW | RTLD_LOCAL );
		the_real_dlsym = __libc_dlsym( libdl, "dlsym" );
	}
	return the_real_dlsym( handle, name );
}

__attribute__((__visibility__("default")))
extern void glXSwapBuffers(Display *dpy, GLXDrawable drawable) {
	void (*og_glXSwapBuffers)(Display *dpy, GLXDrawable drawable) = real_dlsym(RTLD_NEXT, "glXSwapBuffers");
	fps_logger();
	og_glXSwapBuffers(dpy, drawable);
}

VkResult vkQueuePresentKHR(VkQueue queue, const VkPresentInfoKHR *presentInfo) {
	VkResult (*og_vkQueuePresentKHR)(VkQueue queue, const VkPresentInfoKHR *presentInfo) = 
		real_dlsym(RTLD_NEXT, "vkQueuePresent"); 
	//printf("Hooked vulkan\n");
	return og_vkQueuePresentKHR(queue, presentInfo);
}

VkResult vkQueueSubmit(VkQueue queue, uint32_t submitCount, const VkSubmitInfo *submits, VkFence fence) {
	VkResult (*og_vkQueueSubmit)(VkQueue queue, uint32_t submitCount, const VkSubmitInfo *submits, VkFence fence) = real_dlsym(RTLD_NEXT, "vkQueueSubmit");
	//printf("Hooked vulkan\n");
	fps_logger();
	return og_vkQueueSubmit(queue, submitCount, submits, fence);					}

