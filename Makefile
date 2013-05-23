all clean:
	$(MAKE) -C src $@
	$(MAKE) -C test $@

tiraimg:
	$(MAKE) -C src $@

tests:
	$(MAKE) -C test $@

check: tests
	mkdir -p temp
	./tests
	
.PHONY:	all
.PHONY:	clean 
