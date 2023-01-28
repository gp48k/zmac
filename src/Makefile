# On Mac OSX gcc is required as clang complains bitterly about the old style
# C delarations used.

targets = zmac doc
toclean = $(targets)

zmac_deps = doc.inl
zmac_objs = zmac.o mio.o doc.o zi80dis.o
toclean += $(zmac_objs)

doc_objs = doc.c

CFLAGS = -Wall -O2
LDFLAGS = -Wall -O2

# Some systems like CentOS may require this
# YACC=bison -y
# Is there a YACCFLAGS?
# LCU: Wed Jan 25 08:22:19 CET 2023
# Yes, it is called YFLAGS

zmac: $(zmac_deps) $(zmac_objs)
	$(CXX) $(LDFLAGS) $($@_ldfl) -o $@ $($@_objs) $($@_libs)

doc: $(doc_deps) $(doc_objs)
	$(CC) $(LDFLAGS) $($@_ldfl) -DMK_DOC -o $@ $($@_objs) $($@_libs)

doc.inl: doc doc.txt
	./doc >/dev/null

clean:
	rm -f $(toclean)

zmac.o: y.tab.h
