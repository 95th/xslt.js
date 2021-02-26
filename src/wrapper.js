var Xslt = {
    transform: () => { throw "XSLT engine not initialized" }
};

(async () => {
    /* XSLT.RAW.JS */

    let xsltModule = await createXsltModule();
    Xslt = Object.freeze({
        transform: (xsltFile, xml) => {
            if (!xsltFile) {
                throw "Empty XSLT file name";
            }

            if (!xml) {
                throw "Empty XML String";
            }

            var xsl_ptr = xsltModule.allocateUTF8(xsltFile);
            var xml_ptr = xsltModule.allocateUTF8(xml);
            try {
                var output_ptr = xsltModule._xsltTransform(xsl_ptr, xml_ptr);
                if (!output_ptr) {
                    throw "Transformation failed";
                }

                let output = xsltModule.UTF8ToString(output_ptr);
                xsltModule._free(output_ptr);
                return output;
            } finally {
                xsltModule._free(xsl_ptr);
                xsltModule._free(xml_ptr);
            }
        }
    });
})();