COMPILER = g++
FILESYSTEM_FILES = src/main.cpp src/filesystem_data.cpp

build: $(FILESYSTEM_FILES)
	@echo "\n"
	$(COMPILER) $(FILESYSTEM_FILES) -o mg_filesystem -ggdb `pkg-config fuse --cflags --libs`
	@echo "\n"
	@echo 'To Mount: ./mg_filesystem -f [mount point]'
	@echo "\n"
	@echo "mount options:"
	@echo "  -o allow_other  (makes mounted filesystem accessible for other users)"
	@echo "  -o default_permissions  (activates standard file Linux permission checking)"
	@echo "\n"

clean:
	rm ssfs
