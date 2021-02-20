/*
 * xsltproc.c: user program for the XSL Transformation 1.0 engine
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 */

#include <emscripten/emscripten.h>
#include "libxslt/libxslt.h"
#include "libxslt/xsltconfig.h"
#include "libexslt/exslt.h"
#include <stdio.h>
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_TIME_H
#include <time.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
#include <fcntl.h>
#endif
#include <libxml/xmlmemory.h>
#include <libxml/debugXML.h>
#include <libxml/HTMLtree.h>
#include <libxml/xmlIO.h>
#ifdef LIBXML_XINCLUDE_ENABLED
#include <libxml/xinclude.h>
#endif
#ifdef LIBXML_CATALOG_ENABLED
#include <libxml/catalog.h>
#endif
#include <libxml/parser.h>
#include <libxml/parserInternals.h>
#include <libxml/uri.h>

#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>
#include <libxslt/extensions.h>
#include <libxslt/security.h>

#include <libexslt/exsltconfig.h>

#if defined(HAVE_SYS_TIME_H)
#include <sys/time.h>
#elif defined(HAVE_TIME_H)
#include <time.h>
#endif

#ifdef HAVE_SYS_TIMEB_H
#include <sys/timeb.h>
#endif

static int options = XSLT_PARSE_OPTIONS;

/*
 * Entity loading control and customization.
 */

EM_JS(const char*, xsltprocFetchResource, (const char* url_ptr), {
    let url = UTF8ToString(url_ptr);
    let req = new XMLHttpRequest();
    req.open("GET", url, false);
    req.send();

    if (req.status != 200) {
        console.error(`Request to ${url} failed with ${req.status}`);
        return null;
    }

    let response = req.responseText;
    let len = lengthBytesUTF8(response) + 1;
    let buf = _malloc(len);
    stringToUTF8(response, buf, len);
    return buf;
});

static void xsltprocFree(xmlChar* s) {
    xmlFree(s);
}

static xmlParserInputPtr
xsltprocExternalEntityLoader(const char *filename, const char *ID, xmlParserCtxtPtr ctxt) {
    const xmlChar* result = (const xmlChar*) xsltprocFetchResource(filename);
    if (result == NULL) {
        return NULL;
    }

    xmlParserInputPtr inputStream = xmlNewStringInputStream(ctxt, result);
    if (inputStream == NULL) {
        xmlFree((xmlChar*) result);
        return NULL;
    }

    inputStream->free = xsltprocFree;

    const char* directory = xmlParserGetDirectory(filename);
    inputStream->filename = (char *) xmlCanonicPath((const xmlChar *) filename);
    inputStream->directory = directory;

    if ((ctxt->directory == NULL) && (directory != NULL)) {
        ctxt->directory = (char *) xmlStrdup((const xmlChar *) directory);
    }
    return(inputStream);
}

static const char*
xsltProcess(xmlDocPtr doc, xsltStylesheetPtr cur)
{
    xsltTransformContextPtr ctxt = xsltNewTransformContext(cur, doc);
    if (ctxt == NULL) {
        return NULL;
    }
    xsltSetCtxtParseOptions(ctxt, options);
    
    xmlDocPtr res = xsltApplyStylesheetUser(cur, doc, NULL, NULL, NULL, ctxt);
    if (ctxt->state == XSLT_STATE_ERROR) {
        printf("Transformation in error state\n");
    } else if (ctxt->state == XSLT_STATE_STOPPED) {
        printf("Transformation in stopped state\n");
    }
    xsltFreeTransformContext(ctxt);

    xmlFreeDoc(doc);
    if (res == NULL) {
        printf("no result\n");
        return NULL;
    }

    xmlChar* out = NULL;
    int size = 0;
    if (xmlStrEqual(cur->method, (const xmlChar *)"html")) {
        htmlDocDumpMemory(res, &out, &size);
    } else {
        xmlDocDumpMemory(res, &out, &size);
    }

    xmlFreeDoc(res);
    return (const char*) out;
}

EMSCRIPTEN_KEEPALIVE const char* transform(const char *xslFilename, const char *xml)
{
    xsltStylesheetPtr cur = NULL;
    xmlDocPtr doc = NULL;
    xmlDocPtr style = NULL;
    xsltSecurityPrefsPtr sec = NULL;
    const char* output = NULL;
    const char* xsl = NULL;

    srand(time(NULL));
    xmlInitMemory();

    LIBXML_TEST_VERSION

    sec = xsltNewSecurityPrefs();
    xsltSetDefaultSecurityPrefs(sec);
    xmlSetExternalEntityLoader(xsltprocExternalEntityLoader);

    /*
     * Register the EXSLT extensions and the test module
     */
    exsltRegisterAll();
    xsltRegisterTestModule();

    xsl = xsltprocFetchResource(xslFilename);
    if (xsl == NULL) {
        goto done;
    }

    style = xmlReadMemory(xsl, strlen(xsl), "doc.xslt", NULL, options);
    if (style == NULL) {
        printf("unable to parse XSLT\n");
        cur = NULL;
        goto done;
    }

    cur = xsltParseStylesheetDoc(style);
    if (cur == NULL || cur->errors != 0) {
        goto done;
    }

    doc = xmlReadMemory(xml, strlen(xml), "doc.xml", NULL, options);
    if (doc == NULL)
    {
        printf("unable to parse XML\n");
        goto done;
    }

    output = xsltProcess(doc, cur);
done:
    if (cur != NULL)
    {
        xsltFreeStylesheet(cur);
    }
    if (xsl != NULL)
    {
        xmlFree(BAD_CAST xsl);
    }
    xsltFreeSecurityPrefs(sec);
    xsltCleanupGlobals();
    xmlCleanupParser();
    xmlMemoryDump();
    return output;
}
