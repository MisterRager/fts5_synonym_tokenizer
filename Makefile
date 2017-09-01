# http://www.sqlite.org/download.html

SQLITE_VERSION  ?= 3200000
SQLITE_YEAR     ?= 2017

SQLITE_BASENAME := sqlite-amalgamation-$(SQLITE_VERSION)
# Complete URL sample: http://www.sqlite.org/2017/sqlite-amalgamation-3200000.zip
SQLITE_URL      := http://www.sqlite.org/$(SQLITE_YEAR)/$(SQLITE_BASENAME).zip

CC = gcc
CPP = g++

LIB = -lpthread -ldl

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
	LIBNAME := synonym_tokenizer.dylib
else
	LIBNAME := synonym_tokenizer.so
endif

${LIBNAME}: sqlite3.so
	${CPP} src/SynonymTokenizer.cpp -c -o ${LIBNAME} -I build/

sqlite3.so: build/sqlite3.c
	${CC} $^ -c ${LIB} -o $@ 

# Unpack
build/sqlite3.c: $(SQLITE_BASENAME).zip
	unzip -oq "$<"
	rm -rf build
	mv "$(SQLITE_BASENAME)" build
	touch "$@"

# Download
$(SQLITE_BASENAME).zip:
	wget -N -c "$(SQLITE_URL)"

clean:
	rm -f "$(SQLITE_BASENAME).zip"
	rm -rf "$(SQLITE_BASENAME)"
	rm -rf build
	rm -rf obj
	rm -rf libs
	rm -f synonym_tokenizer.so
	rm -f sqlite3.so

