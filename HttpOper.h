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
	//  ����ͬʱ��ʱʱ��
	inline void setPassTime(int passTime)
	{
		m_passTime = passTime;
	}

public slots:
	//  �źŲ۵ķ�ʽ�����߳�ִ��
	void post(QString url, const QMap<QString, QByteArray>& formData);
	void post2(QString url, const QMap<QString, QByteArray>& formData);
	void post(QString url, const QByteArray& jsonData);
	void post2(QString url, const QByteArray& jsonData, QMap<QString, QString> headerMaps);
	void get(QString url);
	void PutFinished();
	void HttpUploadFinished();
	void HttpDownloadFinished();

signals:
	//  ���߳�ִ����󣬷����ź� data ���ص����ݣ�bRet ����ͨ�Ž��
	void finished(const QString& data, bool bRet);

public:
	//  ����Ľӿ�Ҳ����ֱ�ӵ���, ������߽�����ͬһ�߳�
	//  http post ����ͨ��, passTime ��ʱʱ��
	bool post(QString url, const QMap<QString, QByteArray>& formData, QString& outData, int passTime = 60000);
	bool post2(QString url, const QMap<QString, QByteArray>& formData, QString& outData, int passTime = 60000);
	bool post(QString url, const QByteArray& jsonData, QString& resultString, int passTime = 60000);
	bool post2(QString url, const QByteArray& jsonData, QString& resultString, QMap<QString, QString> headerMaps,int passTime = 60000);
	//  http get ����passTime ��ʱʱ��
	bool get(QString url, QString& outData, int passTime = 60000);
	void PutFile(const QString& url, QMap<QString, QString> headerMaps, const QString& fileName, const QString& filePath);
	void UploadFile(const QString& url, const QString& filePath, const QString& idData);
	void DownloadFile(const QString& url, const QString& idData);
	QByteArray GetDownBackData() { return m_downloadBackData; }
	QString GetDownBackFileName() { return m_downFileName; }
	QByteArray GetUpLoadBackData() { return m_uploadBackData; }

private:
	QThread* m_pThread;
	int m_passTime;     //  ͨ�ų�ʱʱ��
public:
	QNetworkAccessManager* m_httpManager;
	QNetworkReply* m_uploadFileReply;
	QNetworkReply* m_downloadFileReply;
	QFile* m_imgFile;
	QFile* m_backFile;
	QByteArray m_uploadBackData;	//�ϴ��ļ����ص�����
	QString m_downFileName;		//�����ļ���
	QByteArray m_downloadBackData;	//�����ļ����ص�����
};

