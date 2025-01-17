export EMCC_CLOSURE_ARGS = --externs src/externs.js

build/xslt.js: build/xslt.o libs
	@echo "  CCLD     xslt.js"
	@emcc -Oz -flto --closure=1 \
		-s ALLOW_MEMORY_GROWTH=1 \
		-s INITIAL_MEMORY=8MB \
		-s MAXIMUM_MEMORY=1024MB \
		-s "EXPORTED_FUNCTIONS=['_malloc', '_free']" \
		-s "EXPORTED_RUNTIME_METHODS=['allocateUTF8', 'UTF8ToString', 'cwrap', 'ccall']" \
		-s ENVIRONMENT=web \
		-s MODULARIZE=1 \
		-s "EXPORT_NAME='createXsltModule'" \
		-s ASYNCIFY=1 \
		-s "ASYNCIFY_IMPORTS=['xsltJsDownloadFile']" \
		build/xslt.o \
		libxml2/.libs/libxml2.a \
		libxslt/libxslt/.libs/libxslt.a  \
		-o build/xslt.js
	@sed -e '/\/\* XSLT.RAW.JS \*\// {r build/xslt.js' -e 'd}' < src/wrapper.js > build/xslt.new.js
	@mv build/xslt.new.js build/xslt.js

build/xslt.o: src/xslt.c libs
	@echo "  CC       xslt.o"
	@emcc -Oz -flto -Wall -Wextra src/xslt.c -c -o build/xslt.o -Ilibxml2/include -Ilibxslt

libs:
	@$(MAKE) libxml2.la -j4 -C libxml2
	@$(MAKE) libxslt.la -j4 -C libxslt/libxslt

clean:
	@$(MAKE) clean -C libxml2
	@$(MAKE) clean -C libxslt/libxslt
	@rm -f build/*

.PHONY: clean libs
