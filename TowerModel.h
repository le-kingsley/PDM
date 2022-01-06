#pragma once
#include <QObject>
#include <QString>
#include <vector>
using namespace std;

class CBoltRec
{
public:
	CBoltRec(){;}
	~CBoltRec() { ; }
public:
	double adoptLandMax;
	double adoptLandMin;
	QString	description;
	double diameter;
	QString level;
	QString	name;
	int	num;
	double	singleWeight;
	QString	specs;
	QString	type;
	double	unbuckleLength;
	double	validLength;
	QString	id;
	QString	operationType;
};
class CTowerPartRef
{
public:
	CTowerPartRef() { ; }
	~CTowerPartRef() { ; }
public:
	QString code;
	int num;
	QString id;
	QString operationType;
};
class CTowerPlace
{
public:
	CTowerPlace() { ; }
	~CTowerPlace() { ; }

public:
	vector<CBoltRec> boltRecList;
	vector<CTowerPartRef> partRefList;
	QString productNumber;
};

class CTowerPart
{
public:
	CTowerPart() { ; }
	~CTowerPart() { ; }

public:
	int basicsPartNum;
	double basicsWeight;
	QString bend;
	QString code;
	QString electricWelding;
	QString groove;
	double intersectingLine;
	double length;
	QString materialName;
	QString openCloseAngle;
	QString perforate;
	QString rootClear;
	QString shovelBack;
	QString slottedForm;
	QString squash;
	QString structureSpec;
	QString structureTexture;
	double thickness;
	double width;
	QString id;
	int operationType;
};
class Tower
{
public:
	Tower() { ; }
	~Tower() { ; }

public:
	
	vector<CTowerPart> partList;
	QString productCategoryId;
	vector<CTowerPlace> towerPlaceList;
};

