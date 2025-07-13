default:
	$(MAKE) clean
	$(MAKE) build
	./aat

build:
	gcc aat.c stack.c -o aat -Wall -O2

clean:
	rm -f aat