# On Mac OSX gcc is required as clang complains bitterly about the old style
# C delarations used.

# FIXME: parallel build doesn't work due to doc.inl

CC=gcc
DEP=zmac.o mio.o doc.o zi80dis.o

HOSTCC ?= $(CC)

# Some systems like CentOS may require this
YACC=bison -y
# Is there a YACCFLAGS?

zmac: doc.inl $(DEP)
	$(CC) -Wall $(CFLAGS) -o zmac $(DEP)

doc.inl: doc.c doc.txt
	$(HOSTCC) -Wall -DMK_DOC -o doc doc.c
	./doc >/dev/null

clean:
	rm -f doc zmac zmac.c doc.inl $(DEP)
