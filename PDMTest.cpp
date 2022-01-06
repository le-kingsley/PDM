#include "PDMTest.h"
#include <QTimer>

PDMTest::PDMTest(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
	//�����Ŀ����
	this->setWindowTitle(tr("PDM����"));
	InitForm();
	ui.tableWidget->setColumnHidden(0, true); // ���ص�һ��
	ui.tableWidget->setColumnHidden(1, true); // ���صڶ���
	ui.tableWidget->setColumnHidden(3, true); // ���ص�����
	//��ǰ����
	QTimer* timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	oper = new CHttpOper();
	oper->m_httpManager = new QNetworkAccessManager();
	//�����ȡ
	connect(ui.pullButton, &QPushButton::clicked, this, [=]() {
		On_PullButton();
	});
	/*connect(ui.pushButton, &QPushButton::clicked, this, [=]() {
		On_UpLoadButton_Click();
	});*/
	connect(ui.pushButton, &QPushButton::clicked, this, [=]() {
		On_UpLoadButton_Click2();
	});
	//������

	//���Ͱ�ť
	//���ذ�ť
}

void PDMTest::InitForm()
{
	ui.tableWidget->clearContents();
	ui.tableWidget->clearSelection();
	ui.tableWidget->setRowCount(20);
	ui.tableWidget->setColumnCount(8);
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	ui.tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	QHeaderView* headerView = ui.tableWidget->verticalHeader();
	headerView->setHidden(true);
}

BaseConfig g_xbaseConfig;
bool PDMTest::On_PullButton()
{
	QString url= "http://tc-dev-gateway.dhwy.cn/tower-data-archive/productCategory/all";
	QString strUrl = url;
	if (url != NULL && !url.isEmpty()) {
		if (strUrl.indexOf("?") > 0)
		{
			strUrl.append("&Sinzetech-Auth=" + g_xGlobal.sSinzetechAuth.toUtf8());
			strUrl.append("&Tenant-Id=" + g_xGlobal.sCurrentTenantId.toUtf8());
			strUrl.append("&Authorization=" + g_xGlobal.sCurrentTenantName.toUtf8());
		}
		else
		{
			strUrl.append("?Sinzetech-Auth=" + g_xGlobal.sSinzetechAuth.toUtf8());
			strUrl.append("&Tenant-Id=" + g_xGlobal.sCurrentTenantId.toUtf8());
			strUrl.append("&Authorization=" + g_xGlobal.sCurrentTenantName.toUtf8());
		}
	}
	QString resultData;
	oper->get(strUrl, resultData);
	//���ݽ��Ϊ�������ƣ��ΰ����ƣ��ļ�����+�汾�ţ���ť
	QJsonObject towerJsonObj = StringToJson(resultData);
	QVariantList towerDataList = towerJsonObj.toVariantMap()["data"].toList();
	int rowIndex = 0;
	for (int i = 0; i < towerDataList.size(); ++i)
	{
		QVariantMap dataMap = towerDataList[i].toJsonObject().toVariantMap();
		QString sWarehouseId = dataMap["id"].toString();
		QString sProductCategoryId = dataMap["productCategoryName"].toString();
		//��ȡ�ΰ��б�
		QVariantList partPackList = dataMap["productInfoList"].toList();
		for (int j = 0; j < partPackList.size(); ++j)
		{
			//��ȡ��������
			QString sTowerTypeName= dataMap["productCategoryName"].toString();
			//��ȡ�ΰ�
			QVariantMap partPackMap = partPackList[j].toJsonObject().toVariantMap();
			//���ݶΰ������
			QString sPackageId = partPackMap["partPackId"].toString();
			QString partPackName = partPackMap["partPackName"].toString();
			//���������
			ui.tableWidget->setItem(rowIndex, 0, new QTableWidgetItem(sWarehouseId));
			ui.tableWidget->setItem(rowIndex, 1, new QTableWidgetItem(sProductCategoryId));
			ui.tableWidget->setItem(rowIndex, 2, new QTableWidgetItem(sTowerTypeName));
			ui.tableWidget->setItem(rowIndex, 3, new QTableWidgetItem(sPackageId));
			ui.tableWidget->setItem(rowIndex, 4, new QTableWidgetItem(partPackName));
			//��������������Ϊ�ļ���+�汾��
			//ÿ���ΰ���һ��������һ���ϴ���ť��һ�����ذ�ť
			QVariantList fileVersionList = partPackMap["fileVersionList"].toList();
			QComboBox* fileVersionBox = new QComboBox();
			for (int v = 0; v < fileVersionList.size(); v++)
			{
				QVariantMap fileVersionMap = fileVersionList[v].toJsonObject().toVariantMap();
				QString fileVersionId = fileVersionMap["fileVersionId"].toString();
				QString fileVersionName = fileVersionMap["fileName"].toString();
				QString fileVersionVersion = fileVersionMap["version"].toString();
				QStringList fileVersionBoxList;
				fileVersionBoxList.append(QString("%1-%2").arg(fileVersionName).arg(fileVersionVersion));
				fileVersionBox->addItems(fileVersionBoxList);
			}
			ui.tableWidget->setCellWidget(j, 5, fileVersionBox);
			//����
			QWidget * w = new QWidget(this);
			QPushButton *btnUpload = new QPushButton(this);
			btnUpload->setText("�ϴ�");
			QHBoxLayout *vlayout = new QHBoxLayout;
			vlayout->setMargin(0);
			vlayout->setSpacing(0);
			vlayout->addWidget(btnUpload);

			QTableWidget *attachTable = new QTableWidget(0, 1);
			attachTable->setObjectName("attachTable");
			//    attachTable->setVisible(false);
			attachTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
			attachTable->horizontalHeader()->hide();
			attachTable->verticalHeader()->hide();
			vlayout->addWidget(attachTable);
			w->setLayout(vlayout);
			ui.tableWidget->setCellWidget(rowIndex, 6, w);
			connect(btnUpload, &QPushButton::clicked, this, [=]() {
				On_UpLoadButton_Click();
			});
			//��������1
			rowIndex++;
		}
	}
	return true;
}

