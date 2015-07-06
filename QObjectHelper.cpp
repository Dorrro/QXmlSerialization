#include "QObjectHelper.h"

#include "QObjectHelper.h"

QVariantHash QObjectHelper::QObject2QVariantHash(const QObject *object, const QStringList &ignoredProperties)
{
    QVariantHash result;

    const QMetaObject *metaobject = object->metaObject();
    for (int i=0; i< metaobject->propertyCount(); ++i)
    {
        QMetaProperty metaproperty = metaobject->property(i);
        const char *name = metaproperty.name();

        if (ignoredProperties.contains(QLatin1String(name)) || (!metaproperty.isReadable()))
          continue;

        QVariant value = object->property(name);

        if(value.type() == QVariant::UserType)
        {
            QObject * obj = qvariant_cast<QObject *>(value);
            result[name] = QObject2QVariantHash(obj);
        }
        else
        {
            result[name] = value;
        }
    }

    return result;
}

void QObjectHelper::QVariantHash2QObject(const QVariantHash &hash, QObject *object)
{
    const QMetaObject *metaobject = object->metaObject();

    for(int hashElementIndex = 0; hashElementIndex < hash.count(); hashElementIndex++)
    {
        int propertyIndex = metaobject->indexOfProperty(hash.keys().at(hashElementIndex).toUtf8());
        if (propertyIndex < 0)
        {
            continue;
        }

        QMetaProperty metaproperty = metaobject->property(propertyIndex);
        QVariant::Type type = metaproperty.type();
        QVariant variant(hash.values().at(hashElementIndex));
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
            //let's assume that inner object is QVariantHash - as expected
            QVariantHash innerHash = qvariant_cast<QVariantHash>(variant);

            //create instance of an object based on it's registered type
            QObject* innerObject = QMetaType::metaObjectForType(metaproperty.userType())->newInstance();

            //recursively fill the object properties
            QVariantHash2QObject(innerHash, innerObject);

            //assign object to property
            QVariant var;
            var.setValue(innerObject);
            metaproperty.write(object, var);
        }
    }
}

