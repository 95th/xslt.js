var newXsltEngine = (() => {
    /* XSLT.RAW.JS */

    class XsltEngine {
        constructor(mod) {
            this.mod = mod;
            this.fn = mod.cwrap("xsltJsTransform", "number", ["number", "number"], { async: true });
        }

        async transform(xsltFile, xml) {
            if (!xsltFile) {
                throw "Empty XSLT file name";
            }

            if (!xml) {
                throw "Empty XML String";
            }

            let xsltPtr = this.mod.allocateUTF8(xsltFile);
            let xmlPtr = this.mod.allocateUTF8(xml);
            try {
                let outputPtr = await this.fn(xsltPtr, xmlPtr);
                if (!outputPtr) {
                    throw "Transformation failed";
                }

                let output = this.mod.UTF8ToString(outputPtr);
                this.mod._free(outputPtr);
                return output;
            } finally {
                this.mod._free(xsltPtr);
                this.mod._free(xmlPtr);
            }
        }
    }

    return async () => {
        let mod = await newXsltModule();
        return new XsltEngine(mod);
    };
})();