void PDMTest::On_UpLoadButton_Click() {
	int rowIndex = ui.tableWidget->currentRow();
	//
	if (rowIndex == -1)
	{
		QMessageBox::information(NULL, "�ϴ��ļ�ʧ��", "�ϴ��ļ�ʧ�ܣ�����ĳ���ϴ�", QMessageBox::Cancel);
		return;
	}
	QString sWarehouseId = ui.tableWidget->item(rowIndex, 0)->text();
	QString sProductCategoryId = ui.tableWidget->item(rowIndex, 1)->text();
	QString sTowerTypeName = ui.tableWidget->item(rowIndex, 2)->text();
	QString sPackageId = ui.tableWidget->item(rowIndex, 3)->text();
	QString partPackName = ui.tableWidget->item(rowIndex, 4)->text();
	//
	QWidget *widgetVersion = ui.tableWidget->cellWidget(rowIndex, 5);
	QComboBox *fileVersion = (QComboBox*)widgetVersion;
	QString sFileVersion = fileVersion->currentText();
	//�����ļ�ѡ���壬ѡ��tma�ļ�
	QString uploadFilePath = QFileDialog::getOpenFileName(this, tr("file"), "/", tr("tma�ļ�(*.tma)"));
	QFileInfo* fileInfo = new QFileInfo(uploadFilePath);
	QString localFileName = fileInfo->fileName();
	QString localFilePath = fileInfo->path();

	QString url = "http://tc-dev-gateway.dhwy.cn/tower-data-archive/productCategory/bom";
	QString strUrl = url;
	if (url != NULL && !url.isEmpty()) {
		if (strUrl.indexOf("?") > 0)
		{
			strUrl.append("&Sinzetech-Auth=" + g_xGlobal.sSinzetechAuth.toUtf8());
		}
		else
		{
			strUrl.append("?Sinzetech-Auth=" + g_xGlobal.sSinzetechAuth.toUtf8());
		}
	}
	QJsonObject jObject;
	jObject["warehouseId"] = sWarehouseId;
	jObject["productCategoryId"] = sProductCategoryId;
	jObject["segmentPackageId"] = sPackageId;
	jObject["fileName"] = localFileName;
	//post�����������Json��ʽ�����ȴ���QJsonObject���󣬲��������ݡ�
	QJsonDocument doc;
	doc.setObject(jObject);
	QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
	QString resultData;
	oper->post(strUrl, jsonData, resultData);
	//��ȡ�ϴ��ļ�OSS·���ʹ�������
	QJsonObject towerJsonObj = StringToJson(resultData);
	QVariantMap towerData = towerJsonObj.toVariantMap()["data"].toJsonObject().toVariantMap();
	QString sOssPushUrl = towerData["ossPushUrl"].toString();
	QString sOssCallback = towerData["ossCallback"].toString();
	QString sOssCallbackVar = towerData["ossCallbackVar"].toString();
	QString sExpires = towerData["expires"].toString();
	/*QString sOssUrl = sOssPushUrl;
	if (sOssPushUrl != NULL && !sOssPushUrl.isEmpty()) {
		if (sOssUrl.indexOf("?") > 0)
		{
			sOssUrl.append("&ossCallback=" + sOssCallback.toUtf8());
			sOssUrl.append("&ossCallbackVar=" + sOssCallbackVar.toUtf8());
			sOssUrl.append("&expires=" + sExpires.toUtf8());
		}
		else
		{
			sOssUrl.append("?ossCallback=" + sOssCallback.toUtf8());
			sOssUrl.append("&ossCallbackVar=" + sOssCallbackVar.toUtf8());
			sOssUrl.append("&expires=" + sExpires.toUtf8());
		}
	}*/
	//�ϴ��ļ�
	QMap<QString, QString> headerMaps;
	headerMaps["x-oss-callback"] = sOssCallback;
	headerMaps["x-oss-callback-var"] = sOssCallbackVar;
	headerMaps["expires"] = sExpires;
	oper->PutFile(sOssPushUrl, headerMaps,localFileName, localFilePath);
}

