
MCU=atmega32u4
F_CPU=16000000

CROSS_COMPILE=avr-

CC=gcc
OBJCOPY=objcopy

CFLAGS =-Os -flto -frename-registers	\
	-fira-loop-pressure		\
	-fconserve-stack		\
	-fweb				\
	-std=c99			\
	-g3 -Wall			\

LDFLAGS=-Os -flto -fwhole-program	\
	-Wl,--gc-sections 		\
	-Wl,--print-gc-sections		\
	-Wl,-Map=$(@:%.elf=%.map) 	\

ndmx.elf_DEPS :=	\
	ndmx.o		\
	dmx.o		\
	outputs.o	\

ndmx.ihex_DEPS :=	\
	ndmx.elf	\

all_DEPS :=		\
	ndmx.ihex	\

all: $(all_DEPS)

clean:
	rm -f *.o *.a *.elf *.ihex

ALL_FLAGS = CPPFLAGS CFLAGS CXXFLAGS LDFLAGS LIBS FLASH_FLAGS EEPROM_FLAGS

# Add the CPU config to the CFLAGS, but still let the user
# override them if needed.
MCU_CFLAGS=-mmcu=$(MCU) -DF_CPU=$(F_CPU)
MCU_LDFLAGS=-mmcu=$(MCU)
CFLAGS+=$(MCU_CFLAGS)
LDFLAGS+=$(MCU_LDFLAGS)

# Flag to use when creating images
FLASH_FLAGS=-R .eeprom
EEPROM_FLAGS=-j .eeprom --change-section-lma .eeprom=0

define goal_flag
$(1): $(2) += $($(1)_$(2))
endef

define goal
$(1): $($(1)_DEPS) Makefile
$(foreach flag,$(ALL_FLAGS),$(eval $(call goal_flag,$(1),$(flag))))
$(foreach dep,$($(1)_DEPS),$(eval $(call goal,$(dep))))
endef

$(foreach dep,$(all_DEPS),$(eval $(call goal,$(dep))))

%.o: %.c
	$(CROSS_COMPILE)$(CC) -o $@ -c $< $(CPPFLAGS) $(CFLAGS)

%.a:
	rm -f $@
	$(CROSS_COMPILE)$(AR) rs $@ $(filter %.o,$($*.a_DEPS))

%.ihex: %.elf
	$(CROSS_COMPILE)$(OBJCOPY) -O ihex $(FLASH_FLAGS) $< $@

%_eprom.ihex: %.elf
	$(CROSS_COMPILE)$(OBJCOPY) -O ihex $(EEPROM_FLAGS) $< $@

%.elf:
	$(CROSS_COMPILE)$(CC) -o $@ $(filter %.o,$($*.elf_DEPS)) $(LDFLAGS) $(LIBS)

.PHONY: all clean

.SUFFIXES:
