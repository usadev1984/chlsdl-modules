LIBS = -L../chlsdl-common -lchlsdl-common -lpcre2-8

INCS = -I../../include
CFLAGS = -std=gnu23 -fPIC -Wall -Wextra ${INCS}

LDFLAGS = ${LIBS}
