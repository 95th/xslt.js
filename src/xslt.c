/*
 * xslt.c: user program for the XSL Transformation 1.0 engine intended to compile to WebAssembly.
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 * Gurwinder Singh <vargwin@gmail.com>
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
#include <libxml/xmlmemory.h>
#include <libxml/debugXML.h>
#include <libxml/HTMLtree.h>
#include <libxml/xmlIO.h>
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

#ifdef HAVE_SYS_TIMEB_H
#include <sys/timeb.h>
#endif

static int options = XSLT_PARSE_OPTIONS;

/*
 * Entity loading control and customization.
 */

EM_JS(char *, xsltJsDownloadFile, (const char *urlPtr), {
    var url = UTF8ToString(urlPtr);
    var req = new XMLHttpRequest();
    req.open("GET", url, false);
    req.send();

    if (req.status != 200)
    {
        return null;
    }

    return allocateUTF8(req.responseText);
});

static void
xsltJsFree(xmlChar *s)
{
    xmlFree(s);
}

static xmlParserInputPtr
xsltJsEntityLoader(const char *filename, const char *ID, xmlParserCtxtPtr ctxt)
{
    if (ID)
    {
        // Just to ignore unused parameter warning
    }

    const xmlChar *result = (const xmlChar *)xsltJsDownloadFile(filename);
    if (result == NULL)
    {
        return NULL;
    }

    xmlParserInputPtr input = xmlNewStringInputStream(ctxt, result);
    if (input == NULL)
    {
        xmlFree(BAD_CAST result);
        return NULL;
    }

    input->free = xsltJsFree;
    const char *directory = xmlParserGetDirectory(filename);
    input->filename = (char *)xmlCanonicPath((const xmlChar *)filename);
    input->directory = directory;

    if ((ctxt->directory == NULL) && (directory != NULL))
    {
        ctxt->directory = (char *)xmlStrdup((const xmlChar *)directory);
    }

    return input;
}

static const char *
xsltJsApplyXslt(xmlDocPtr xml_doc, xsltStylesheetPtr style)
{
    xsltTransformContextPtr ctxt = xsltNewTransformContext(style, xml_doc);
    if (ctxt == NULL)
    {
        return NULL;
    }

    xsltSetCtxtParseOptions(ctxt, options);

    xmlDocPtr res = xsltApplyStylesheetUser(style, xml_doc, NULL, NULL, NULL, ctxt);
    if (ctxt->state == XSLT_STATE_ERROR)
    {
        printf("Transformation in error state\n");
    }
    else if (ctxt->state == XSLT_STATE_STOPPED)
    {
        printf("Transformation in stopped state\n");
    }

    xsltFreeTransformContext(ctxt);
    xmlFreeDoc(xml_doc);

    if (res == NULL)
    {
        printf("no result\n");
        return NULL;
    }

    xmlChar* out;
    int size;
    xsltSaveResultToString(&out, &size, res, style);
    xmlFreeDoc(res);
    return (const char *)out;
}

EMSCRIPTEN_KEEPALIVE const char *
xsltJsTransform(const char *xsl_filename, const char *xml)
{
    xsltStylesheetPtr style = NULL;
    xmlDocPtr xml_doc = NULL;
    xmlDocPtr xsl_doc = NULL;
    xsltSecurityPrefsPtr sec = NULL;
    const char *output = NULL;
    const char *xsl = NULL;

    srand(time(NULL));
    xmlInitMemory();

    LIBXML_TEST_VERSION

    sec = xsltNewSecurityPrefs();
    xsltSetDefaultSecurityPrefs(sec);
    xmlSetExternalEntityLoader(xsltJsEntityLoader);

    /*
     * Register the EXSLT extensions and the test module
     */
    exsltRegisterAll();
    xsltRegisterTestModule();

    xsl = xsltJsDownloadFile(xsl_filename);
    if (xsl == NULL)
    {
        goto done;
    }

    xsl_doc = xmlReadMemory(xsl, strlen(xsl), xsl_filename, NULL, options);
    if (xsl_doc == NULL)
    {
        printf("unable to parse XSLT\n");
        style = NULL;
        goto done;
    }

    style = xsltParseStylesheetDoc(xsl_doc);
    if (style == NULL || style->errors != 0)
    {
        goto done;
    }

    xml_doc = xmlReadMemory(xml, strlen(xml), "[XML]", NULL, options);
    if (xml_doc == NULL)
    {
        printf("unable to parse XML\n");
        goto done;
    }

    output = xsltJsApplyXslt(xml_doc, style);
done:
    if (style != NULL)
    {
        xsltFreeStylesheet(style);
    }

    if (xsl != NULL)
    {
        xmlFree(BAD_CAST xsl);
    }

    xsltFreeSecurityPrefs(sec);
    xsltCleanupGlobals();
    xmlCleanupParser();
    return output;
}
