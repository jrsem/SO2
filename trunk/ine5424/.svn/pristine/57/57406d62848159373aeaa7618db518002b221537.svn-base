# EPOS Image Makefile

include	../makedefs

NETWORK1	:= -net socket,vlan=1,listen=:1234           -net dump,vlan=1,file=r-$(APPLICATION).pcap -device pcnet,vlan=1,mac=56:34:12:00:54:08 -device pcnet,vlan=2,mac=00:00:00:00:00:01
# add the rest of the line to enable a dhcp server -net user,vlan=1,ipv6=off,restrict=on,net=10.0.1.0/24,host=10.0.1.1,dhcpstart=10.0.1.8
NETWORK2	:= -net socket,vlan=1,connect=127.0.0.1:1234 -net dump,vlan=1,file=r-$(APPLICATION).pcap -device pcnet,vlan=1,mac=56:34:12:00:54:09 -device pcnet,vlan=2,mac=00:00:00:00:00:02

SYSTEM		:= $(shell find . -name $(MACH)_\* -and -not -name $(MACH)_boot)
IMAGE		:= $(addsuffix .img,$(APPLICATION))
IMAGES 		:= $(subst .img,,$(shell find . -name \*.img -printf "%f\n"))
ifeq ($(MMOD),zynq)
FLASH		:= $(IMAGE)
else
FLASH		:= $(addsuffix .hex,$(APPLICATION))
endif
OUTPUT		:= $(addsuffix .out,$(APPLICATION))
EMULATORR	:= $(EMULATOR)
EMULATORS	:= $(subst 1235,1236,$(EMULATOR))
DEBUGGER	:= $(MACH_DEBUGGER)
ifeq ($(DEBUG),1)
ifneq ($(wildcard $(IMG)/$(MACH)_setup),)
DEBUGGER	:= $(DEBUGGER) -ex "add-symbol-file $(call DBSEC,$(SRC)/setup/$(MACH)_setup,.text)"
endif
ifneq ($(wildcard $(IMG)/$(MACH)_init),)
DEBUGGER	:= $(DEBUGGER) -ex "add-symbol-file $(call DBSEC,$(SRC)/init/$(MACH)_init,.text)"
endif
ifneq ($(wildcard $(IMG)/$(MACH)_system),)
DEBUGGER	:= $(DEBUGGER) -ex "add-symbol-file $(call DBSEC,$(SRC)/system/$(MACH)_system,.text)"
endif
endif
DEBUGGERR	:= $(DEBUGGER)
DEBUGGERS	:= $(subst 1235,1236,$(DEBUGGER))

all:		strip $(IMAGE)

flash:		strip $(FLASH)
		$(FLASHER) $(IMG)/$(FLASH)

run:		strip $(IMAGE) run1

run1:
ifeq ($(NODES),1)
		$(EMULATOR)$(IMAGE) | $(TEE) $(OUTPUT)
else
		$(TERM) "$(EMULATOR)$(IMAGE) $(NETWORK1) | $(TEE) $(OUTPUT) \
		&& $(TCPDUMP) $(APPLICATION).pcap > $(APPLICATION).net \
		&& read -p 'Press [Enter] key to close ...'" &
		sleep 2
		$(TERM) "$(EMULATOR)$(IMAGE) $(NETWORK2) | $(TEE) p-$(OUTPUT) \
		&& $(TCPDUMP) p-$(APPLICATION).pcap > p-$(APPLICATION).net \
		&& read -p 'Press [Enter] key to close ...'" &
endif

runall:		FORCE
		$(foreach app,$(IMAGES),$(MAKE) APPLICATION=$(app) prerun_$(app) run1 posrun_$(app);)

debug:		$(IMAGE)
ifeq ($(NODES),1)
		$(EMULATOR)$(IMAGE) | $(TEE) $(OUTPUT) &
		$(DEBUGGER) $(APP)/$(APPLICATION)
else
		$(TERM) "$(EMULATORR)$(IMAGE) $(NETWORK1) | $(TEE) $(OUTPUT) \
		&& $(TCPDUMP) $(APPLICATION).pcap > $(APPLICATION).net \
		&& read -p 'Press [Enter] key to close ...'" &
		$(DEBUGGERR) $(APP)/$(APPLICATION) &
		sleep 2
		$(TERM) "$(EMULATORS)$(IMAGE) $(NETWORK2) | $(TEE) p-$(OUTPUT) \
		&& $(TCPDUMP) p-$(APPLICATION).pcap > p-$(APPLICATION).net \
		&& read -p 'Press [Enter] key to close ...'" &
		$(DEBUGGERS) $(APP)/$(APPLICATION) &
endif

strip:		$(SYSTEM) $(APPLICATION)
		@for file in $^; do $(STRIP) "$${file}"; done;

.PHONY: prebuild_$(APPLICATION) posbuild_$(APPLICATION) prerun_$(APPLICATION)
prerun_$(APPLICATION):
		@echo "Cooling down for 10s ..."
		sleep 10
		@echo "Running $(APPLICATION) ..."
posrun_$(APPLICATION):
		@echo "done!"

clean:
		$(CLEAN) $(SYSTEM) $(APPLICATION) $(IMAGE)

FORCE: