#include "QObjectHelper.h"

#include "QObjectHelper.h"

QVariantMap QObjectHelper::QObject2QVariantMap(const QObject *object, const QStringList &ignoredProperties)
{
    QVariantMap result;

    const QMetaObject *metaobject = object->metaObject();
    for (int i=0; i< metaobject->propertyCount(); ++i)
    {
        QMetaProperty metaproperty = metaobject->property(i);
        const char *name = metaproperty.name();

        if (ignoredProperties.contains(QLatin1String(name)) || (!metaproperty.isReadable()))
          continue;

        QVariant value = object->property(name);

        QVariant::Type type = value.type();
        if(type == QVariant::UserType)
        {
            QObject* obj = qvariant_cast<QObject*>(value);

            result[name] = QObject2QVariantMap(obj);
        }
        else
        {
            result[name] = value;
        }
    }

    return result;
}

