
CFLAGS := -O3 -Wall -Iinclude


ifeq ($(OS),Windows_NT)
	
	CC := gcc

	LIBS :=
	OBJFLAGS := -c -DBUILD_DLL
	LIBFLAGS := -shared

	CORE := ezSfx_core_win
	LIB := bin/libezsfx.dll

	forceremove = del /F /Q $(subst /,\\, $(1))

else 

	CC := gcc

	LIBS := -lasound -lpthread
	OBJFLAGS := -c -fPIC
	LIBFLAGS := -shared

	CORE := ezSfx_core_linux_alsa
	LIB := bin/libezsfx.so

	forceremove = rm -rf $(1)

endif