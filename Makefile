.PHONY: clean All

All:
	@echo "----------Building project:[ Emulate6502 - Debug ]----------"
	@cd "Emulate6502" && "$(MAKE)" -f  "Emulate6502.mk"
clean:
	@echo "----------Cleaning project:[ Emulate6502 - Debug ]----------"
	@cd "Emulate6502" && "$(MAKE)" -f  "Emulate6502.mk" clean
