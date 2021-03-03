var xsltTransform = (function () {
    function validateArgs(xsltFile, xml) {
        if (!xsltFile) {
            throw "Empty XSLT file name";
        }

        if (!xml) {
            throw "Empty XML String";
        }
    }

    if ("ActiveXObject" in window) {
        return function (xsltFile, xml) {
            validateArgs(xsltFile, xml);

            var xsltDoc = new ActiveXObject("MSXML2.DOMDocument.6.0");
            xsltDoc.setProperty("AllowXsltScript", true);
            xsltDoc.setProperty("AllowDocumentFunction", true);
            xsltDoc.resolveExternals = true;
            xsltDoc.async = false;
            xsltDoc.load(xsltFile);

            var xmlDoc = new ActiveXObject("MSXML2.DOMDocument.6.0");
            xmlDoc.setProperty("AllowXsltScript", true);
            xmlDoc.setProperty("AllowDocumentFunction", true);
            xmlDoc.resolveExternals = true;
            xmlDoc.preserveWhiteSpace = true;
            xmlDoc.async = false;
            xmlDoc.loadXML(xml);

            return xmlDoc.transformNode(xsltDoc.documentElement);
        };
    }

    if (typeof WebAssembly === "object" && typeof WebAssembly.instantiate === "function") {
        var Module = {};

        /* XSLT.RAW.JS */

        return function (xsltFile, xml) {
            validateArgs(xsltFile, xml);

            var xsl_ptr = Module.allocateUTF8(xsltFile);
            var xml_ptr = Module.allocateUTF8(xml);
            try {
                var output_ptr = Module._xsltJsTransform(xsl_ptr, xml_ptr);
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
    }

    throw "XSLT transformation is not supported on this browser";
})();