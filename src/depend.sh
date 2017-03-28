#!/bin/sh
#set -x
SRCDIR="$1"
shift 1
OBJDIR="$1"
shift 1
CPP=g++
#CPP=clang++
case "$SRCDIR" in
    "" | ".") $CPP -MM "$@" | sed -e "s@^\(.*\)\.o:@\1.d $OBJDIR\/\1.o:@" ;;
    *) $CPP -MM "$@" | sed -e "s@^\(.*\).o:@$SRCDIR/\1.d $OBJDIR\/\1.o:@" ;;
esac
echo
echo "# nothing"
