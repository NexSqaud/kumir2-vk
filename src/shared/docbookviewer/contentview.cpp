#include "contentview.h"
#include "docbookmodel.h"

#include <QtCore>
#include <QtGui>

namespace DocBookViewer {

ContentView::ContentView(QWidget *parent)
    : QTextBrowser(parent)
{
    setOpenLinks(false);
    connect(this, SIGNAL(anchorClicked(QUrl)),
            this, SLOT(handleInternalLink(QUrl)));
}

QSize ContentView::minimumSizeHint() const
{
    return QSize(640, 400);
}

void ContentView::reset()
{
    loadedModel_.clear();
    clear();    
}

void ContentView::renderData(ModelPtr data)
{
    ModelPtr dataToRender = onePageParentModel(data);
    if (dataToRender != loadedModel_) {
        loadedModel_ = dataToRender;
        const QString html = wrapHTML(render(dataToRender));
        setHtml(html);
    }
    if (dataToRender != data) {
        QString anchor = modelToLink(data);
        QUrl anchorUrl("#" + anchor);
        setSource(anchorUrl);
    }
}

QString ContentView::renderChapter(ModelPtr data) const
{
    QString result;
    result += renderTOC(data);
    foreach (ModelPtr child, data->children()) {
        result += renderElement(child);
    }
    return result;
}

QString ContentView::wrapHTML(const QString &body) const
{
    return QString() +
            "<html><head>"
            "<style type=\"text/css\">"
            "kbd {"
            "   background-color: lightgray;"
            "}"
            "</style></head>"
            "<body>\n" + body +"\n</body></html>";
}

QString ContentView::render(ModelPtr data) const
{
    if (data->modelType() == DocBookModel::Set ||
            data->modelType() == DocBookModel::Book)
    {
        return renderTOC(data);
    }
    else {
        return renderPlainPage(data);
    }
}

bool ContentView::isPlainPage(ModelPtr data) const
{
    quint32 sectionedChilds = 0;
    foreach (ModelPtr child, data->children()) {
        if (child->isSectioningNode())
            sectionedChilds += 1;
    }
    return sectionedChilds == 0;
}

QString ContentView::renderPlainPage(ModelPtr data) const
{
    return renderElement(data);
}

QString ContentView::renderChilds(ModelPtr data) const
{
    QString result;
    foreach (ModelPtr child, data->children()) {
        result += renderElement(child) + "\n";
    }
    return result;
}

QString ContentView::renderElement(ModelPtr data) const
{
    if (data == DocBookModel::Text) {
        return renderText(data);
    }
    else if (data == DocBookModel::Para) {
        return renderParagraph(data);
    }
    else if (data == DocBookModel::Emphasis) {
        return renderEmphasis(data);
    }
    else if (data == DocBookModel::ListItem) {
        return renderListItem(data);
    }
    else if (data == DocBookModel::OrderedList) {
        return renderOrderedList(data);
    }
    else if (data == DocBookModel::ItemizedList) {
        return renderItemizedList(data);
    }
    else if (data == DocBookModel::Chapter) {
        return renderChapter(data);
    }
    else if (data == DocBookModel::Section) {
        return renderSection(data);
    }
    else if (data == DocBookModel::Example) {
        return renderExample(data);
    }
    else if (data == DocBookModel::ProgramListing) {
        return renderProgramListing(data);
    }
    else if (data == DocBookModel::Code) {
        return renderCode(data);
    }
    else if (data == DocBookModel::Xref) {
        return renderXref(data);
    }
    else if (data == DocBookModel::KeyCombo) {
        return renderKeyCombo(data);
    }
    else if (data == DocBookModel::KeySym) {
        return renderKeySym(data);
    }
    else if (data == DocBookModel::InformalTable) {
        return renderInformalTable(data);
    }
    else if (data == DocBookModel::Table) {
        return renderTable(data);
    }
    else if (data == DocBookModel::THead) {
        return renderTHead(data);
    }
    else if (data == DocBookModel::TBody) {
        return renderTBody(data);
    }
    else if (data == DocBookModel::Row) {
        return renderRow(data);
    }
    else if (data == DocBookModel::Entry) {
        return renderEntry(data);
    }
    else if (data == DocBookModel::InlineMediaObject) {
        return renderInlineMediaObject(data);
    }
    else if (data == DocBookModel::ImageObject) {
        return renderImageObject(data);
    }
    else if (data == DocBookModel::ListOfExamples) {
        return renderListOfExamples(data);
    }
    else if (data == DocBookModel::ListOfTables) {
        return renderListOfTables(data);
    }
    else {
        return "";
    }
}

QString ContentView::renderKeyCombo(ModelPtr data) const
{
    QString result;
    for (int i=0; i<data->children().size(); i++) {
        if (i>0) {
            result += "&nbsp;";
        }
        result += renderElement(data->children().at(i));
    }
    return result;
}

QString ContentView::renderKeySym(ModelPtr data) const
{
    QString result;
    QString keysym;
    foreach (ModelPtr  child, data->children()) {
        keysym += child->text();
    }
    const QStringList keys = keysym.split("+", QString::SkipEmptyParts);
    QStringList parts;
    foreach (const QString & key, keys) {
        QString part = "<kbd>";
        part += key.toUpper();
        part += "</kbd>";
        parts.push_back(part);
    }
    result = parts.join("+");
    return result;
}

QString ContentView::programTextForLanguage(const QString &source,
                                            const QString &language)
{
    QStringList keywordsList;
    QString inlineCommentSymbol;
    QString multilineCommentStartSymbol;
    QString multilineCommentEndSymbol;

    if (language.toLower() == "kumir") {
        keywordsList = QString::fromUtf8("алг,нач,кон,нц,кц,кц_при,если,"
                                         "то,иначе,все,выбор,при,утв,"
                                         "дано,надо,ввод,вывод,пауза,"
                                         "использовать,исп,кон_исп,"
                                         "цел,вещ,лит,сим,лог,таб,"
                                         "целтаб,вещтаб,"
                                         "литтаб,симтаб,логтаб,"
                                         "арг,рез,аргрез,пока,для,от,до,знач,"
                                         "да,нет,не,и,или,раз,нс,файл,"
                                         "выход,шаг"
                                         ).split(",");
        inlineCommentSymbol = "|";
    }
    else if (language.toLower() == "pascal") {
        keywordsList = QString::fromAscii("begin,end,program,unit,uses,for,from,"
                                         "to,if,then,else,"
                                         "integer,real,string,char,boolean,"
                                         "array,of"
                                         ).split(",");
        inlineCommentSymbol = "//";
        multilineCommentStartSymbol = "{";
        multilineCommentEndSymbol = "}";
    }
    return formatProgramSourceText(
                source.trimmed(),
                keywordsList,
                inlineCommentSymbol,
                multilineCommentStartSymbol,
                multilineCommentEndSymbol
                ).trimmed();
}

QString ContentView::renderProgramListing(ModelPtr data) const
{
    QString result = "<pre align='left'><font face='monospace'>";
    const QString programText = renderChilds(data);
    result += programTextForLanguage(programText, data->role());
    result += "</font></pre>\n";
    return result;
}

QString ContentView::renderCode(ModelPtr data) const
{
    QString result = "<font face='monospace'>";
    const QString programText = renderChilds(data);
    result += programTextForLanguage(programText, data->role());
    result += "</font>";
    return result;
}

QString ContentView::renderTableContent(ModelPtr data) const
{
    QString result;
    result += "<table border='1' bordercolor='black' cellspacing='0' cellpadding='0' width='100%'>\n";
    result += "<tr><td>\n";
    result += "<table border='0' cellspacing='0' cellpadding='10' width='100%'>\n";
    result += renderChilds(data);
    result += "</table>\n";
    result += "</td></tr>\n";
    result += "</table>\n";
    return result;
}

QString ContentView::renderTHead(ModelPtr data) const
{
    QString result;
    result += "<thead>\n";
    result += renderChilds(data);
    result += "</thead>\n";
    return result;
}

QString ContentView::renderTBody(ModelPtr data) const
{
    QString result;
    result += "<tbody>\n";
    result += renderChilds(data);
    result += "</tbody>\n";
    return result;
}

QString ContentView::renderRow(ModelPtr data) const
{
    ModelPtr parent = data->parent();
    bool inTableHead = false;
    bool inTableBody = false;
    while (parent) {
        if (parent->modelType()==DocBookModel::THead) {
            inTableHead = true;
            break;
        }
        if (parent->modelType()==DocBookModel::TBody) {
            inTableBody = true;
            break;
        }
        parent = parent->parent();
    }
    QString result;
    if (inTableHead) {
        result += "<tr valign='center' bgcolor='lightgray'>\n";
    }
    else {
        result += "<tr valign='center'>\n";
    }
    result += renderChilds(data);
    result += "</tr>\n";
    return result;
}

QString ContentView::renderEntry(ModelPtr data) const
{
    ModelPtr parent = data->parent();
    bool inTableHead = false;
    bool inTableBody = false;
    while (parent) {
        if (parent->modelType()==DocBookModel::THead) {
            inTableHead = true;
            break;
        }
        if (parent->modelType()==DocBookModel::TBody) {
            inTableBody = true;
            break;
        }
        parent = parent->parent();
    }
    QString result;
    result += "<td align='center' valign='center'>\n";
    if (inTableHead) {
        result += "<b>";
    }
    result += renderChilds(data);
    if (inTableHead) {
        result += "</b>";
    }
    result += "</td>\n";
    return result;
}


QString ContentView::renderTable(ModelPtr data) const
{
    QString result;
    const QString & title = data->title();

    const QString index = chapterNumber(data) > 0
            ? QString("%1.%2")
              .arg(chapterNumber(data))
              .arg(elementNumber(data))
            : QString::number(elementNumber(data));

    if (loadedModel_ == DocBookModel::ListOfTables) {
        result += "<a name='" + modelToLink(data) + "'>\n";
        result += "<h2>" +
                tr("Table&nbsp;%1. ").arg(index) +
                normalizeText(title) +
                "</h2>\n";
        result += renderItemContextLink(data);
        result += "<br/>";
        result += renderTableContent(data);
        result += "</a>";
    }
    else {
        result += "<a name='" + modelToLink(data) + "'>\n";
        result += "<table width='100%'>\n";
        result += "<tr><td height='10'>&nbsp;</td></tr>\n";
        result += "<tr><td align='left'><b>";
        result += tr("Table&nbsp;%1. ").arg(index);
        result += "</b>" + title + "</td></tr>\n";
        result += "<tr><td>\n";
        result += renderTableContent(data);
        result += "</td></tr>\n";
        result += "<tr><td height='10'>&nbsp;</td></tr>\n";
        result += "</table>\n";
    }
    return result;
}

QString ContentView::renderInformalTable(ModelPtr data) const
{
    QString result;
    result += renderTableContent(data);
    return result;
}

quint16 ContentView::elementNumber(ModelPtr data)
{
    ModelPtr root = data->parent();
    while (root) {
        if (root == DocBookModel::Chapter ||
                root == DocBookModel::Book ||
                root == DocBookModel::Article)
        {
            break;
        }
        root = root->parent();
    }
    bool stop = false;
    return countOfElements(root, data, stop) + 1;
}

quint16 ContentView::countOfElements(ModelPtr root, ModelPtr until, bool &stop)
{
    quint16 result = 0;
    DocBookModel::ModelType rt = root->modelType();
    DocBookModel::ModelType ut = until->modelType();
    quint8 rl = root->sectionLevel();
    quint8 ul = until->sectionLevel();
    bool match = rt == ut;
    if (match && rt == DocBookModel::Section) {
        match = rl == ul;
    }
    if (root == until) {
        stop = true;
    }
    else if (match) {
        result = 1;
    }
    else {
        for (int i=0; i<root->children().size(); i++) {
            const ModelPtr & child = root->children()[i];
            result += countOfElements(child, until, stop);
            if (stop) {
                break;
            }
        }
    }
    return result;
}

quint16 ContentView::chapterNumber(ModelPtr data)
{
    ModelPtr topLevelSection;
    quint16 chapterNumber = 0;
    ModelPtr topLevelBook;

    topLevelSection = data->parent();
    while (topLevelSection) {
        if (topLevelSection->modelType() == DocBookModel::Chapter) {
            topLevelBook = topLevelSection->parent();
            if (topLevelBook) {
                for (int i = 0; i < topLevelBook->children().size(); i++) {
                    const ModelPtr & child = topLevelBook->children()[i];
                    if (child->modelType() == DocBookModel::Chapter) {
                        chapterNumber ++;
                        if (child == topLevelSection) {
                            break;
                        }
                    }
                }
            }
            break;
        }
        topLevelSection = topLevelSection->parent();
    }
    return chapterNumber;
}

QString ContentView::renderItemContextLink(ModelPtr data) const
{
    QString result;
    ModelPtr context = data->parent();
    while (context) {
        if (context == DocBookModel::Section ||
                context == DocBookModel::Chapter ||
                context == DocBookModel::Article ||
                context == DocBookModel::Book)
        {
            break;
        }
        context = context->parent();
    }
    if (context) {
        const QString & contextTitle = context->title();
        const QString contextLink = "model_ptr:" + modelToLink(context);
        result += "<p><b>" + tr("Context:") + "</b> ";
        result += "<a href='" + contextLink + "'>" +
                contextTitle + "</a></p>";
    }
    return result;
}

QString ContentView::renderExample(ModelPtr data) const
{
    QString result;
    const QString & title = data->title();

    const QString index = chapterNumber(data) > 0
            ? QString("%1.%2")
              .arg(chapterNumber(data))
              .arg(elementNumber(data))
            : QString::number(elementNumber(data));

    if (loadedModel_ == DocBookModel::ListOfExamples) {
        result += "<a name='" + modelToLink(data) + "'>\n";
        result += "<h2>" +
                tr("Example&nbsp;%1. ").arg(index) +
                normalizeText(title) +
                "</h2>\n";
        result += renderItemContextLink(data);
        result += renderChilds(data);
        result += "</a>";
    }
    else {
        result += "<a name='" + modelToLink(data) + "'>\n";
        result += "<table width='100%'>\n";
        result += "<tr><td height='10'>&nbsp;</td></tr>\n";
        result += "<tr><td align='center'>\n";
        result += "<table border='1' bordercolor='gray' cellspacing='0' cellpadding='10' width='100%'>";
        result += "<tr><td>\n";
        result += renderChilds(data);
        result += "</td></tr></table>\n";
        result += "</td></tr>\n";
        result += "<tr><td align='center'>\n";
        result += "<b>" + tr("Example&nbsp;%1. ").arg(index) + "</b>";
        result += normalizeText(title);
        result += "</p></td></tr>\n";
        result += "</table></a>\n";
    }
    return result;
}

QString ContentView::renderEmphasis(ModelPtr data) const
{
    const QString tag = data->role()=="bold" ? "b" : "i";
    QString result = "<" + tag + ">";
    result += renderChilds(data);
    result += "</" + tag + ">";
    return result;
}

QString ContentView::renderItemizedList(ModelPtr data) const
{
    QString result = "<ul>\n";
    result += renderChilds(data);
    result += "</ul>";
    return result;
}


QString ContentView::renderOrderedList(ModelPtr data) const
{
    QString result = "<ol>\n";
    result += renderChilds(data);
    result += "</ol>";
    return result;
}

QString ContentView::renderListItem(ModelPtr data) const
{
    QString result = "<li>\n";
    result += renderChilds(data);
    result += "</li>";
    return result;
}

QString ContentView::renderParagraph(ModelPtr data) const
{
    QString result = "<p>";
    result += renderChilds(data);
    result += "</p>";
    return result;
}

QString ContentView::renderInlineMediaObject(ModelPtr data) const
{
    QString result;
    ModelPtr mediaObject = findImageData(data);
    if (mediaObject) {
        result += renderElement(mediaObject);
    }
    return result;
}

QString ContentView::renderImageObject(ModelPtr data) const
{
    QString result;
    ModelPtr imageData;
    foreach (ModelPtr child, data->children()) {
        if (child->modelType() == DocBookModel::ImageData) {
            imageData = child;
            break;
        }
    }
    result += "<img src='model_ptr:"+modelToLink(imageData)+"'>";
    return result;
}

QString ContentView::renderListOfExamples(ModelPtr data) const
{
    QString result;
    result += renderTOC(data);
    result += renderChilds(data);
    return result;
}

QString ContentView::renderListOfTables(ModelPtr data) const
{
    QString result;
    result += renderTOC(data);
    result += renderChilds(data);
    return result;
}

QVariant ContentView::loadResource(int type, const QUrl &name)
{
    QVariant result;
    bool ignore = true;
    if (type == QTextDocument::ImageResource) {
        const QString link = name.toString();
        if (link.startsWith("model_ptr:")) {
            ignore = false;
            QByteArray linkPtr = QByteArray::fromHex(link.toAscii().mid(10));
            QDataStream ds(linkPtr);
            quintptr rawPointer = 0;
            ds >> rawPointer;
            if (rawPointer) {
                DocBookModel * model =
                        reinterpret_cast<DocBookModel*>(rawPointer);
                if (model->modelType() == DocBookModel::ImageData) {
                    const QImage & image = model->imageData();
                    result = image;
                }
            }
        }
    }
    if (ignore) {
        return QTextBrowser::loadResource(type, name);
    }
    else {
        return result;
    }
}

QString ContentView::normalizeText(QString textData) const
{
    static QMap<QString,QString> replacements;
    if (replacements.empty()) {
        replacements["--"] = "&ndash;";
        replacements["---"] = "&mdash;";
    }
    foreach (const QString & key, replacements.keys())
    {
        textData.replace(key, replacements[key]);
    }
    bool inQuote = false;
    for (int i=0; i<textData.length(); i++) {
        if (textData[i] == '"') {
            if (!inQuote) {
                textData.replace(i, 1, "&lsaquo;&lsaquo;");
                inQuote = true;
            }
            else {
                textData.replace(i, 1, "&rsaquo;&rsaquo;");
                inQuote = false;
            }
        }
    }
    return textData;
}

QString ContentView::renderText(ModelPtr data) const
{
    ModelPtr parent = data->parent();
    bool isPreformat = false;
    while (parent) {
        if (parent->modelType() == DocBookModel::ProgramListing
                ||
                parent->modelType() == DocBookModel::Code
                ) {
            isPreformat = true;
            break;
        }
        parent = parent->parent();
    }
    return isPreformat? data->text() : normalizeText(data->text());
}

QString ContentView::renderSection(ModelPtr data) const
{
    const qint8 thisSectionLevel =
            data->sectionLevel() -
            onePageParentModel(data)->sectionLevel();
    const QString tag = QString::fromAscii("h%1").arg(thisSectionLevel + 1);
    const QString anchor = modelToLink(data);
    QString result = "<a name='" + anchor + "'><" + tag + +" class=\"title\">" +
            normalizeText(data->title()) +
            "</" + tag + "></a>\n";
    if (data->subtitle().length() > 0) {
        result += "<" + tag + " class=\"subtitle\">" +
                normalizeText(data->subtitle()) +
                "</"  + tag + ">\n";
    }
    result += renderChilds(data);
    return result;
}

QString ContentView::modelToLink(ModelPtr data) const
{
    const quintptr ptr = quintptr(data.toWeakRef().data());
    QByteArray buffer;
    QDataStream ds(&buffer, QIODevice::WriteOnly);
    ds << ptr;
    return QString::fromAscii(buffer.toHex());
}

QString ContentView::renderXref(ModelPtr data) const
{
    QString result;
    const QString & linkEnd = data->xrefLinkEnd();
    const QString & endTerm = data->xrefEndTerm();
    ModelPtr target = findModelById(topLevelModel(data), linkEnd);
    ModelPtr term;
    if (endTerm.length() > 0) {
        term = findModelById(topLevelModel(data), linkEnd);
    }
    if (target) {
        QString href;
        if (hasModelOnThisPage(target)) {
            href = "#" + linkEnd;
        }
        else {
            ModelPtr container = findModelById(
                        topLevelModel(data), linkEnd
                        );
            if (container) {
                href = QString::fromAscii("model_ptr:") +
                        modelToLink(container);
            }
        }
        const QString targetTitle = normalizeText(
                    term ? term->title() : target->title()
                    );
        if (href.length() > 0) {
            result += tr("(see&nbsp;<a href=\"%1\">%2</a>)")
                    .arg(href)
                    .arg(targetTitle);
        }
    }
    return result;
}

ModelPtr ContentView::findModelById(
        ModelPtr top,
        const QString & modelId
        ) const
{
    if (!top) {
        return ModelPtr();
    }
    else if (top->id() == modelId) {
        return top;
    }
    else {
        foreach (ModelPtr child, top->children()) {
            ModelPtr result = findModelById(child, modelId);
            if (result) {
                return result;
            }
        }
    }
    return ModelPtr();
}

ModelPtr ContentView::topLevelModel(ModelPtr data) const
{
    if (data->parent().isNull()) {
        return data;
    }
    else {
        return topLevelModel(data->parent());
    }
}

ModelPtr ContentView::onePageParentModel(ModelPtr data) const
{
    if (data->parent().isNull() || data->modelType() == DocBookModel::Chapter ||
            data->modelType() == DocBookModel::Article ||
            data->modelType() == DocBookModel::ListOfExamples ||
            data->modelType() == DocBookModel::ListOfTables ||
            data->modelType() == DocBookModel::Book)
    {
        return data;
    }
    else if (data->indexParent()) {
        return data->indexParent();
    }
    else if (data->parent()->modelType() == DocBookModel::Chapter ||
             data->parent()->modelType() == DocBookModel::Article ||
             data->parent()->modelType() == DocBookModel::Book)
    {
        return data->parent();
    }
    else {
        return onePageParentModel(data->parent());
    }
}

bool ContentView::hasModelOnThisPage(ModelPtr data) const
{
    return loadedModel_ && hasChild(loadedModel_, data);
}

bool ContentView::hasChild(ModelPtr who, ModelPtr childToFind) const
{
    foreach (ModelPtr child, who->children()) {
        if (childToFind == child || hasChild(child, childToFind)) {
            return true;
        }
    }
    return false;
}

ModelPtr ContentView::findImageData(ModelPtr parent) const
{
    ModelPtr svgChild;
    ModelPtr pngChild;
    foreach (ModelPtr child, parent->children()) {
        if (child->modelType()==DocBookModel::ImageObject) {
            foreach (ModelPtr childChild, child->children()) {
                if (childChild->modelType()==DocBookModel::ImageData) {
                    if (childChild->format() == "svg") {
                        svgChild = child;
                    }
                    else if (childChild->format() == "png") {
                        pngChild = child;
                    }
                }
            }
        }
    }
    return pngChild ? pngChild : svgChild;
}


QString ContentView::renderTOC(ModelPtr data) const
{
    QString result;
    QString title;
    if (data == DocBookModel::ListOfExamples) {
        title = tr("List of examples in \"%1\"").arg(data->title());
    }
    else if (data == DocBookModel::ListOfTables) {
        title = tr("List of tables in \"%1\"").arg(data->title());
    }
    else {
        title = data->title();
    }
    result += "<h1 class=\"title\">" + normalizeText(title) + "</h1>\n";
    if (data->subtitle().length() > 0) {
        result += "<h1 class=\"subtitle\">" +
                normalizeText(data->subtitle()) +
                "</h1>\n";
    }
    result += "<hr/>\n";
    result += "<ol>\n";
    foreach (ModelPtr child, data->children()) {
        result += renderTOCElement(child);
    }
    result += "</ol>\n";
    result += "<hr/>\n";
    return result;
}

QString ContentView::renderTOCElement(ModelPtr data) const
{
    const quintptr dataPtr = quintptr(data.toWeakRef().data());
    QByteArray buffer;
    QDataStream ds(&buffer, QIODevice::WriteOnly);
    ds << dataPtr;
    const QString href = QString::fromAscii("model_ptr:") +
            QString::fromAscii(buffer.toHex());

    QString result = "\n<li>";
    result += "<p><a href=\"" + href + "\">" + data->title() + "</p>";
    if (!isPlainPage(data)) {
        result += "\n<ol>\n";
        foreach (ModelPtr child, data->children()) {
            result += renderTOCElement(child);
        }
        result += "\n</ol>\n";
    }
    return result + "</li>\n";
}

void ContentView::handleInternalLink(const QUrl &url)
{
    if (url.encodedPath().startsWith("model_ptr:")) {
        const QByteArray path = url.encodedPath().mid(10);
        QByteArray data = QByteArray::fromHex(path);
        QDataStream ds(&data, QIODevice::ReadOnly);
        quintptr ptr = 0u;
        ds >> ptr;
        emit itemRequest(findModelByRawPtr(ptr));
    }
}

ModelPtr ContentView::findModelByRawPtr(quintptr raw) const
{
    ModelPtr result;
    ModelPtr topLevel = topLevelModel(loadedModel_);
    if (topLevel) {
        result = findModelByRawPtr(topLevel, raw);
    }
    if (!result) {
        foreach (ModelPtr child, topLevel->children()) {
            ModelPtr newTopLevel = topLevelModel(child);
            result = findModelByRawPtr(newTopLevel, raw);
            if (result) {
                break;
            }
        }
    }
    return result;
}

ModelPtr ContentView::findModelByRawPtr(ModelPtr root, quintptr raw) const
{
    ModelPtr result;
    if (root.data() == reinterpret_cast<DocBookModel*>(raw)) {
        result = root;
    }
    else foreach (ModelPtr child, root->children()) {
        result = findModelByRawPtr(child, raw);
        if (result)
            break;
    }
    return result;
}

static QString screenRegexSymbols(QString s)
{
    s.replace("|", "\\|");
    s.replace("*", "\\*");
    s.replace("+", "\\+");
    s.replace("{", "\\{");
    s.replace("}", "\\}");
    s.replace("[", "\\[");
    s.replace("]", "\\]");
    return s;
}

QString ContentView::formatProgramSourceText(
        const QString &source,
        const QStringList &keywords,
        const QString &inlineCommentSymbol,
        const QString &multilineCommentStartSymbol,
        const QString &multilineCommentEndSymbol)
{
    QStringList kwds;
    QString result;
    if (keywords.isEmpty()) {
        return source;
    }
    static const QString kwdOpenTag = "<b>";
    static const QString kwdCloseTag = "</b>";
    static const QString beforeCommentTag = "<font color='gray'>";
    static const QString afterCommentTag = "</font>";
    static const QString commentOpenTag = "<i>";
    static const QString commentCloseTag = "</i>";

    foreach (const QString & keyword, keywords) {
        kwds << "\\b" + keyword + "\\b";
    }
    if (inlineCommentSymbol.length() > 0) {
        kwds << screenRegexSymbols(inlineCommentSymbol);
        kwds << "\n";
    }
    if (multilineCommentStartSymbol.length() > 0
            && multilineCommentEndSymbol.length() > 0)
    {
        kwds << screenRegexSymbols(multilineCommentStartSymbol);
        kwds << screenRegexSymbols(multilineCommentEndSymbol);
    }
    QRegExp rxLexer(kwds.join("|"));
    rxLexer.setMinimal(true);
    bool inlineComment = false;
    bool multilineComment = false;
    for (int p = 0, c = 0; ;  ) {
        c = rxLexer.indexIn(source, p);
        if (c == -1) {
            result += source.mid(p);
            break;
        }
        else {
            if (c > p) {
                result += source.mid(p, c - p);
            }
            const QString cap = rxLexer.cap();
            if (cap == inlineCommentSymbol) {
                inlineComment = true;
                result += beforeCommentTag;
                result += cap;
                result += commentOpenTag;
            }
            else if (inlineComment && cap=="\n") {
                inlineComment = false;
                result += commentCloseTag;
                result += afterCommentTag;
                result += "\n";
            }
            else if (cap == multilineCommentStartSymbol) {
                multilineComment = true;
                result += beforeCommentTag;
                result += cap;
                result += commentOpenTag;
            }
            else if (multilineComment && cap==multilineCommentEndSymbol) {
                multilineComment = false;
                result += commentCloseTag;
                result += cap;
                result += afterCommentTag;
            }
            else if (keywords.contains(cap)) {
                result += kwdOpenTag + cap + kwdCloseTag;
            }
            else {
                result += cap;
            }
            p = c + rxLexer.matchedLength();
        }
    }
    if (inlineComment) {
        result += commentCloseTag;
        result += afterCommentTag;
    }

    return result;
}

}