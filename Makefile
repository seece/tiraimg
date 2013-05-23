all clean:
	$(MAKE) -C src $@
	$(MAKE) -C test $@

tiraimg:
	$(MAKE) -C src $@

tests:
	$(MAKE) -C test $@

check: tests
	./tests
	
.PHONY:	all
.PHONY:	clean 
