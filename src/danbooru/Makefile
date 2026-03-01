# compile time flags:
# COLOR 	- enable colored output

.ONESHELL:

MAKEFLAGS += -rR

include config.mk
override INCS += -I../../lib

ifeq ($(strip $(COLOR)),1)
	CFLAGS += -DCOLOR
endif

ifeq ($(strip $(PREFIX)),)
	PREFIX = /usr/local
endif

LIB_OBJ := $(patsubst %.c, %.o, $(wildcard *.c))

# outputs
LIB_BASENAME := libdanbooru
LIB_COMMON   := ${LIB_BASENAME}.so
VER_MAJOR    := 0
VER_MINOR    := 0
VER_PATCH    := 0
LIB_MAJOR    := ${LIB_BASENAME}.so.$(VER_MAJOR)
LIB_MINOR    := ${LIB_BASENAME}.so.$(VER_MAJOR).$(VER_MINOR)
LIB_PATCH    := ${LIB_BASENAME}.so.$(VER_MAJOR).$(VER_MINOR).$(VER_PATCH)
LIB_VERSION  := ${LIB_BASENAME}.so.$(VER_MAJOR).$(VER_MINOR).$(VER_PATCH)

install:
	mkdir -p ${DESTDIR}${PREFIX}/lib/chlsdl-modules

	cp -f ${LIB_VERSION} ${DESTDIR}${PREFIX}/lib/chlsdl-modules/${LIB_VERSION}
	chmod 755 ${DESTDIR}${PREFIX}/lib/chlsdl-modules/${LIB_VERSION}

	cp -f ${LIB_MINOR} ${DESTDIR}${PREFIX}/lib/chlsdl-modules/${LIB_MINOR}

	cp -f ${LIB_MAJOR} ${DESTDIR}${PREFIX}/lib/chlsdl-modules/${LIB_MAJOR}
	cp -f ${LIB_COMMON} ${DESTDIR}${PREFIX}/lib/chlsdl-modules/${LIB_COMMON}

debug_libdanbooru: CFLAGS += -DDEBUG -g3
debug_libdanbooru: ${LIB_COMMON}

libdanbooru: ${LIB_COMMON}

${LIB_COMMON}: ${LIB_MAJOR}
	ln -sf $^ $@

${LIB_MAJOR}: ${LIB_MINOR}
	ln -sf $^ $@

${LIB_MINOR}: ${LIB_VERSION}
	ln -sf $^ $@

${LIB_VERSION}: ${LIB_OBJ}
	${CC} -shared -Wl,-soname,${LIB_MAJOR} ${LDFLAGS} ${CFLAGS} -o $@ $^ || exit 1

${LIB_OBJ}: %.o: %.c
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ -c $^ || exit 1

clean:
	rm -f ${LIB_COMMON} ${LIB_MAJOR} ${LIB_MINOR} ${LIB_PATCH} ${LIB_OBJ} \
		&> /dev/null

.PHONY: debug_libdanbooru libdanbooru install clean
# end
