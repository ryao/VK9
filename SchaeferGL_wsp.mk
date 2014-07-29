.PHONY: clean All

All:
	@echo ----------Building project:[ Tests - Debug ]----------
	@cd "Tests" && "$(MAKE)" -f "Tests.mk"
clean:
	@echo ----------Cleaning project:[ Tests - Debug ]----------
	@cd "Tests" && "$(MAKE)" -f "Tests.mk" clean
