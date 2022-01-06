#include "JsonTransform.h"


QString JsonToString(const QJsonObject& json)
{
	/*QJsonDocument document = QJsonDocument::toJson();
	json*/
	return QString(QJsonDocument(json).toJson(QJsonDocument::Compact));
}

QJsonObject StringToJson(const QString& str)
{
	QJsonObject object;
	QJsonParseError error;
	QJsonDocument document = QJsonDocument::fromJson(str.toUtf8(), &error);
	if (error.error == QJsonParseError::NoError)
	{
		if (document.isObject() && !document.isNull())
		{
			object = document.object();
		}
	}
	return object;
}
