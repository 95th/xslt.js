const el = document.getElementById("test");

(async () => {
  const xslt = "test.xslt";
  const xml = await fetch("test.xml").then((resp) => resp.text());

  const xsltEngine = await createXsltEngine();
  const html = await xsltEngine.transform(xslt, xml);
  el.innerHTML = html;
})();
