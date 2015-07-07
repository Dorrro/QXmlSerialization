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
};

#endif // QOBJECTHELPER_H
