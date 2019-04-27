#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdint.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/time.h>

pid_t pid;
char *log_dir;
char log_location[512];
char prog_name[128];
unsigned long long prev_time = 0;
unsigned short frames = 0;
static uint64_t tot_frames = 0;
uint64_t start_time = 0;
uint64_t prev_utime = 0;
uint64_t avg_frametime = 0; 

FILE *logfile;

volatile sig_atomic_t done = 0;

bool ran_destructor = false;

// Variables for launching gnuplot
const char *frametime_unit = "milliseconds";

// Function to handle exits - print the total statistics

__attribute__((destructor))
void on_terminate() {
	// In some cases the destructor may be called twice, so check if it's been ran before
	if (!ran_destructor) {
		ran_destructor = true;
		uint64_t end_time = time(NULL);
		uint64_t tot_time = end_time - start_time;
		// Close the logfile
		//fclose(logfile);

		fprintf(stdout, "Exiting...\n");
		fprintf(stdout, "Statistics for pid %d, name %s:\n\n", pid, prog_name);
		fprintf(stdout, "Frames\t Time\t Avg. FPS\n");
		fprintf(stdout, "%d\t %d\t %3.3f\n", tot_frames, tot_time, (float)tot_frames / tot_time);
		printf("\n");

		// Optional launch of gnuplot with the logfile
		/*char plot_launch_cmd[512];
		sprintf(plot_launch_cmd, "gnuplot -p -e 'set ylabel \"%s\";set xlabel \"frames\";set yrange [0:100]; plot \"%s\" with lines' ", frametime_unit, log_location);

		printf("%s\n", plot_launch_cmd);*/
		done = 1;

		exit(EXIT_SUCCESS);
	}
}

void fps_logger();
void fps_logger() {
	tot_frames++;
	frames++;

	uint64_t cur_time = time(NULL);
	struct timeval tv;
	gettimeofday(&tv, NULL);

	uint64_t cur_utime = (tv.tv_sec * 1000000) + tv.tv_usec;
	//printf("Frametime: %llu\n", cur_utime - prev_utime);
	avg_frametime += cur_utime - prev_utime;

	if (log_dir != NULL) {
		//printf("Logging..\n");
		//FILE *logfile = fopen(log_location, "a");
       	 	fprintf(logfile, "%3.3f\n", (float) (cur_utime - prev_utime) / 1000);
       	 	//fclose(logfile);
     	}
	//FILE *testf = fopen("/home/jussi/ohj/vulkan_fps_logger/testlog.csv", "a");
	//fprintf(testf, "%d,\n", cur_utime - prev_utime);
	//fclose(testf);


	prev_utime = cur_utime; 

        if (cur_time > prev_time) {
       	         prev_time = cur_time;
                 fprintf(stdout, "FPS: %d \t Avg. frametime: %3.3f ms\n", frames, (float) (avg_frametime / frames) / 1000);
		 avg_frametime = 0;
        	 // Print the fps to a file
		 //printf("%s\n", lod_dir);
		 
		 frames = 0;
	}
}

__attribute__ ((constructor))
void init() {
	//exit(1);
	//fopen("/home/jussi/ohj/vulkan_fps_logger/test", "a");

	// Get the startup time
	start_time = time(NULL);
	struct tm tm = *localtime((const long int*) &start_time);
	
	struct timeval tv;
	gettimeofday(&tv, NULL);
	prev_utime = (tv.tv_sec * 1000000) + tv.tv_usec;

	
	// Get formatted date and time
	char date[64];
	strftime(date, 64, "%F", &tm);

	char time[64];
	strftime(time, 64, "%X", &tm);

	// Set the signal handler
	struct sigaction action;
	memset(&action, 0, sizeof(struct sigaction));
	action.sa_handler = on_terminate; 	
	sigaction(SIGTERM, &action, NULL);
	sigaction(SIGINT, &action, NULL);

	fprintf(stdout, "\n\nPerflogger starting...\n\n");
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
		strcat(log_location, "/perflogger.");
		strcat(log_location, date);
		strcat(log_location, "_");
		strcat(log_location, time); 
		strcat(log_location, ".csv");

		// Open the logfile
		logfile = fopen(log_location, "a");
		if (logfile == NULL) {
			fprintf(stderr, "perflogger: Couldn't open the logfile\n");
		}	
		fprintf(stdout, "Logging to file: %s\n", log_location);
	} else fprintf(stdout, "Not logging to a file.\n");

	fprintf(stdout, "PID: %d\n", pid);
}
