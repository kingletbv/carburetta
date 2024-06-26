CC ?= gcc
CFLAGS ?= -Wall
CXXFLAGS ?= -Wall
CXXLDFLAGS ?= -lstdc++
LDFLAGS ?=
DESTDIR ?= /usr/local
PREFIX ?=

OUT = build
SRC = src
INTERMEDIATE = build/objs

SOURCES = $(filter-out src/tokens_generated_scanners.c,$(wildcard $(SRC)/*.c))
OBJECTS = $(patsubst $(SRC)/%.c,$(INTERMEDIATE)/%.o,$(SOURCES))
TESTS_SRC = $(wildcard tester/*.cbrt)
TESTS_CPP_SRC = $(wildcard tester/cpp/*.cbrt)
TESTS_C = $(patsubst tester/%.cbrt,$(INTERMEDIATE)/tester/%.c,$(TESTS_SRC))
TESTS_CPP = $(patsubst tester/cpp/%.cbrt,$(INTERMEDIATE)/tester/cpp/%.cpp,$(TESTS_CPP_SRC))
TESTS_CPP_OBJ = $(patsubst tester/cpp/%.cbrt,$(INTERMEDIATE)/tester/cpp/%.o,$(TESTS_CPP_SRC))

TILLY_CBRT_SRC = $(wildcard examples/tilly/*.cbrt)
TILLY_CPP_SRC = $(wildcard examples/tilly/*.cpp)
TILLY_CBRT_CPP_SRC = $(patsubst examples/tilly/%.cbrt,$(INTERMEDIATE)/tilly/%.cpp,$(TILLY_CBRT_SRC))
TILLY_CBRT_CPP_OBJ = $(patsubst examples/tilly/%.cbrt,$(INTERMEDIATE)/tilly/%.o,$(TILLY_CBRT_SRC))
TILLY_CPP_OBJ = $(patsubst examples/tilly/%.cpp,$(INTERMEDIATE)/tilly/%.o,$(TILLY_CPP_SRC))

.PHONY: all
all: $(OUT)/carburetta $(OUT)/calc $(OUT)/template_scan $(OUT)/inireader $(OUT)/tilly $(OUT)/tester

$(INTERMEDIATE)/%.o: $(SRC)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OUT)/carburetta: $(OBJECTS)
	$(CC) -o $(OUT)/carburetta $(OBJECTS) $(LDFLAGS)

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
	$(CC) $(CFLAGS) -o $(OUT)/calc $(INTERMEDIATE)/calc/calc.c $(LDFLAGS)

$(OUT)/inireader: $(INTERMEDIATE)/inireader/iniparser.c examples/inireader/main.c
	$(CC) $(CFLAGS) -o $(OUT)/inireader -I$(INTERMEDIATE)/inireader -Iexamples/inireader $(INTERMEDIATE)/inireader/iniparser.c examples/inireader/main.c $(LDFLAGS)

$(OUT)/template_scan: $(INTERMEDIATE)/template_scan/template_scan.c
	$(CC) $(CFLAGS) -o $(OUT)/template_scan $(INTERMEDIATE)/template_scan/template_scan.c $(LDFLAGS)

.PRECIOUS: $(INTERMEDIATE)/tester/%.c
$(INTERMEDIATE)/tester/%.c: tester/%.cbrt
	mkdir -p $(@D)
	$(OUT)/carburetta $< --c $@ --h

$(INTERMEDIATE)/tester/t3.c: tester/t3.cbrt
	mkdir -p $(@D)
	$(OUT)/carburetta --x-raw $< --c $@ --h

.PRECIOUS: $(INTERMEDIATE)/tester/cpp/%.cpp
$(INTERMEDIATE)/tester/cpp/%.cpp: tester/cpp/%.cbrt
	mkdir -p $(@D)
	$(OUT)/carburetta $< --c $@ --h

$(INTERMEDIATE)/tester/cpp/%.o: $(INTERMEDIATE)/tester/cpp/%.cpp
	$(CC) $(CXXFLAGS) -c $^ -o $@

$(OUT)/tester: $(TESTS_C) $(TESTS_CPP_OBJ) tester/tester.c
	$(CC) $(CFLAGS) -o $@ $^ $(CXXLDFLAGS)

  
.PRECIOUS: $(INTERMEDIATE)/tilly/%.cpp
$(INTERMEDIATE)/tilly/%.cpp: examples/tilly/%.cbrt
	mkdir -p $(@D)
	$(OUT)/carburetta $< --c $@ --h

$(INTERMEDIATE)/tilly/%.o: $(INTERMEDIATE)/tilly/%.cpp $(TILLY_CBRT_CPP_SRC)
	$(CC) $(CXXFLAGS) -c $< -o $@ -I examples/tilly/ -I $(INTERMEDIATE)/tilly/

$(INTERMEDIATE)/tilly/%.o: examples/tilly/%.cpp $(TILLY_CBRT_CPP_SRC)
	$(CC) $(CXXFLAGS) -c $< -o $@ -I examples/tilly/ -I $(INTERMEDIATE)/tilly/

$(OUT)/tilly: $(TILLY_CPP_OBJ) $(TILLY_CBRT_CPP_OBJ)
	$(CC) -o $@ $^ $(CXXLDFLAGS)

.PHONY: clean
clean:
	@rm -rf $(OUT)

.PHONY: install
install: all
	install $(OUT)/carburetta $(DESTDIR)$(PREFIX)/bin

.PHONY: uninstall
uninstall:
	rm $(DESTDIR)$(PREFIX)/bin/carburetta

.PHONY: test
test: all
	$(OUT)/tester

