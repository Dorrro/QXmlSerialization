#ifndef QXMLSERIALIZATION_H
#define QXMLSERIALIZATION_H

#include <QDomDocument>
#include <QMap>
#include <QString>

#include "QObjectHelper.h"

class QXmlSerialization
{
public slots:
    /**
     * @brief Serialize A static method taking QObject* as a param and trying to serialize it to QDomDocument
     * @param objectToSerialize Object* to be serialized
     * @return Serialized QObject* in form of QDomDocument
     */
    static QDomDocument Serialize(QObject* objectToSerialize);

    /**
     * @brief Deserialize A static method taking QDomDocument and a QObject* as parameters which causes rewriting XML structure of doc to proper \p targetObject properties
     * @param doc Mapped \p targetObject structure in form of XML
     * @param targetObject Instance of an object which is QObject representation of given \p doc
     */
    static void Deserialize(const QDomDocument& doc, QObject* targetObject);

private slots:
    /**
     * @brief CreateDomElement Creates a node element with name \p name in \p document according to \p map members
     * @param name Name of root element of given \p document
     * @param map Would be treated as a map for filling the \p document
     * @param document QDomDocument which would be will according to the \p map members
     * @return Root element of a \p document
     */
    static QDomElement CreateDomElement(const QString &name, const QVariantMap &map, QDomDocument &document);

    /**
     * @brief CreateDomElement Creates a node element with name \p name in \p document according to \p list members
     * @param name Name of root element of given \p document
     * @param list Would be treated as a map for filling the \p document
     * @param document QDomDocument which would be will according to the \p list members
     * @return Root element of a \p document
     */
    static QDomElement CreateDomElement(const QString &name, const QVariantList &list, QDomDocument &document);

    /**
     * @brief FillObjectPropertiesBasedOnXMLNodeList Converts given QDomNodeList to QObject representation
     * @param list Basically, inner child list of a root tag
     */
    static void FillObjectPropertiesBasedOnXMLNodeList(const QDomNodeList& list, QObject* object);

    /**
     * @brief QDomNodeList2QVariantList Converts given QDomNodeList to QVariantList representation
     * @param list Basically, inner child list of a root tag
     * @return Converted \p list in form of QVariantList
     */
    static QVariantList QDomNodeList2QVariantList(const QDomNodeList& list);
};

#endif // QXMLSERIALIZATION_H
