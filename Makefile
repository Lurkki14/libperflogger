SOURCEDIR=src/
SOURCES=$(wildcard $(SOURCEDIR)*.c)
LIB32_DIR=/usr/local/lib/libperflogger/lib32
LIB64_DIR=/usr/local/lib/libperflogger/lib64
BINDIR=/usr/local/bin
all: perflogger

perflogger:
	gcc $(SOURCES) -fpic -shared -DHOOK_DLSYM -o libperflogger.so
	gcc $(SOURCES) -fpic -shared -DHOOK_DLSYM -m32 -o libperflogger32.so

install: all
	install -m 0644 -D -T libperflogger.conf /etc/ld.so.conf.d/libperflogger.conf
	install -m 0755 -D -T perflogger.sh $(BINDIR)/perflogger
	install -m 0755 -D -T libperflogger32.so $(LIB32_DIR)/libperflogger.so
	install -m 0755 -D -T libperflogger.so $(LIB64_DIR)/libperflogger.so
	ldconfig

clean:
	rm libperflogger.so
	rm libperflogger32.so


uninstall:
	rm /etc/ld.so.conf.d/libperflogger.conf
	rm $(BINDIR)/perflogger
	rm $(LIB32_DIR)/libperflogger.so
	rm $(LIB64_DIR)/libperflogger.so

