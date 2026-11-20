# Materialize a writable vendor copy under $(BUILD_DIR)/_vendor/... (third_party source stays read-only).
# Do not edit upstream trees under $(MODULE_ROOT)/$(PROJ)-$(VERSION)/ in the repo; put Celestia fixes in
# the module Makefile or in optional patch files applied to $(WORK_SRC) after the copy step.
#
# Loaded by third_party/Makefile or scripts/build.sh with:
#   make -C <module_dir> -f Makefile -f ../vendor_rules.mk ...
# Requires MODULE_ROOT, PROJ, VERSION, BUILD_DIR from the module Makefile.
# Use $(WORK_SRC) for configure --srcdir / scripts (not $(VENDOR_SRC)).

ifndef THIRD_PARTY_VENDOR_RULES_MK
THIRD_PARTY_VENDOR_RULES_MK := 1

VENDOR_SRC := $(MODULE_ROOT)/$(PROJ)-$(VERSION)
WORK_SRC := $(BUILD_DIR)/_vendor/$(PROJ)-$(VERSION)
TARBALL := $(firstword $(wildcard $(MODULE_ROOT)/$(PROJ)-$(VERSION).tar.gz $(MODULE_ROOT)/$(PROJ)-$(VERSION).tar.bz2 $(MODULE_ROOT)/$(PROJ)-$(VERSION).tar.xz $(MODULE_ROOT)/$(PROJ)-$(VERSION).tar.*))

.PHONY: vendor_tree
vendor_tree:
	@mkdir -p "$(dir $(WORK_SRC))"
	@if [ ! -f "$(WORK_SRC)/.vendor_ready" ]; then \
		echo "third_party/$(PROJ): staging vendor tree -> $(WORK_SRC)"; \
		rm -rf "$(WORK_SRC)"; \
		if [ -d "$(VENDOR_SRC)" ]; then \
			mkdir -p "$(WORK_SRC)" && cp -a "$(VENDOR_SRC)/." "$(WORK_SRC)/"; \
		elif [ -n "$(TARBALL)" ] && [ -f "$(TARBALL)" ]; then \
			mkdir -p "$(BUILD_DIR)/_vendor" && tar xf "$(TARBALL)" -C "$(BUILD_DIR)/_vendor"; \
		else \
			echo "ERROR $(PROJ): need $(VENDOR_SRC) or a tarball $(MODULE_ROOT)/$(PROJ)-$(VERSION).tar.*"; \
			exit 1; \
		fi; \
		touch "$(WORK_SRC)/.vendor_ready"; \
	fi
	@find "$(WORK_SRC)" -type f \( -name '*.sh' -o -name configure -o -name config.guess -o -name config.sub -o -name install-sh -o -name compile \) -exec chmod +x {} + 2>/dev/null || true

# Remove autotools/configure droppings in checked-in $(VENDOR_SRC) (legacy in-tree builds).
.PHONY: vendor_clean_intree
vendor_clean_intree:
	@if [ -z "$(VENDOR_SRC)" ] || [ ! -d "$(VENDOR_SRC)" ]; then exit 0; fi; \
	echo "third_party/$(PROJ): vendor_clean_intree $(VENDOR_SRC)"; \
	if [ -f "$(VENDOR_SRC)/configure" ] && [ -f "$(VENDOR_SRC)/Makefile" ]; then \
		$(MAKE) -C "$(VENDOR_SRC)" distclean 2>/dev/null || true; \
	fi; \
	rm -rf "$(VENDOR_SRC)/autom4te.cache" 2>/dev/null || true; \
	find "$(VENDOR_SRC)" -depth -type d \( -name autom4te.cache -o -name .deps -o -name .libs \) -exec rm -rf {} + 2>/dev/null || true; \
	rm -f "$(VENDOR_SRC)/config.status" "$(VENDOR_SRC)/config.log" "$(VENDOR_SRC)/libtool" \
		"$(VENDOR_SRC)/stamp-h1" "$(VENDOR_SRC)/config.h" "$(VENDOR_SRC)/jconfig.h" 2>/dev/null || true; \
	rm -f "$(VENDOR_SRC)"/*.pc 2>/dev/null || true

endif
