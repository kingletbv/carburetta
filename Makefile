CC=gcc

OUT = build/linux
SRC = src
INTERMEDIATE = build/linux/objs

SOURCES = $(filter-out src/tokens_generated_scanners.c,$(wildcard $(SRC)/*.c))
OBJECTS = $(patsubst $(SRC)/%.c,$(INTERMEDIATE)/%.o,$(SOURCES))

.PHONY: all
all: $(OUT)/carburetta $(OUT)/calc $(OUT)/template_scan

$(INTERMEDIATE)/%.o: $(SRC)/%.c
	@mkdir -p $(@D)
	$(CC) -c -o $@ $<

$(OUT)/carburetta: $(OBJECTS)
	$(CC) -o $(OUT)/carburetta $(OBJECTS)

$(INTERMEDIATE)/calc/calc.c: $(OUT)/carburetta examples/calc/calc.cbrt
	mkdir -p $(@D)
	$(OUT)/carburetta examples/calc/calc.cbrt --c $(INTERMEDIATE)/calc/calc.c

$(INTERMEDIATE)/template_scan/template_scan.c: $(OUT)/carburetta examples/template_scan/template_scan.cbrt
	mkdir -p $(@D)
	$(OUT)/carburetta examples/template_scan/template_scan.cbrt --c $(INTERMEDIATE)/template_scan/template_scan.c

$(OUT)/calc: $(INTERMEDIATE)/calc/calc.c
	$(CC) -o $(OUT)/calc $(INTERMEDIATE)/calc/calc.c

$(OUT)/template_scan: $(INTERMEDIATE)/template_scan/template_scan.c
	$(CC) -o $(out)/template_scan $(INTERMEDIATE)/template_scan/template_scan.c

.PHONY: clean
clean:
	@rm -rf $(OUT)

.PHONY: install
install: all
	install $(OUT)/carburetta /usr/local/bin

.PHONY: uninstall
uninstall:
	rm /usr/local/bin/carburetta
