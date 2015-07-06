#ifndef QXMLSERIALIZATION_H
#define QXMLSERIALIZATION_H

#include <QDomDocument>
#include <QHash>

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
     * @brief CreateVariantHash Converts given QDomNodeList to QVariantHash representation
     * @param document Basically, inner child list of a root tag
     * @return Converted \p document in form of QVariantHash
     */
    static QVariantHash CreateVariantHash(const QDomNodeList& document);

    /**
     * @brief CreateDomElement Creates a node element with name \p name in \p document according to \p hash members
     * @param name Name of root element of given \p document
     * @param hash Would be treated as a map for filling the \p document
     * @param document QDomDocument which would be will according to \p hash members
     * @return Root element of a \p document
     */
    static QDomElement CreateDomElement(const QString &name, const QVariantHash &hash, QDomDocument &document);
};

#endif // QXMLSERIALIZATION_H
