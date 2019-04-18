#!/bin/bash

PROG_NAME="$1"
echo $@
for i in "$@"; do
	case $i in
		--log_dir=*)
		LOG_DIR="${i#*=}"
		export LOG_DIR=$LOG_DIR
	esac
done

LD_PRELOAD=./resolv.so $PROG_NAME
