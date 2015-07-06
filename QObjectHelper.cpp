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

void QObjectHelper::QVariantMap2QObject(const QVariantMap &map, QObject *object)
{
    const QMetaObject *metaobject = object->metaObject();

    for(int mapElementIndex = 0; mapElementIndex < map.count(); mapElementIndex++)
    {
        int propertyIndex = metaobject->indexOfProperty(map.keys().at(mapElementIndex).toUtf8());
        if (propertyIndex < 0)
        {
            continue;
        }

        QMetaProperty metaproperty = metaobject->property(propertyIndex);
        QVariant::Type type = metaproperty.type();
        QVariant variant(map.values().at(mapElementIndex));
        if (variant.canConvert(type))
        {
            variant.convert(type);
            metaproperty.write(object, variant);
        }
        else if(QString("QVariant").compare(metaproperty.typeName()) == 0)
        {
            metaproperty.write(object, variant);
        }
        else
        {
            //let's assume that inner object is QVariantMap - as expected
            QVariantMap innerMap = qvariant_cast<QVariantMap>(variant);

            //create instance of an object based on it's registered type
            QObject* innerObject = QMetaType::metaObjectForType(metaproperty.userType())->newInstance();

            //recursively fill the object properties
            QVariantMap2QObject(innerMap, innerObject);

            //assign object to property
            QVariant var;
            var.setValue(innerObject);
            metaproperty.write(object, var);
        }
    }
}

