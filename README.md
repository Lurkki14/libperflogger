# libperflogger - Game performance logging library

Libperflogger is a library for monitoring frametimes in OpenGL/Vulkan games for Linux.

## Installation

```
git clone https://github.com/Lurkki14/libperflogger
cd libperflogger
make
sudo make install
```
make install installs the libraries in /usr/local/lib/libperflogger, the script in /usr/local/bin and an ld config file into /etc/ld.so.conf.d

## Usage

```
LD_PRELOAD="${LD_PRELOAD}:libperflogger.so" <program>
```
Environment variables:

```
PERFLOGGER_LOG_DIR=<dir> # Directory for the logfile - no logfile is saved if not set
PERFLOGGER_USE_STDOUT=1 # Display performance information in stdout
```

The supplied script can be used to simplify usage once 'make install' is ran:

```
perflogger <program> [options]
```

Options:

```
--log_dir=<dir> # Same as PERFLOGGER_LOG_DIR
--use_stdout # Same as PERFLOGGER_USE_STDOUT=1
--help # Display a help message
```

To use in Steam games, set the following launch option:

```
perflogger %command%
```
or

```
LD_PRELOAD="${LD_PRELOAD}:libperflogger.so" %command%
```
For viewing the logfile with gnuplot:

```
gnuplot -p -e 'set ylabel "milliseconds"; set xlabel "frames"; set yrange [0:100]; plot"<log location>" with lines'
```



