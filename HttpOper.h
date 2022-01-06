#pragma once
#include <qobject.h>
#include <QObject>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>
#include <QCryptographicHash>
#include <QEventLoop>
#include <QtNetwork/QHttpPart>
#include <QtNetwork/QHttpMultiPart>
#include <QtNetwork/QtNetwork>
#include <QThread>
#include <QTimer>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QFile>
#include <QFileDialog>
#include <QMap>
#include <QMessageBox>
#include "JsonTransform.h"


class CHttpOper :
	public QObject
{
	Q_OBJECT

public:
	explicit CHttpOper(QObject* parent = nullptr);
	~CHttpOper();
	//  设置同时超时时间
	inline void setPassTime(int passTime)
	{
		m_passTime = passTime;
	}

public slots:
	//  信号槽的方式，子线程执行
	void post(QString url, const QMap<QString, QByteArray>& formData);
	void post2(QString url, const QMap<QString, QByteArray>& formData);
	void post(QString url, const QByteArray& jsonData);
	void post2(QString url, const QByteArray& jsonData, QMap<QString, QString> headerMaps);
	void get(QString url);
	void PutFinished();
	void HttpUploadFinished();
	void HttpDownloadFinished();

signals:
	//  子线程执行完后，发出信号 data 返回的数据，bRet 网络通信结果
	void finished(const QString& data, bool bRet);

public:
	//  下面的接口也可以直接调用, 与调用者将处于同一线程
	//  http post 网络通信, passTime 超时时间
	bool post(QString url, const QMap<QString, QByteArray>& formData, QString& outData, int passTime = 60000);
	bool post2(QString url, const QMap<QString, QByteArray>& formData, QString& outData, int passTime = 60000);
	bool post(QString url, const QByteArray& jsonData, QString& resultString, int passTime = 60000);
	bool post2(QString url, const QByteArray& jsonData, QString& resultString, QMap<QString, QString> headerMaps,int passTime = 60000);
	//  http get 请求，passTime 超时时间
	bool get(QString url, QString& outData, int passTime = 60000);
	void PutFile(const QString& url, QMap<QString, QString> headerMaps, const QString& fileName, const QString& filePath);
	void UploadFile(const QString& url, const QString& filePath, const QString& idData);
	void DownloadFile(const QString& url, const QString& idData);
	QByteArray GetDownBackData() { return m_downloadBackData; }
	QString GetDownBackFileName() { return m_downFileName; }
	QByteArray GetUpLoadBackData() { return m_uploadBackData; }

private:
	QThread* m_pThread;
	int m_passTime;     //  通信超时时间
public:
	QNetworkAccessManager* m_httpManager;
	QNetworkReply* m_uploadFileReply;
	QNetworkReply* m_downloadFileReply;
	QFile* m_imgFile;
	QFile* m_backFile;
	QByteArray m_uploadBackData;	//上传文件返回的数据
	QString m_downFileName;		//下载文件名
	QByteArray m_downloadBackData;	//下载文件返回的数据
};

