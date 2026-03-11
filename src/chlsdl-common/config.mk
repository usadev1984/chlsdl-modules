LIBS = -lpcre2-8 -ljson-c -lcurl

INCS = -I../../include
CFLAGS = -std=gnu23 -fPIC -Wall -Wextra ${INCS}

LDFLAGS = ${LIBS}
