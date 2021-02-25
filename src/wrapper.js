var Xslt = {
    transform: () => { throw "XSLT engine not initialized" }
};

(async () => {
    /* XSLT.RAW.JS */

    let module = await createXsltModule();
    Xslt = Object.freeze({
        transform: (xsltFile, xml) => {
            if (!xsltFile) {
                throw "Empty XSLT file name";
            }

            if (!xml) {
                throw "Empty XML String";
            }

            var xsl_ptr = module.allocateUTF8(xsltFile);
            var xml_ptr = module.allocateUTF8(xml);
            try {
                var output_ptr = module._xsltTransform(xsl_ptr, xml_ptr);
                if (!output_ptr) {
                    throw "Transformation failed";
                }

                let output = module.UTF8ToString(output_ptr);
                module._free(output_ptr);
                return output;
            } finally {
                module._free(xsl_ptr);
                module._free(xml_ptr);
            }
        }
    });
})();