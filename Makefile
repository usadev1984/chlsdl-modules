# compile time flags:
# COLOR 	- enable colored output

.ONESHELL:

MAKEFLAGS += -rR

ifeq ($(strip $(PREFIX)),)
	PREFIX = /usr
endif

MODULES       := $(notdir $(patsubst src/chlsdl-common, , $(wildcard src/*)))
MODULES       := $(foreach target, ${MODULES}, lib${target})
DEBUG_MODULES := $(foreach target, ${MODULES}, debug_${target})

install:
#	only install our headers. each library's makefile will take care of
#	installing it
	mkdir -p ${DESTDIR}${PREFIX}/include/
	cp -rf include/chlsdl-modules ${DESTDIR}${PREFIX}/include/
# 	chmod -R 444 ${DESTDIR}${PREFIX}/include/chlsdl

#	install modules
#	TODO: make sub-makefiles inherit the value of `$PREFIX` and maybe other
#	useful variables from a configuration file
	$(MAKE) -C src/chlsdl-common install PREFIX="${PREFIX}"
	$(foreach module, ${MODULES},$(MAKE) -C $(patsubst lib%,src/%,
		${module}) install PREFIX="${PREFIX}"
	) # intentional

debug_modules: debug_libchlsdl-common .WAIT ${DEBUG_MODULES}

${DEBUG_MODULES}:
	$(MAKE) -C $(patsubst debug_lib%, src/%, $@) $@ PREFIX="${PREFIX}"

debug_libchlsdl-common:
	$(MAKE) -C src/chlsdl-common $@ PREFIX="${PREFIX}"

modules: libchlsdl-common .WAIT ${MODULES}

${MODULES}:
	$(MAKE) -C $(patsubst lib%, src/%, $@) $@ PREFIX="${PREFIX}"

libchlsdl-common:
	$(MAKE) -C src/chlsdl-common $@ PREFIX="${PREFIX}"

clean:
	$(MAKE) -C src/chlsdl-common clean
	$(foreach module, ${MODULES},$(MAKE) -C $(patsubst lib%,src/%,
		${module}) clean
	) # intentional

.PHONY: debug_modules modules debug_libchlsdl-common libchlsdl-common \
			install clean
# end
