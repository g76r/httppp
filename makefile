.PHONY: default links

default: links

links:
	cd httppp-build-windows/release; \
	  rm -f qpcap.dll; \
	  ln -s ../../libqpcap-build-windows/release/qpcap.dll
