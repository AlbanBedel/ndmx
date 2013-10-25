
BOARD=arduino_micro

# Get the MCU from the board config
BOARD_H := boards/$(BOARD).h
BOARD_O := $(patsubst %.c,%.o,$(wildcard boards/$(BOARD).c))
MCU := $(shell echo MCU | $(CPP) -E -P -imacros $(BOARD_H) -)

CROSS_COMPILE=avr-

CC=gcc
OBJCOPY=objcopy

CFLAGS =-Os -flto 			\
	-std=gnu99			\
	-g3 -Wall			\

LDFLAGS=-Os -flto -fwhole-program	\
	-Wl,-Map=$(@:%.elf=%.map) 	\

ndmx.elf_LDFLAGS=ndmx.x

ndmx.elf_DEPS :=	\
	ndmx.o		\
	dmx.o		\
	outputs.o	\
	config.o	\
	nano_vm.o	\
	$(BOARD_O)	\

ndmx.flash.ihex_DEPS :=		\
	ndmx.elf		\

ndmx.eeprom.ihex_DEPS :=	\
	ndmx.elf		\

all_DEPS :=			\
	ndmx.flash.ihex		\
	ndmx.eeprom.ihex	\

all: $(all_DEPS)

clean:
	rm -f *.o *.a *.elf *.ihex

ALL_FLAGS = CPPFLAGS CFLAGS CXXFLAGS LDFLAGS LIBS FLASH_FLAGS EEPROM_FLAGS

CPPFLAGS+=-include $(BOARD_H)
CFLAGS+=-mmcu=$(MCU)
LDFLAGS+=-mmcu=$(MCU)

# Flag to use when creating images
FLASH_FLAGS=-R .eeprom
EEPROM_FLAGS=-j .eeprom --change-section-lma .eeprom=0

define goal_flag
$(1): $(2) += $($(1)_$(2))
endef

define goal
$(1): $($(1)_DEPS) Makefile
$(foreach flag,$(ALL_FLAGS),$(eval $(call goal_flag,$(1),$(flag))))
$(foreach dep,$($(1)_DEPS),$(eval $(if $($(dep)_DEFINED),,$(call goal,$(dep)))))
$(1)_DEFINED := true
endef

$(foreach dep,$(all_DEPS),$(eval $(call goal,$(dep))))

%.o: %.c
	$(CROSS_COMPILE)$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -c $<

%.a:
	rm -f $@
	$(CROSS_COMPILE)$(AR) rs $@ $(filter %.o,$($*.a_DEPS))

%.eeprom.ihex: %.elf
	$(CROSS_COMPILE)$(OBJCOPY) -O ihex $(EEPROM_FLAGS) $< $@

%.flash.ihex: %.elf
	$(CROSS_COMPILE)$(OBJCOPY) -O ihex $(FLASH_FLAGS) $< $@

%.elf:
	$(CROSS_COMPILE)$(CC) $(LDFLAGS) -o $@ $(filter %.o,$($*.elf_DEPS)) $(LIBS)

.PHONY: all clean

.SUFFIXES:
