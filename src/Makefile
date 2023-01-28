# On Mac OSX gcc is required as clang complains bitterly about the old style
# C delarations used.

targets   = zmac doc zmac.1.pdf zmac.1.gz
toclean   = $(targets)

zmac_deps = doc.inl
zmac_objs = zmac.o mio.o doc.o zi80dis.o
toclean  += $(zmac_objs)

doc_objs  = doc.c

CFLAGS    = -Wall -O2 -I/usr/local/include
LDFLAGS   = -Wall -O2 -L/usr/local/lib

RM       ?= rm -f
RD       ?= rmdir
INSTALL	 ?= /usr/bin/install

prefix   ?= /usr/local
bindir   ?= $(prefix)/bin
mandir   ?= $(prefix)/share/man
man1dir  ?= $(mandir)/man1

own      ?= root
grp      ?= wheel
dmod     ?= 755
pmod     ?= 711
mmod     ?= 644

IFLAGS   ?= -o $(own) -g $(grp)

# Some systems like CentOS may require this
# YACC=bison -y
# Is there a YACCFLAGS?
# LCU: Wed Jan 25 08:22:19 CET 2023
# Yes, it is called YFLAGS

.PHONY: all clean

all: $(targets)
clean:
	rm -f $(toclean)
install: $(targets)
	$(INSTALL) $(IFLAGS) -m $(dmod)        -d $(man1dir)
	$(INSTALL) $(IFLAGS) -m $(dmod)        -d $(bindir)
	$(INSTALL) $(IFLAGS) -m $(pmod) zmac      $(bindir)
	$(INSTALL) $(IFLAGS) -m $(mmod) zmac.1.gz $(man1dir)
uninstall:
	-$(RM) $(man1dir)/zmac.1.gz
	-$(RM) $(bindir)/zmac
	-$(RD) $(man1dir) $(bindir)

zmac: $(zmac_deps) $(zmac_objs)
	$(CXX) $(LDFLAGS) $($@_ldfl) -o $@ $($@_objs) $($@_libs)

doc: $(doc_deps) $(doc_objs)
	$(CC) $(LDFLAGS) $($@_ldfl) -DMK_DOC -o $@ $($@_objs) $($@_libs)

doc.inl: doc doc.txt
	./doc >/dev/null

zmac.1.pdf: zmac.1
	groff -t -mandoc -Tpdf $? >$@

zmac.1.gz: zmac.1
	gzip -v <$? >$@

zmac.o: y.tab.h
