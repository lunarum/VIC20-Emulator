.PHONY: clean All

All:
	@echo "----------Building project:[ VicWin - Debug ]----------"
	@cd "VicWin" && "$(MAKE)" -f  "VicWin.mk"
clean:
	@echo "----------Cleaning project:[ VicWin - Debug ]----------"
	@cd "VicWin" && "$(MAKE)" -f  "VicWin.mk" clean