void PDMTest::On_DownLoadButton_Click() 
{

}

void PDMTest::On_UpLoadButton_Click2()
{
	int rowIndex = ui.tableWidget->currentRow();
	//
	if (rowIndex == -1)
	{
		QMessageBox::information(NULL, "�ϴ��ļ�ʧ��", "�ϴ��ļ�ʧ�ܣ�����ĳ���ϴ�", QMessageBox::Cancel);
		return;
	}
	QString sProductCategoryId = ui.tableWidget->item(rowIndex, 0)->text();
	QString url = "http://tc-dev-gateway.dhwy.cn/tower-data-archive/productCategory/bom";
	QString strUrl = url;
	if (url != NULL && !url.isEmpty()) {
		if (strUrl.indexOf("?") > 0)
		{
			strUrl.append("&Sinzetech-Auth=" + g_xGlobal.sSinzetechAuth.toUtf8());
			strUrl.append("&Tenant-Id=" + g_xGlobal.sCurrentTenantId.toUtf8());
			strUrl.append("&Authorization=" + g_xGlobal.sCurrentTenantName.toUtf8());
		}
		else
		{
			strUrl.append("?Sinzetech-Auth=" + g_xGlobal.sSinzetechAuth.toUtf8());
			strUrl.append("&Tenant-Id=" + g_xGlobal.sCurrentTenantId.toUtf8());
			strUrl.append("&Authorization=" + g_xGlobal.sCurrentTenantName.toUtf8());
		}
	}
	Tower tower;
	InitTowerModel(sProductCategoryId, &tower);
	QJsonObject jObject;
	QJsonArray componentList=GetPartList(&tower);
	QJsonArray productInfoList=GetTowerPlaceList(&tower);
	jObject["componentList"] = componentList;
	jObject["productCategoryId"] = sProductCategoryId;
	jObject["productInfoList"] = productInfoList;
	//post�����������Json��ʽ�����ȴ���QJsonObject���󣬲��������ݡ�
	QJsonDocument doc;
	doc.setObject(jObject);
	QVariantMap map = jObject.toVariantMap();
	QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
	QString resultData;
	oper->post(strUrl, jsonData, resultData);
	//��ȡ��������
	QJsonObject towerJsonObj = StringToJson(resultData);
	int niCode = towerJsonObj.toVariantMap()["code"].toString().toInt();
	QString errorInfo = towerJsonObj.toVariantMap()["msg"].toString();
	if (niCode == 200)
	{
		QVariantMap towerData = towerJsonObj.toVariantMap()["data"].toJsonObject().toVariantMap();
		QString sCallbackTemplate = towerData["callbackTemplate"].toString();
		QString sCallbackVar = towerData["callbackVar"].toString();
		int niExpires = towerData["expires"].toInt();
		QString sOssPushUrl = towerData["ossPushUrl"].toString();
	}
	else 
		QMessageBox::information(NULL, "error", errorInfo); 
}

