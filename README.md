# libperflogger - Game performance logging library

Libperflogger is a library for monitoring frametimes in OpenGL/Vulkan games for Linux.

## Installation

```
git clone https://github.com/Lurkki14/libperflogger
cd libperflogger
make
```

## Usage

```
LD_PRELOAD=./libperflogger.so <program>
```

For logging to a file, environment varialbe LOG_DIR needs to be set:

```
export LOG_DIR=<dir>
``` 