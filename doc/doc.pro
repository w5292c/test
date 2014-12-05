TEMPLATE = aux

QMAKE_DOCS = $$PWD/example.qdocconf
QMAKE_DOCS_OUTPUTDIR = $$OUT_PWD/docs

docstarget.target = docs
mytarget.depends = $${QMAKE_DOCS_OUTPUTDIR}
docstarget.commands = \
  @echo "Building documentation..." && \
  qdoc $${QMAKE_DOCS} -outputdir $${QMAKE_DOCS_OUTPUTDIR} && \
  echo "Documentation is ready."

QMAKE_EXTRA_TARGETS += docstarget
