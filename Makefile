SOURCEDIR=src/
SOURCES=$(wildcard $(SOURCEDIR)*.c)

all: perflogger

perflogger:
	gcc $(SOURCES) -fpic -shared -DHOOK_DLSYM -o perflogger.so

