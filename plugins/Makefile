include ../Makefile.conf

all:
	for i in */; do cd $$i; $(MAKE) all; cd ..; done
clean:
	for i in */; do cd $$i; $(MAKE) clean; cd ..; done
new: clean all
