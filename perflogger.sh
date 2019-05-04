#!/bin/bash

PROG_NAME="$1"
if [ $# -lt 1 ]; then
	echo "error: no program specified"
	echo "use --help for help"
fi
for i in "$@"; do
	case $i in
		--log_dir=*)
		LOG_DIR="${i#*=}"
		export PERFLOGGER_LOG_DIR=$LOG_DIR
		;;

		--use_stdout)
		export PERFLOGGER_USE_STDOUT=1
		;;
		
		--help)
		echo -e "usage: perflogger <program> [options]"
		echo "Options:"
		echo -e "\t --log_dir=<dir> \t Directory for the logfile to be saved to."
		echo -e "\t --help \t \t Display this message."
		echo -e "\t --use_stdout \t Display performance info in stdout."
		exit 0
		;;
	esac
done

LD_PRELOAD="${LD_PRELOAD}:libperflogger.so" $PROG_NAME
