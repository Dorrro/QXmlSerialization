#ifndef QOBJECTHELPER_H
#define QOBJECTHELPER_H

#include <QVariantMap>
#include <QMetaProperty>

class QObjectHelper
{
public:
    /**
     * @brief QObject2QVariantHash A static method taking object and optional list of ignored properties returning QVariantHash value
     * @param object Object to be converted
     * @param ignoredProperties List of ignored properties' name
     * @return Representation of QObject properties in form of QVariantHash
     */
    static QVariantHash QObject2QVariantHash(const QObject* object,
                                                      const QStringList& ignoredProperties = QStringList(QString(QLatin1String("objectName"))));

    /**
     * @brief QVariantHash2QObject A static method taking QVariantHash and pointer to QObject which causes to fill QObject properties to be filled according to QVariantHash members
     * @param hash Representation of QObject properties in form of QVariantHash
     * @param object Object which properties would be filled according to hash members
     */
    static void QVariantHash2QObject(const QVariantHash& hash, QObject *object);
};

#endif // QOBJECTHELPER_H
