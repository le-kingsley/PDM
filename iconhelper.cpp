#include "iconhelper.h"

IconHelper* IconHelper::_instance = 0;
IconHelper::IconHelper(QObject*):
    QObject(qApp)
{
    int fontId = QFontDatabase::addApplicationFont("./image/fontawesome-webfont.ttf");
    QString fontName = "fontawesome-webfont.ttf";
	if (QFontDatabase::applicationFontFamilies(fontId).size() > 0) {
		QStringList fontNameList = QFontDatabase::applicationFontFamilies(fontId);
		fontName = fontNameList[0];
	}
    iconFont = QFont(fontName);
}

void IconHelper::SetIcon(QLabel* lab, QChar c, int size)
{
    iconFont.setPointSize(size);
    lab->setFont(iconFont);
    lab->setText(c);
}

void IconHelper::SetIcon(QPushButton* btn, QChar c, int size)
{
    iconFont.setPointSize(size);
    btn->setFont(iconFont);
    btn->setText(c);
}
