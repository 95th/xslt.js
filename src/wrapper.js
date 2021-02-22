var Xslt = (function() {
    let Module = {};

    /* XSLT.RAW.JS */

    return Object.freeze({
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
                var output_ptr = Module._xsltTransform(xsl_ptr, xml_ptr);
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
    });
})();