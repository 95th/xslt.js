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

        var xsltPtr = Module.allocateUTF8(xsltFile);
        var xmlPtr = Module.allocateUTF8(xml);
        try {
            var outputPtr = Module._xsltJsTransform(xsltPtr, xmlPtr);
            if (!outputPtr) {
                throw "Transformation failed";
            }

            var output = Module.UTF8ToString(outputPtr);
            Module._free(outputPtr);
            return output;
        } finally {
            Module._free(xsltPtr);
            Module._free(xmlPtr);
        }
    };
})();
