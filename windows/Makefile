.PHONY: default all nsis zip

default: all

all: zip nsis

nsis:
	cd image; makensis -NOCD ../httppp.nsis

zip:
	rm -f httppp_portable.zip
	cd image; zip -r9 ../httppp_portable.zip .
