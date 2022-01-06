#include "Logindialog.h"
#include "../app/iconhelper.h"

Logindialog::Logindialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	InitForm();
	oper = new CHttpOper();
	oper->m_httpManager = new QNetworkAccessManager();
}

Logindialog::~Logindialog()
{
}

void Logindialog::InitForm()
{
	setWindowFlags(Qt::FramelessWindowHint);
	ui.txtUserName->setFocus();
	ui.lab_Title->installEventFilter(this);
	m_bMousePressed = false;
	IconHelper::Instance()->SetIcon(ui.btnMenu_Close, QChar(0xf00d), 10);
	IconHelper::Instance()->SetIcon(ui.btnMenu_Min, QChar(0xf068), 10);
	IconHelper::Instance()->SetIcon(ui.lab_Ico, QChar(0xf015), 12);
}

void Logindialog::on_btnMenu_Close_clicked()
{
	this->close();
}

void Logindialog::on_btnMenu_Min_clicked()
{
	this->showMinimized();
}

Global g_xGlobal;
void Logindialog::on_btnLogin_clicked()
{
	QString userName = ui.txtUserName->text().trimmed();
	QString userPwd = ui.txtUserPwd->text().trimmed();
	ui.txtUserPwd->setFocus();
	if (!userName.isEmpty() && !userPwd.isEmpty())
	{
		//url
		QString url = "http://tc-dev-gateway.dhwy.cn/tower-vcs/openApi/login";
		QString strUrl = url;
		if (url != NULL && !url.isEmpty()) {
			if (strUrl.indexOf("?") > 0)
			{
				strUrl.append("&userName=" + userName);
				strUrl.append("&password=" + userPwd);
			}
			else
			{
				strUrl.append("?userName=" + userName);
				strUrl.append("&password=" + userPwd);
			}
		}
		//录入用户信息后，打包成json，给服务器发送用户信息
		/*for (int i = 0; i < 2; ++i)
		{*/
			QJsonObject jObject;
			jObject["userName"] = userName;
			jObject["password"] = userPwd;
			//jObject["tenantId"] = "000000";
			//jObject["days"] = QString::fromLocal8Bit(ui.daysEdit->text().toLocal8Bit());
			//post请求的数据是Json格式。首先创建QJsonObject对象，并插入数据。
			QJsonDocument doc;
			doc.setObject(jObject);
			//QByteArray postContent = doc.toJson(QJsonDocument::Compact);
			QString resultData = "";
			oper->post2(strUrl, "", resultData, m_HttpHeaderMap);
			//无返回
			if (resultData == "")
			{
				QMessageBox::information(NULL, "登录失败", "登录失败，请检查用户名和密码", QMessageBox::Cancel);
			}
			//有返回
			else
			{
				QJsonObject enterJsonObj = StringToJson(resultData);
				if (enterJsonObj.isEmpty()) {

				}
				QString sEnterBackData = enterJsonObj.toVariantMap()["code"].toString();
				QJsonObject loginDataObj = enterJsonObj.toVariantMap()["data"].toJsonObject();
				//全局授权码
				g_xGlobal.sAuthorization = loginDataObj.toVariantMap()["authorization"].toString();
				//全局token
				g_xGlobal.sSinzetechAuth = loginDataObj.toVariantMap()["token"].toString();
				//全局租户id和名称
				g_xGlobal.sCurrentTenantId = loginDataObj.toVariantMap()["currentTenantId"].toString();
				g_xGlobal.sCurrentTenantName = loginDataObj.toVariantMap()["currentTenantName"].toString();
				QVariantList tenantVos = loginDataObj.toVariantMap()["tenantVos"].toList();
				vector<Tenant> returnTenantVec;
				for (int i = 0; i < tenantVos.size(); ++i)
				{
					Tenant ten;
					ten.tenantId = tenantVos[i].toJsonObject().toVariantMap()["tenantId"].toString();
					ten.tenantName = tenantVos[i].toJsonObject().toVariantMap()["tenantName"].toString();
					returnTenantVec.push_back(ten);
				}
				g_xGlobal.xTenantVector = returnTenantVec;
				//状态码
				if (sEnterBackData == "10000")
				{
					//租户数据进行提取保存至全局设置内
					//租户界面
					/*m_tenantWidget = new Tenantdialog(this);
					m_tenantWidget->show();*/

				}
				if (sEnterBackData == "200")
				{
					m_PdmWidget = new PDMTest();
					m_PdmWidget->show();
					this->close();
				}
			}
		//}
	}
	else
	{
		QMessageBox::information(NULL, "登录失败", "登录失败，请检查用户名和密码", QMessageBox::Cancel);
	}
}

void Logindialog::mouseMoveEvent(QMouseEvent *e)
{
	if (m_bMousePressed && (e->buttons()) && Qt::LeftButton)
	{
		move(e->globalPos() - m_MousePt);
		e->accept();
	}
}

void Logindialog::mousePressEvent(QMouseEvent *e)
{
	if (e->button() == Qt::LeftButton)
	{
		m_bMousePressed = true;
		m_MousePt = e->globalPos() - pos();
		e->accept();
	}
}

void Logindialog::mouseReleaseEvent(QMouseEvent *)
{
	m_bMousePressed = false;
}