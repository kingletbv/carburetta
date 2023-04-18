CC=gcc

OUT = build/linux
SRC = src
INTERMEDIATE = build/linux/objs

SOURCES = $(filter-out src/tokens_generated_scanners.c,$(wildcard $(SRC)/*.c))
OBJECTS = $(patsubst $(SRC)/%.c,$(INTERMEDIATE)/%.o,$(SOURCES))
TESTS = $(wildcard tester/*.cbrt)
TESTS_C = $(patsubst tester/%.cbrt,$(INTERMEDIATE)/tester/%.c,$(TESTS))

.PHONY: all
all: $(OUT)/carburetta $(OUT)/calc $(OUT)/template_scan $(OUT)/inireader $(OUT)/tester

$(INTERMEDIATE)/%.o: $(SRC)/%.c
	@mkdir -p $(@D)
	$(CC) -c -o $@ $<

$(OUT)/carburetta: $(OBJECTS)
	$(CC) -o $(OUT)/carburetta $(OBJECTS)

$(INTERMEDIATE)/calc/calc.c: $(OUT)/carburetta examples/calc/calc.cbrt
	mkdir -p $(@D)
	$(OUT)/carburetta examples/calc/calc.cbrt --c $(INTERMEDIATE)/calc/calc.c

$(INTERMEDIATE)/inireader/iniparser.c: $(OUT)/carburetta examples/inireader/iniparser.cbrt
	mkdir -p $(@D)
	$(OUT)/carburetta examples/inireader/iniparser.cbrt --c $(INTERMEDIATE)/inireader/iniparser.c --h
    
$(INTERMEDIATE)/template_scan/template_scan.c: $(OUT)/carburetta examples/template_scan/template_scan.cbrt
	mkdir -p $(@D)
	$(OUT)/carburetta examples/template_scan/template_scan.cbrt --c $(INTERMEDIATE)/template_scan/template_scan.c

$(OUT)/calc: $(INTERMEDIATE)/calc/calc.c
	$(CC) -o $(OUT)/calc $(INTERMEDIATE)/calc/calc.c

$(OUT)/inireader: $(INTERMEDIATE)/inireader/iniparser.c examples/inireader/main.c
	$(CC) -o $(OUT)/inireader -I$(INTERMEDIATE)/inireader -Iexamples/inireader $(INTERMEDIATE)/inireader/iniparser.c examples/inireader/main.c

$(OUT)/template_scan: $(INTERMEDIATE)/template_scan/template_scan.c
	$(CC) -o $(OUT)/template_scan $(INTERMEDIATE)/template_scan/template_scan.c

$(INTERMEDIATE)/tester/%.c: tester/%.cbrt
	mkdir -p $(@D)
	$(OUT)/carburetta --x-utf8 $< --c $@ --h

$(INTERMEDIATE)/tester/t3.c: tester/t3.cbrt
	mkdir -p $(@D)
	$(OUT)/carburetta $< --c $@ --h

$(OUT)/tester: $(TESTS_C) tester/tester.c
	$(CC) -o $@ $^

.PHONY: clean
clean:
	@rm -rf $(OUT)

.PHONY: install
install: all
	install $(OUT)/carburetta /usr/local/bin

.PHONY: uninstall
uninstall:
	rm /usr/local/bin/carburetta

.PHONY: test
test: all
	$(OUT)/tester
