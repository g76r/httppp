.PHONY: default all nsis zip

default: all

all: zip nsis

nsis:
	cd image; makensis -NOCD ../httppp.nsis

zip:
	rm -f httppp-win32-portable.zip
	cd image; zip -r9 ../httppp-win32-portable.zip .
