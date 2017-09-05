# http://www.sqlite.org/download.html

SQLITE_VERSION  ?= 3200000
SQLITE_YEAR     ?= 2017

SQLITE_BASENAME := sqlite-amalgamation-$(SQLITE_VERSION)
# Complete URL sample: http://www.sqlite.org/2017/sqlite-amalgamation-3200000.zip
SQLITE_URL      := http://www.sqlite.org/$(SQLITE_YEAR)/$(SQLITE_BASENAME).zip

CC = gcc
CPP = g++

LIB = -ldl -lpthread 
SHELL_FLAGS = \
-DSQLITE_DEBUG \
-DSQLITE_ENABLE_EXPLAIN_COMMENTS \
-DSQLITE_ENABLE_SELECTTRACE \
-DSQLITE_ENABLE_WHERETRACE


UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
	LIBNAME := synonym_tokenizer.dylib
	SHAREDNAME := sqlite3.dylib
else
	LIBNAME := synonym_tokenizer.so
	SHAREDNAME := sqlite3.so
endif

${LIBNAME}: ${SHAREDNAME}
	${CPP} src/SynonymTokenizer.cpp -shared -rdynamic -o ${LIBNAME} -I build/ -fPIC ${LIB} ${SHELL_FLAGS}

${SHAREDNAME}: build/sqlite3.c
	${CC} $^ -c -o $@ -fPIC

# Unpack
build/sqlite3.c: $(SQLITE_BASENAME).zip
	unzip -oq "$<"
	rm -rf build
	mv "$(SQLITE_BASENAME)" build
	touch "$@"

# Download
$(SQLITE_BASENAME).zip:
	wget -N -c "$(SQLITE_URL)"

sqlite3-shell: build/sqlite3.c
	${CC} build/sqlite3.c build/shell.c ${LIB} -o $@ ${SHELL_FLAGS}

clean:
	rm -f "$(SQLITE_BASENAME).zip"
	rm -rf "$(SQLITE_BASENAME)"
	rm -rf build
	rm -rf obj
	rm -rf libs
	rm -f ${LIBNAME}
	rm -f ${SHAREDNAME}
	rm -f sqlite3-shell
