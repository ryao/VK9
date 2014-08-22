.PHONY: clean All

All:
	@echo ----------Building project:[ libschaefergl - Debug ]----------
	@cd "libschaefergl" && "$(MAKE)" -f "libschaefergl.mk"
clean:
	@echo ----------Cleaning project:[ libschaefergl - Debug ]----------
	@cd "libschaefergl" && "$(MAKE)" -f "libschaefergl.mk" clean
