var Xslt = {
    transform: function() {
        throw "XSLT Engine not initialized yet"
    }
};

(async () => {
    /* XSLT.RAW.JS */

    Module = await Module();
    Xslt = {
        transform: function (xsltFile, xml) {
            if (!xsltFile) {
                throw "Empty XSLT file name";
            }

            if (!xml) {
                throw "Empty XML String";
            }

            var xsl_ptr = Module.allocateUTF8(xsltFile);
            var xml_ptr = Module.allocateUTF8(xml);
            try {
                var output_ptr = Module._transform(xsl_ptr, xml_ptr);
                if (!output_ptr) {
                    throw "Transformation failed";
                }

                let output = Module.UTF8ToString(output_ptr);
                Module._free(output_ptr);
                return output;
            } finally {
                Module._free(xsl_ptr);
                Module._free(xml_ptr);
            }
        }
    };
})();