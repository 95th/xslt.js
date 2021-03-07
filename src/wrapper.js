var xsltTransform = (function () {
    var Module = {};

    /* XSLT.RAW.JS */

    return function (xsltFile, xml) {
        if (!xsltFile) {
            throw "Empty XSLT file name";
        }

        if (!xml) {
            throw "Empty XML String";
        }

        let xsltPtr = Module.allocateUTF8(xsltFile);
        let xmlPtr = Module.allocateUTF8(xml);
        try {
            let outputPtr = Module._xsltJsTransform(xsltPtr, xmlPtr);
            if (!outputPtr) {
                throw "Transformation failed";
            }

            let output = Module.UTF8ToString(outputPtr);
            Module._free(outputPtr);
            return output;
        } finally {
            Module._free(xsltPtr);
            Module._free(xmlPtr);
        }
    };
})();
