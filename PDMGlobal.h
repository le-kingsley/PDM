#pragma once
#include <QString>
#include <vector>

using namespace std;
struct Tenant
{
	QString tenantId;
	QString tenantName;
};

//设置全局
struct Global
{
	Global() {};
	~Global() {};
	QString sSinzetechAuth;	//token
	QString sAuthorization;	//
	vector<Tenant> xTenantVector;
	QString sCurrentTenantId;	//选中租户ID
	QString sCurrentTenantName;	//选中租户名称
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

//基础配置信息
struct BaseConfig
{
	QString sWarehouseId;
	QString sProductCategoryId;
	vector<TowerSegPackage> xTowerSegPackageVector;
};

extern vector<BaseConfig> g_xbaseConfigVector;
