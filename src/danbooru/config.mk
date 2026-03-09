LIBS = -lchlsdl-common -lpcre2-8 -ljson-c -lcurl

CFLAGS = -std=gnu23 -fPIC -Wall -Wextra ${INCS}

LDFLAGS = ${LIBS}
