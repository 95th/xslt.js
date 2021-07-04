class XsltEngine {
  constructor(mod) {
    this.mod = mod;
    this._transform = mod.cwrap(
      "xsltJsTransform",
      "number",
      ["number", "number"],
      { async: true }
    );
  }

  async transform(xsltFile, xml) {
    if (!xsltFile) {
      throw "Empty XSLT file name";
    }

    if (!xml) {
      throw "Empty XML String";
    }

    const xsltPtr = this.mod.allocateUTF8(xsltFile);
    const xmlPtr = this.mod.allocateUTF8(xml);

    try {
      const outputPtr = await this._transform(xsltPtr, xmlPtr);
      if (outputPtr === 0) {
        const errPtr = this.mod._xsltJsLastError();
        if (errPtr === 0) {
          return "";
        }

        const err = this.mod.UTF8ToString(errPtr);
        throw "Transformation failed: " + err;
      }

      const output = this.mod.UTF8ToString(outputPtr);
      this.mod._free(outputPtr);
      return output;
    } finally {
      this.mod._free(xsltPtr);
      this.mod._free(xmlPtr);
    }
  }
}

var createXsltEngine = (() => {
  /* XSLT.RAW.JS */

  return async () => {
    const mod = await createXsltModule();
    return new XsltEngine(mod);
  };
})();
