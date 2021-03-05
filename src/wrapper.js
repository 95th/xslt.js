var xsltTransform;
(async () => {
    /* XSLT.RAW.JS */

    let mod = await createXsltModule();
    let xsltJsTransform = mod.cwrap("xsltJsTransform", "number", ["number", "number"], { async: true });

    xsltTransform = async (xsltFile, xml) => {
        if (!xsltFile) {
            throw "Empty XSLT file name";
        }

        if (!xml) {
            throw "Empty XML String";
        }

        let xsltPtr = mod.allocateUTF8(xsltFile);
        let xmlPtr = mod.allocateUTF8(xml);
        try {
            let outputPtr = await xsltJsTransform(xsltPtr, xmlPtr);
            if (!outputPtr) {
                throw "Transformation failed";
            }

            let output = mod.UTF8ToString(outputPtr);
            mod._free(outputPtr);
            return output;
        } finally {
            mod._free(xsltPtr);
            mod._free(xmlPtr);
        }
    };
})();