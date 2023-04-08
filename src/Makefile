# On Mac OSX gcc is required as clang complains bitterly about the old style
# C delarations used.

# FIXME: parallel build doesn't work due to doc.inl

CC=gcc
DEP=zmac.o mio.o doc.o zi80dis.o

HOSTCC ?= $(CC)

# Some systems like CentOS may require this
YACC=bison -y
# Is there a YACCFLAGS?

.PHONY: all clean

all: zmac zmac.1.gz

zmac: doc.inl $(DEP)
	$(CC) -Wall $(CFLAGS) -o zmac $(DEP)

doc: doc.c
	$(HOSTCC) -Wall -DMK_DOC -o doc doc.c
	
doc.inl: doc doc.txt
	./doc >/dev/null

zmac.1: doc
	./doc -man >zmac.1

zmac.1.gz: zmac.1
	gzip -v < zmac.1 > zmac.1.gz

zmac.1.pdf: zmac.1
	groff -t -mandoc -Tpdf zmac.1 > zmac.1.pdf

clean:
	rm -f doc zmac zmac.c doc.inl zmac.1 zmac.1.gz zmac.1.pdf $(DEP)
