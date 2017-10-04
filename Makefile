.PHONY: clean All

All:
	@echo "----------Building project:[ Emulator - Debug ]----------"
	@cd "Emulate6502" && "$(MAKE)" -f  "Emulator.mk"
clean:
	@echo "----------Cleaning project:[ Emulator - Debug ]----------"
	@cd "Emulate6502" && "$(MAKE)" -f  "Emulator.mk" clean
