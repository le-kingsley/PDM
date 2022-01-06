#pragma once

#include <QWidget>
#include "ui_Logindialog.h"
#include <QDialog>
#include <QKeyEvent>
#include "PDMTest.h"
#include <QMouseEvent>
#include "JsonTransform.h"
#include <QMessageBox>
#include "Tenantdialog.h"
#include "PDMGlobal.h"

class Logindialog : public QDialog
{
	Q_OBJECT

public:
	Logindialog(QWidget *parent = Q_NULLPTR);
	~Logindialog();
	void InitForm();

private slots:
	void on_btnMenu_Close_clicked();
	void on_btnMenu_Min_clicked();
	void on_btnLogin_clicked();

protected:
	void mouseMoveEvent(QMouseEvent *e);
	void mousePressEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *);
private:
	Ui::Logindialog ui;
	CHttpOper* oper;
	bool m_bMousePressed;
	QPoint m_MousePt;
	QMap<QString, QString> m_HttpHeaderMap;
	Tenantdialog *m_tenantWidget;//租户界面
	PDMTest *m_PdmWidget;		//用户登陆后主界面
};
