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
            default:
            {
                case QVariant::String:
                {
                    QString tagName = variant.typeName();
                    tagName = tagName.replace("Q", "").toLower();
                    tag = document.createElement(tagName);
                    element.appendChild(tag);
                    QDomText text = document.createTextNode(variant.toString());
                    tag.appendChild(text);
                }
            }
            break;
            //TODO: QVariant::Hash -> convert QHash to QMap - it's that simple
        }
    }
    return element;
}

void QXmlSerialization::Deserialize(const QDomDocument &doc, QObject *targetObject)
{
    const QMetaObject *metaobject = targetObject->metaObject();
    QDomNodeList rootChildNodes = doc.documentElement().childNodes();
    FillObjectPropertiesBasedOnXMLNodeList(rootChildNodes, targetObject);
}

void QXmlSerialization::FillObjectPropertiesBasedOnXMLNodeList(const QDomNodeList &list, QObject* object)
{
    const QMetaObject* metaObject = object->metaObject();

    for(int childIndex = 0; childIndex < list.count(); childIndex++)
    {
        QDomNode rootChildNode = list.at(childIndex);
        QDomElement rootChildElement = rootChildNode.toElement();
        QMetaProperty rootProperty = metaObject->property(metaObject->indexOfProperty(rootChildNode.nodeName().toUtf8()));

        QString rootPropertyName = rootProperty.typeName();
        QString content = rootChildElement.text();
        QVariant::Type type = rootProperty.type();
        QVariant result;
        switch (type)
        {
                case QVariant::UserType:
                {
                    //create instance of an object based on it's registered type
                    int listElementType = QMetaType::type(rootPropertyName.toUtf8());
                    #if defined(Q_OS_WIN)
                        QObject* innerObject = QMetaType::metaObjectForType(rootProperty.type())->newInstance();
                    #else
                        QObject* innerObject = QMetaType::metaObjectForType(listElementType)->newInstance();
                    #endif

                    FillObjectPropertiesBasedOnXMLNodeList(rootChildNode.childNodes(), innerObject);
                    result.setValue(innerObject);
                }
                break;
                case QVariant::List:
                {
                    result.setValue(QDomNodeList2QVariantList(rootChildElement.childNodes()));
                }
                break;
                case QVariant::Hash:
                case QVariant::Map:
                qDebug("unhandled type");
                break;
                case QVariant::String:
                default:
                {
                    result.setValue(content);
                }
                break;
        }
        rootProperty.write(object, result);
    }
}

QVariantList QXmlSerialization::QDomNodeList2QVariantList(const QDomNodeList &list)
{
    QVariantList variantList;
    for(int innerChildIndex = 0; innerChildIndex < list.count(); innerChildIndex++)
    {
        QDomNode innerChildNode = list.at(innerChildIndex);
        int listElementType = QMetaType::type(innerChildNode.nodeName().toUtf8().append("*"));
        if(listElementType > 0)
        {
            QObject* innerObject = QMetaType::metaObjectForType(listElementType)->newInstance();

            FillObjectPropertiesBasedOnXMLNodeList(innerChildNode.childNodes(), innerObject);

            QVariant var;
            var.setValue(innerObject);
            int i = var.type();
            var.convert(listElementType);
            variantList << QVariant::fromValue(innerObject);
        }
        else
        {
            variantList << innerChildNode.toElement().text();
        }
    }

    return variantList;
}
