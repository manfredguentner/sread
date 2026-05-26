# Installation prefix (default: /usr/local)
prefix ?= /usr/local
bindir = $(prefix)/bin
mandir = $(prefix)/share/man/man1


# Compiler and flags
CC      = cc
PKGCONFIG?= pkg-config
PKGNAME= 

#PKG_CFLAGS!= ${PKGCONFIG} --cflags ${PKGNAME}
#PKG_LIBS!=   ${PKGCONFIG} --libs   ${PKGNAME}

#CFLAGS+= -std=c89 -Os -Wall ${PKG_CFLAGS}
CFLAGS+= -std=c89 -Os -Wall
LDFLAGS+= -s
LDADD+= ${LDFLAGS}
#LDADD+= ${PKG_LIBS}

# Project files
TARGET  = sread
TMAN    = sread.1
SRC     = sread.c

# Default rule
all:

	#$(CC) $(SRC) -o $(TARGET) $(CFLAGS) $(LDADD)
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS) $(LDADD)

# Install binary into $(prefix)/bin
install:
	mkdir -p $(bindir)
	install -m 0755 $(TARGET) $(bindir)/
	mkdir -p $(mandir)
	install -m 0755 $(TMAN) $(mandir)/

# Remove build artifacts
clean:
	rm -f $(TARGET)

# Remove everything including installed binary
uninstall:
	rm -f $(bindir)/$(TARGET)
	rm -f $(mandir)/$(TMAN)

debug:
	echo "$(CC) $(SRC) -o $(TARGET) $(CFLAGS) $(LDADD)"
	echo "$(prefix)"

.PHONY: all install clean uninstall
