# compile time flags:
# COLOR	- enable colored output

.ONESHELL:

MAKEFLAGS += -rR

include config.mk

ifeq ($(strip $(COLOR)),1)
	CFLAGS += -DCOLOR
endif

ifeq ($(strip $(PREFIX)),)
	PREFIX = /usr/local
endif

LIB_OBJ := $(patsubst %.c, %.o, $(wildcard */*.c)) \
				$(patsubst %.c, %.o, $(wildcard *.c))

LIB_BASENAME := libchlsdl-common
LIB_COMMON   := ${LIB_BASENAME}.so
VER_MAJOR    := 0
VER_MINOR    := 0
VER_PATCH    := 0
LIB_MAJOR    := ${LIB_BASENAME}.so.$(VER_MAJOR)
LIB_MINOR    := ${LIB_BASENAME}.so.$(VER_MAJOR).$(VER_MINOR)
LIB_VERSION  := ${LIB_BASENAME}.so.$(VER_MAJOR).$(VER_MINOR).$(VER_PATCH)

CFLAGS +=  -DCHLSDL_VERSION=\"${VER_MAJOR}.${VER_MINOR}.${VER_PATCH}\" \
				-DCHLSDL_MAJOR="${VER_MAJOR}" -DCHLSDL_MINOR="${VER_MINOR}" \
				-DCHLSDL_PATCH="${VER_PATCH}" \
				-DMODULES_PATH=\"${PREFIX}/lib/chlsdl-modules\"

install:
# mkdir -p ${DESTDIR}${PREFIX}/include/
# cp -rf include/chlsdl ${DESTDIR}${PREFIX}/include/
# # chmod -R 444 ${DESTDIR}${PREFIX}/include/chlsdl

	mkdir -p ${DESTDIR}${PREFIX}/lib/chlsdl-modules
	command cp -df ${LIB_VERSION} ${DESTDIR}${PREFIX}/lib/${LIB_VERSION}
	chmod 755 ${DESTDIR}${PREFIX}/lib/${LIB_VERSION}

	command cp -df ${LIB_MINOR} ${DESTDIR}${PREFIX}/lib/${LIB_MINOR}

	command cp -df ${LIB_MAJOR} ${DESTDIR}${PREFIX}/lib/${LIB_MAJOR}
	command cp -df ${LIB_COMMON} ${DESTDIR}${PREFIX}/lib/${LIB_COMMON}

debug_libchlsdl-common: CFLAGS += -DDEBUG -g3
debug_libchlsdl-common: ${LIB_COMMON}

libchlsdl-common: ${LIB_COMMON}

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
	rm -f ${LIB_OBJ} ${LIB_COMMON} ${LIB_MAJOR} ${LIB_MINOR} ${LIB_VERSION} \
		&> /dev/null

.PHONY: install clean debug_libchlsdl-common libchlsdl-common
# end
