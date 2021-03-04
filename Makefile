LIB_XML = libxml2/.libs/libxml2.a
LIB_XSLT = libxslt/libxslt/.libs/libxslt.a 
LIB_EXSLT = libxslt/libexslt/.libs/libexslt.a

build/xslt.js: build/xslt.o $(LIB_XML) $(LIB_XSLT) $(LIB_EXSLT)
	@echo "  CCLD     xslt.js"
	@emcc -O3 --closure=1 \
		-s ALLOW_MEMORY_GROWTH=1 \
		-s INITIAL_MEMORY=64MB \
		-s MAXIMUM_MEMORY=1024MB \
		-s "EXPORTED_FUNCTIONS=['_malloc', '_free']" \
		-s "EXTRA_EXPORTED_RUNTIME_METHODS=['allocateUTF8', 'UTF8ToString']" \
		build/xslt.o \
		$(LIB_XML) \
		$(LIB_XSLT) \
		$(LIB_EXSLT) \
		-o build/xslt.js
	@sed -e '/\/\* XSLT.RAW.JS \*\// {r build/xslt.js' -e 'd}' < src/wrapper.js > build/xslt.new.js
	@mv build/xslt.new.js build/xslt.js

build/xslt.o: src/xslt.c $(LIB_XML) $(LIB_XSLT) $(LIB_EXSLT)
	@echo "  CC       xslt.o"
	@emcc -O3 -Wall -Wextra src/xslt.c -c -o build/xslt.o -Ilibxml2/include -Ilibxslt

$(LIB_EXSLT): $(LIB_XML) $(LIB_XSLT)
	@$(MAKE) libexslt.la -C libxslt/libexslt
	
$(LIB_XSLT): $(LIB_XML)
	@$(MAKE) libxslt.la -C libxslt/libxslt

$(LIB_XML):
	@$(MAKE) libxml2.la -C libxml2

clean:
	@$(MAKE) clean -C libxml2
	@$(MAKE) clean -C libxslt/libxslt
	@$(MAKE) clean -C libxslt/libexslt
	@rm -f build/*

.PHONY: clean
