#pragma once
#include <qobject.h>
#include <QObject>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>

QString JsonToString(const QJsonObject& json);
QJsonObject StringToJson(const QString& str);
