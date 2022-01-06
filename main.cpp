#include <QtWidgets/QApplication>
#include "PDMTest.h"
#include "Logindialog.h"
#include "Tenantdialog.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	Logindialog login;
	login.show();
	/*PDMTest pdm;
	pdm.show();
	Tenantdialog tenantdlg;
	tenantdlg.show();*/
    return a.exec();
}
