.POHONY: tests
tests:
	cd inet && $(MAKE)
	cd cmds && $(MAKE)
	cd core && $(MAKE)
	cd lua  && $(MAKE)

.POHONY: clean
clean:
	cd inet && $(MAKE) clean
	cd cmds && $(MAKE) clean
	cd core && $(MAKE) clean
	cd lua  && $(MAKE) clean
