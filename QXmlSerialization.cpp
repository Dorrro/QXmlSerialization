#include "QXmlSerialization.h"

QDomDocument QXmlSerialization::Serialize(QObject *objectToSerialize)
{
    QVariantMap map = QObjectHelper::QObject2QVariantMap(objectToSerialize);

    QDomDocument doc;

    //adding header element
    QDomProcessingInstruction header = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(header);

    //filling root tag element
    QDomElement root = QXmlSerialization::CreateDomElement(
                objectToSerialize->metaObject()->className(),
                map,
                doc);

    doc.appendChild(root);

    return doc;
}

QDomElement QXmlSerialization::CreateDomElement(const QString &name, const QVariantMap &map, QDomDocument &document)
{
    QDomElement element = document.createElement(name);

    for(int mapElementIndex = 0; mapElementIndex < map.count(); mapElementIndex++)
    {
        QDomElement tag;
        QVariant variant = map.values().at(mapElementIndex);

        if(variant.type() == QVariant::Map)
        {
            //if QVariant is QMap we can assume that it is a QVariantMap
            QDomDocument temp;
            QVariantMap variantMap = qvariant_cast<QVariantMap>(variant);
            tag = CreateDomElement(map.keys().at(mapElementIndex), variantMap, temp);
            element.appendChild(tag);
        }
        else if(variant.type() == QVariant::List)
        {
            QDomDocument temp;
            QVariantList variantList = qvariant_cast<QVariantList>(variant);
            tag = CreateDomElement(map.keys().at(mapElementIndex), variantList, temp);
            element.appendChild(tag);
        }
        else if(variant.toString() != "")
        {
            //any other option is treated as a simple string
            tag = document.createElement(map.keys().at(mapElementIndex));
            element.appendChild(tag);
            QDomText text = document.createTextNode(variant.toString());
            tag.appendChild(text);
        }
    }

    return element;
}

QDomElement QXmlSerialization::CreateDomElement(const QString &name, const QVariantList &list, QDomDocument &document)
{
    QDomElement element = document.createElement(name);
    foreach(QVariant variant, list)
    {
        QDomElement tag;
        switch(variant.type())
        {
            case QVariant::String:
            {
                tag = document.createElement("string");
                element.appendChild(tag);
                QDomText text = document.createTextNode(variant.toString());
                tag.appendChild(text);
            }
            break;
            case QVariant::Map:
            {
                QDomDocument temp;
                QVariantMap variantMap = qvariant_cast<QVariantMap>(variant);
                tag = CreateDomElement("Hash", variantMap, temp);
                element.appendChild(tag);
            }
            break;
            case QVariant::List:
            {
                QDomDocument temp;
                QVariantList variantList = qvariant_cast<QVariantList>(variant);
                tag = CreateDomElement("Array", variantList, temp);
                element.appendChild(tag);
            }
            break;
            case QVariant::UserType:
            {
                QDomDocument temp;
                QObject* obj = qvariant_cast<QObject*>(variant);
                QVariantMap variantMap = QObjectHelper::QObject2QVariantMap(obj);
                tag = CreateDomElement(obj->metaObject()->className(), variantMap, temp);
                element.appendChild(tag);
            }
            break;
            //TODO: QVariant::Hash -> convert QHash to QMap - it's that simple
        }
    }
    return element;
}

void QXmlSerialization::Deserialize(const QDomDocument &doc, QObject *targetObject)
{
    QVariantMap map = QXmlSerialization::CreateVariantMap(doc.documentElement().childNodes());

    QObjectHelper::QVariantMap2QObject(map, targetObject);
}

QVariantMap QXmlSerialization::CreateVariantMap(const QDomNodeList &childNodes)
{
    QDomNodeList rootChildNodes = childNodes;
    QVariantMap map;
    for(int childIndex = 0; childIndex < rootChildNodes.count(); childIndex++)
    {
        QDomElement childElement = rootChildNodes.at(childIndex).toElement();
        QString innerChildNodeName = rootChildNodes.at(childIndex).childNodes().at(0).toElement().tagName();

        if(innerChildNodeName == "")
        {
            map[childElement.tagName()] = childElement.text();
        }
        else
        {
            map[childElement.tagName()] = CreateVariantMap(rootChildNodes.at(childIndex).childNodes());
        }
    }

    return map;
}
