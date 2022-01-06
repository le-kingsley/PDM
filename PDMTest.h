#pragma once

#include "stdafx.h"
#include <QtWidgets/QMainWindow>
#include "ui_PDMTest.h"
#include "HttpOper.h"
#include "JsonTransform.h"
#include "PDMGlobal.h"
#include <QComboBox>
#include <QPushButton>
#include <QMessageBox>
#include "TowerModel.h"

class PDMTest : public QMainWindow
{
    Q_OBJECT

public:
	PDMTest(QWidget *parent = Q_NULLPTR);
	void InitForm();
	void InitTowerModel(QString sId, Tower* tower);
	QJsonArray GetPartList(Tower* tower);
	QJsonArray GetTowerPlaceList(Tower* tower);
	QJsonArray GetBoltRecList(Tower* tower);
	QJsonArray GetTowerPartRefList(Tower* tower);
public slots:
	bool On_PullButton();
	void On_UpLoadButton_Click();
	void On_DownLoadButton_Click();
	void On_UpLoadButton_Click2();
	void On_SearchButton_Click();
private:
    Ui::PDMTestClass ui;
	CHttpOper* oper;
	Global g_Header;
};
