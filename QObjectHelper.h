#ifndef QOBJECTHELPER_H
#define QOBJECTHELPER_H

#include <QVariantMap>
#include <QMetaProperty>

class QObjectHelper
{
public:
    /**
     * @brief QObject2QVariantMap A static method taking object and optional list of ignored properties returning QVariantMap value
     * @param object Object to be converted
     * @param ignoredProperties List of ignored properties' name
     * @return Representation of QObject properties in form of QVariantMap
     */
    static QVariantMap QObject2QVariantMap(const QObject* object,
                                                      const QStringList& ignoredProperties = QStringList(QString(QLatin1String("objectName"))));

    /**
     * @brief QVariantMap2QObject A static method taking QVariantMap and pointer to QObject which causes to fill QObject properties to be filled according to QVariantMap members
     * @param map Representation of QObject properties in form of QVariantMap
     * @param object Object which properties would be filled according to map members
     */
    static void QVariantMap2QObject(const QVariantMap& map, QObject *object);
};

#endif // QOBJECTHELPER_H
