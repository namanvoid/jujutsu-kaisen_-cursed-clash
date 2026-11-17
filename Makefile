all:
	$(MAKE) -C cpp

clean:
	$(MAKE) -C cpp clean

run:
	$(MAKE) -C cpp run

test:
	$(MAKE) -C cpp test

.PHONY: all clean run test
