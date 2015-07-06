#include "QXmlSerialization.h"

QDomDocument QXmlSerialization::Serialize(QObject *objectToSerialize)
{
    QVariantHash hash = QObjectHelper::QObject2QVariantHash(objectToSerialize);

    QDomDocument doc;

    //adding header element
    QDomProcessingInstruction header = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(header);

    //filling root tag element
    QDomElement root = QXmlSerialization::CreateDomElement(
                objectToSerialize->metaObject()->className(),
                hash,
                doc);

    doc.appendChild(root);

    return doc;
}

QDomElement QXmlSerialization::CreateDomElement(const QString &name, const QVariantHash &hash, QDomDocument &document)
{
    QDomElement element = document.createElement(name);

    for(int hashElementIndex = 0; hashElementIndex < hash.count(); hashElementIndex++)
    {
        QDomElement tag;
        QVariant variant = hash.values().at(hashElementIndex);

        if(variant.type() == QVariant::Hash)
        {
            //if QVariant is QHash we can assume that it is a QVariantHash
            QDomDocument temp;
            QVariantHash variantHash = qvariant_cast<QVariantHash>(variant);
            tag = CreateDomElement(hash.keys().at(hashElementIndex), variantHash, temp);
            element.appendChild(tag);
        }
        else if(variant.toString() != "")
        {
            //any other option is treated as a simple string
            tag = document.createElement(hash.keys().at(hashElementIndex));
            element.appendChild(tag);
            QDomText text = document.createTextNode(variant.toString());
            tag.appendChild(text);
        }
    }

    return element;
}

void QXmlSerialization::Deserialize(const QDomDocument &doc, QObject *targetObject)
{
    QVariantHash hash = QXmlSerialization::CreateVariantHash(doc.firstChild().toElement().childNodes());

    QObjectHelper::QVariantHash2QObject(hash, targetObject);
}

QVariantHash QXmlSerialization::CreateVariantHash(const QDomNodeList &childNodes)
{
    QDomNodeList rootChildNodes = childNodes;
    QVariantHash hash;
    for(int childIndex = 0; childIndex < rootChildNodes.count(); childIndex++)
    {
        QDomElement childElement = rootChildNodes.at(childIndex).toElement();
        QString innerChildNodeName = rootChildNodes.at(childIndex).childNodes().at(0).toElement().tagName();

        if(innerChildNodeName == "")
        {
            hash[childElement.tagName()] = childElement.text();
        }
        else
        {
            hash[childElement.tagName()] = CreateVariantHash(rootChildNodes.at(childIndex).childNodes());
        }
    }

    return hash;
}
