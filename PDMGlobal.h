#pragma once
#include <QString>
#include <vector>

using namespace std;
struct Tenant
{
	QString tenantId;
	QString tenantName;
};

//����ȫ��
struct Global
{
	Global() {};
	~Global() {};
	QString sSinzetechAuth;	//token
	QString sAuthorization;	//
	vector<Tenant> xTenantVector;
	QString sCurrentTenantId;	//ѡ���⻧ID
	QString sCurrentTenantName;	//ѡ���⻧����
};

extern Global g_xGlobal;

//////////////////////////////////////////////////////////////////////////
struct SegPackageFileVersion {
	QString sFileVersionId;
	QString sFileName;
	QString sOssPath;
	QString sVersion;
};

struct TowerSegPackage
{
	QString sPackageId;
	QString sPackageName;
	vector<SegPackageFileVersion> xFileVersionVector;
};

//����������Ϣ
struct BaseConfig
{
	QString sWarehouseId;
	QString sProductCategoryId;
	vector<TowerSegPackage> xTowerSegPackageVector;
};

extern vector<BaseConfig> g_xbaseConfigVector;
