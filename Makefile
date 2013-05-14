all clean:
	$(MAKE) -C src $@
	$(MAKE) -C src/test $@

tiraimg:
	$(MAKE) -C src $@

test:
	$(MAKE) -C src/test $@

check: test
	./test
	
.PHONY:	all
.PHONY:	check
.PHONY:	clean 
