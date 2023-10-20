CC=gcc
LDFLAGS=-Lfaest-ref/build -lfaest
BINNAME=faest-test
VARIANT=faest_em_128f

.PHONY: faest-test compile copy-files rm-files clean
.SILENT: copy-files rm-files

faest-test: copy-files compile rm-files

compile:
	@printf "\e[0;36m> Compiling $(VARIANT)\e[0;37m\n"
	$(CC) $(LDFLAGS) -o $(BINNAME) crypto_sign.c faest-test.c

copy-files:
	cp faest-ref/faest_defines.h .
	cp faest-ref/build/$(VARIANT)/api.h .
	cp faest-ref/build/$(VARIANT)/crypto_sign.h .
	cp faest-ref/build/$(VARIANT)/crypto_sign.c .
	cp faest-ref/build/$(VARIANT).h .

rm-files:
	$(RM) faest_defines.h
	$(RM) api.h
	$(RM) crypto_sign.h
	$(RM) crypto_sign.c
	$(RM) $(VARIANT).h

clean: rm-files
	$(RM) $(BINNAME)

run: faest-test
	@DYLD_LIBRARY_PATH='faest-ref/build' LD_LIBRARY_PATH='faest-ref/build' ./${BINNAME}