void PDMTest::InitTowerModel(QString sId, Tower* tower)
{
	//
	CBoltRec botListInfoList;
	botListInfoList.adoptLandMax = 0;
	botListInfoList.adoptLandMin = 0;
	botListInfoList.description = " ";
	botListInfoList.diameter = 30.0;
	botListInfoList.level = "M30X60";
	botListInfoList.name = "��ͨ��˨";
	botListInfoList.num = 64;
	botListInfoList.singleWeight = 0.0;
	botListInfoList.specs = "M30X60";
	botListInfoList.type = "��ͨ��˨";
	botListInfoList.unbuckleLength = 0;
	botListInfoList.validLength = 0;
	botListInfoList.id = " ";
	botListInfoList.operationType = " ";
	//
	CTowerPartRef componentDetailList;
	componentDetailList.code = "101";
	componentDetailList.num = 1;
	componentDetailList.id = " ";
	componentDetailList.operationType = 1;
	//
	CTowerPlace productInfoList;
	productInfoList.boltRecList.push_back(botListInfoList);
	productInfoList.partRefList.push_back(componentDetailList);
	productInfoList.productNumber = "";
	//
	CTowerPart componentList;
	componentList.basicsPartNum = 1;
	componentList.basicsWeight = 103;
	componentList.bend = "";
	componentList.code = "101";
	componentList.electricWelding = "";
	componentList.groove = "";
	componentList.intersectingLine = 0;
	componentList.length = 7608;
	componentList.materialName = "�Ǹ�";
	componentList.openCloseAngle = "�кϽǣ�����Ϊ90��";
	componentList.perforate = "";
	componentList.rootClear = "";
	componentList.shovelBack = "";
	componentList.slottedForm = "";
	componentList.squash = "";
	componentList.structureSpec = "110x8";
	componentList.structureTexture = "�Ǹ�";
	componentList.thickness = 8;
	componentList.width = 110;
	componentList.id = "";
	componentList.operationType = 1;
	//
	tower->partList.push_back(componentList);
	tower->productCategoryId = sId;
	tower->towerPlaceList.push_back(productInfoList);
}

QJsonArray PDMTest::GetPartList(Tower* tower)
{
	QJsonArray componentList;
	for (int i = 0; i < 1; i++)
	{
		QJsonObject objComponentList;
		objComponentList["basicsPartNum"] = tower->partList[i].basicsPartNum;
		objComponentList["basicsWeight"] = tower->partList[i].basicsWeight;
		objComponentList["bend"] = tower->partList[i].bend;
		objComponentList["code"] = tower->partList[i].code;
		objComponentList["electricWelding"] = tower->partList[i].electricWelding;
		objComponentList["groove"] = tower->partList[i].groove;
		objComponentList["intersectingLine"] = tower->partList[i].intersectingLine;
		objComponentList["length"] = tower->partList[i].length;
		objComponentList["materialName"] = tower->partList[i].materialName;
		objComponentList["openCloseAngle"] = tower->partList[i].openCloseAngle;
		objComponentList["perforate"] = tower->partList[i].perforate;
		objComponentList["rootClear"] = tower->partList[i].rootClear;
		objComponentList["shovelBack"] = tower->partList[i].shovelBack;
		objComponentList["slottedForm"] = tower->partList[i].slottedForm;
		objComponentList["squash"] = tower->partList[i].squash;
		objComponentList["structureSpec"] = tower->partList[i].structureSpec;
		objComponentList["structureTexture"] = tower->partList[i].structureTexture;
		objComponentList["thickness"] = tower->partList[i].thickness;
		objComponentList["width"] = tower->partList[i].width;
		objComponentList["id"] = tower->partList[i].id;
		objComponentList["operationType"] = tower->partList[i].operationType;
		componentList.append(objComponentList);
	}
	return componentList;
}

