#pragma once

#include <QDialog>
#include "ui_Tenantdialog.h"
#include <QHeaderView>

class Tenantdialog : public QDialog
{
	Q_OBJECT

public:
	Tenantdialog(QWidget *parent = Q_NULLPTR);
	~Tenantdialog();
	void InitForm();
	
private:
	Ui::Tenantdialog ui;
};
