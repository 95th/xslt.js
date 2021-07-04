# xslt.js

XSLT tranformation library for the web in modern web browsers with WebAssembly support.

It is created by compiling Daniel Vellard's excellent [libxslt](https://github.com/GNOME/libxslt) library to WebAssembly using emscripten.

## Usage

Include `xslt.js` in the your HTML page and use:

```javascript
const engine = await createXsltEngine();

const xsltFile = "path/to/file.xslt";
const xmlString = "<foo><bar/></foo>";

const output = await engine.transform(xsltFile, xmlString);
```

An `XsltEngine` instance may be used to perform any number of transformations but **not** in parallel.

## Why not use XSLTProcessor

- `XSLTProcessor` is not a standard API and can be removed at any point.
- In Google Chrome, and by extension the new MS Edge, do not support all the functionality of XSLT by default. Example `document` function in expressions

## License

For license, see https://gitlab.gnome.org/GNOME/libxslt