QJsonArray PDMTest::GetTowerPlaceList(Tower* tower)
{
	QJsonArray productInfoList;
	QJsonArray botListInfoList = GetBoltRecList(tower);
	QJsonArray componentDetailList = GetTowerPartRefList(tower);
	for (int i = 0; i < 1; i++)
	{
		QJsonObject objProductInfoList;
		objProductInfoList["botListInfoList"] = botListInfoList;
		objProductInfoList["componentDetailList"] = componentDetailList;
		objProductInfoList["productNumber"] = tower->productCategoryId;
		productInfoList.append(objProductInfoList);
	}
	return productInfoList;
}

QJsonArray PDMTest::GetBoltRecList(Tower* tower)
{
	QJsonArray botListInfoList;
	for (int i = 0; i < 1; i++)
	{
		QJsonObject objBotListInfoList;
		objBotListInfoList["adoptLandMax"] = tower->towerPlaceList[i].boltRecList[i].adoptLandMax;
		objBotListInfoList["adoptLandMin"] = tower->towerPlaceList[i].boltRecList[i].adoptLandMin;
		objBotListInfoList["description"] = tower->towerPlaceList[i].boltRecList[i].description;
		objBotListInfoList["diameter"] = tower->towerPlaceList[i].boltRecList[i].diameter;
		objBotListInfoList["level"] = tower->towerPlaceList[i].boltRecList[i].level;
		objBotListInfoList["name"] = tower->towerPlaceList[i].boltRecList[i].name;
		objBotListInfoList["num"] = tower->towerPlaceList[i].boltRecList[i].num;
		objBotListInfoList["singleWeight"] = tower->towerPlaceList[i].boltRecList[i].singleWeight;
		objBotListInfoList["specs"] = tower->towerPlaceList[i].boltRecList[i].specs;
		objBotListInfoList["type"] = tower->towerPlaceList[i].boltRecList[i].type;
		objBotListInfoList["unbuckleLength"] = tower->towerPlaceList[i].boltRecList[i].unbuckleLength;
		objBotListInfoList["validLength"] = tower->towerPlaceList[i].boltRecList[i].validLength;
		objBotListInfoList["id"] = tower->towerPlaceList[i].boltRecList[i].id;
		objBotListInfoList["operationType"] = tower->towerPlaceList[i].boltRecList[i].operationType;
		botListInfoList.append(objBotListInfoList);
	}
	return botListInfoList;
}

QJsonArray PDMTest::GetTowerPartRefList(Tower* tower)
{
	QJsonArray componentDetailList;
	for (int i=0; i<1; ++i)
	{
		QJsonObject objComponentDetailList;
		objComponentDetailList["code"] = tower->towerPlaceList[i].partRefList[i].code;
		objComponentDetailList["num"] = tower->towerPlaceList[i].partRefList[i].num;
		objComponentDetailList["id"] = tower->towerPlaceList[i].partRefList[i].id;
		objComponentDetailList["operationType"] = tower->towerPlaceList[i].partRefList[i].operationType;
		componentDetailList.append(objComponentDetailList);
	}
	return componentDetailList;
}

void PDMTest::On_SearchButton_Click()
{
	int rowIndex = ui.tableWidget->currentRow();
	//
	if (rowIndex == -1)
	{
		QMessageBox::information(NULL, "�ϴ��ļ�ʧ��", "�ϴ��ļ�ʧ�ܣ�����ĳ���ϴ�", QMessageBox::Cancel);
		return;
	}
	QString sProductCategoryId = ui.tableWidget->item(rowIndex, 0)->text();
	QString url = "http://tc-dev-gateway.dhwy.cn/tower-data-archive/productCategory/bom/"+ sProductCategoryId;
	QString strUrl = url;
	if (url != NULL && !url.isEmpty()) {
		if (strUrl.indexOf("?") > 0)
		{
			strUrl.append("&Sinzetech-Auth=" + g_xGlobal.sSinzetechAuth.toUtf8());
			strUrl.append("&Tenant-Id=" + g_xGlobal.sCurrentTenantId.toUtf8());
			strUrl.append("&Authorization=" + g_xGlobal.sCurrentTenantName.toUtf8());
		}
		else
		{
			strUrl.append("?Sinzetech-Auth=" + g_xGlobal.sSinzetechAuth.toUtf8());
			strUrl.append("&Tenant-Id=" + g_xGlobal.sCurrentTenantId.toUtf8());
			strUrl.append("&Authorization=" + g_xGlobal.sCurrentTenantName.toUtf8());
		}
	}
	QString resultData;
	oper->get(strUrl, resultData);

}