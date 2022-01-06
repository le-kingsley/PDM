#include "HttpOper.h"

CHttpOper::CHttpOper(QObject* parent) : QObject(parent)
{
	m_passTime = 60000;
	m_pThread = new QThread(this);
	moveToThread(m_pThread);

	m_pThread->start();
}

CHttpOper::~CHttpOper()
{
	m_pThread->quit();
	m_pThread->wait();
}

void CHttpOper::post(QString url, const QMap<QString, QByteArray>& formData)
{
	QString outData;
	bool bRet = post(url, formData, outData, m_passTime);
	emit finished(outData, bRet);
}

void CHttpOper::post2(QString url, const QMap<QString, QByteArray>& formData)
{
	QString outData;
	bool bRet = post2(url, formData, outData, m_passTime);
	emit finished(outData, bRet);
}

void CHttpOper::post(QString url, const QByteArray& jsonData)
{
	QString outData;
	bool bRet = post(url, jsonData, outData, m_passTime);
	emit finished(outData, bRet);
}

void CHttpOper::post2(QString url, const QByteArray& jsonData, QMap<QString, QString> headerMaps)
{
	QString outData;
	bool bRet = post2(url, jsonData, outData, headerMaps, m_passTime);
	emit finished(outData, bRet);
}

void CHttpOper::get(QString url)
{
	QString outData;
	bool bRet = get(url, outData, m_passTime);
	emit finished(outData, bRet);
}

void CHttpOper::PutFinished()
{
	if (m_uploadFileReply->error() == QNetworkReply::NoError)
	{
		m_uploadBackData = m_uploadFileReply->readAll();
		m_imgFile->flush();
		m_imgFile->close();
		m_uploadFileReply->deleteLater();
		m_uploadFileReply = NULL;
		//delete m_imgFile;
		m_imgFile = NULL;
	}
	else
	{
		//qDebug() << "error string:" << m_uploadFileReply->errorString();
		m_uploadFileReply->deleteLater();
	}

}

void CHttpOper::HttpUploadFinished()
{
	if (m_uploadFileReply->error() == QNetworkReply::NoError)
	{
		m_uploadBackData = m_uploadFileReply->readAll();
		m_imgFile->flush();
		m_imgFile->close();
		m_uploadFileReply->deleteLater();
		m_uploadFileReply = NULL;
		m_imgFile = NULL;
		QJsonObject productJsonObj = StringToJson(m_uploadBackData);
		QString isSuccess = productJsonObj.toVariantMap()["success"].toString();
		if (!isSuccess.isEmpty() && isSuccess != NULL && isSuccess == "ok")
		{
			QMessageBox::information(NULL, "上传文件成功", "上传文件成功", QMessageBox::Cancel);
		}
	}
	else
	{
		QString sErrorString = m_uploadFileReply->errorString();
		qDebug() << "error string:" << m_uploadFileReply->errorString();
		m_uploadFileReply->deleteLater();
	}

}

void CHttpOper::HttpDownloadFinished()
{
	if (m_downloadFileReply->error() == QNetworkReply::NoError)
	{
		QStringList fileList = QString::fromUtf8(m_downloadFileReply->rawHeader("Content-Disposition")).split("fileName=");
		m_downFileName = fileList.last();
		m_downloadBackData = m_downloadFileReply->readAll();
		m_downloadFileReply->deleteLater();
		m_downloadFileReply = NULL;
	}
	else
		m_downloadFileReply->deleteLater();
}

bool CHttpOper::post(QString url, const QMap<QString, QByteArray>& formData, QString& outData, int passTime)
{
	QNetworkRequest request;
	request.setUrl(QUrl(url));

	QHttpMultiPart multiPart(QHttpMultiPart::FormDataType);

	QHttpPart textPart;
	//   textPart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
	for (auto it = formData.begin(); it != formData.end(); it++)
	{
		textPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(QString("form-data; name=\"%1\"").arg(it.key())));
		textPart.setBody(it.value());
		multiPart.append(textPart);
	}

	request.setHeader(QNetworkRequest::ContentTypeHeader, "multipart/form-data;boundary=" + multiPart.boundary());

	//  进行网络通信
	bool bRet = false;
	int nTimes = 2;
	for (int i = 0; i < nTimes; ++i)
	{
		QNetworkAccessManager mgr;
		QEventLoop loop;
		connect(&mgr, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);

		QTimer timer;
		timer.setSingleShot(true);
		connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
		timer.start((passTime > 0) ? passTime : 40000);

		QNetworkReply* pReply = mgr.post(request, &multiPart);
		connect(pReply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
		connect(pReply, SIGNAL(error(QNetworkReply::NetworkError)), &loop, SLOT(quit()));

		loop.exec();

		//  超时，重发
		if (!timer.isActive())
		{
			qDebug() << "post to url: " << url << " over time, time = " << passTime << ", post times: " << i;
			pReply->deleteLater();
			continue;
		}
		else
		{
			timer.stop();

			//  获取发送的返回结果
			QNetworkReply::NetworkError err = pReply->error();
			if (err != QNetworkReply::NoError)
			{
				qDebug() << "Failed: " << pReply->errorString();
				bRet = false;
			}
			else
			{
				outData = QString::fromUtf8(pReply->readAll());
				qDebug() << "Result: " << outData;
				bRet = true;
			}

			pReply->deleteLater();
			break;
		}
	}

	return bRet;
}

bool CHttpOper::post2(QString url, const QMap<QString, QByteArray>& formData, QString& outData, int passTime)
{
	QByteArray params;
	for (auto it = formData.begin(); it != formData.end(); it++)
	{
		params.append(it.key().toUtf8());
		params.append("=");
		params.append(it.value());
		params.append("&");
	}
	params.remove(params.length() - 1, 1);

	QNetworkRequest request;
	request.setUrl(QUrl(url));
	request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/x-www-form-urlencoded"));
	request.setHeader(QNetworkRequest::ContentLengthHeader, params.length());

	//  进行网络通信
	bool bRet = false;
	int nTimes = 2;
	for (int i = 0; i < nTimes; ++i)
	{
		QNetworkAccessManager mgr;
		QEventLoop loop;
		connect(&mgr, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);

		QTimer timer;
		timer.setSingleShot(true);
		connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
		timer.start((passTime > 0) ? passTime : 40000);

		QNetworkReply* pReply = mgr.post(request, params);
		connect(pReply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
		connect(pReply, SIGNAL(error(QNetworkReply::NetworkError)), &loop, SLOT(quit()));

		loop.exec();

		//  超时，重发
		if (!timer.isActive())
		{
			qDebug() << "post to url: " << url << " over time, time = " << passTime << ", post times: " << i;
			pReply->deleteLater();
			continue;
		}
		else
		{
			timer.stop();

			//  获取发送的返回结果
			QNetworkReply::NetworkError err = pReply->error();
			if (err != QNetworkReply::NoError)
			{
				qDebug() << "Failed: " << pReply->errorString();
				bRet = false;
			}
			else
			{
				//outObj = QJsonDocument::fromJson( pReply->readAll() ).object();
				outData = QString::fromUtf8(pReply->readAll());
				qDebug() << outData;
				bRet = true;
			}

			pReply->deleteLater();
			break;
		}
	}

	return bRet;
}

bool CHttpOper::post(QString url, const QByteArray& jsonData, QString& resultString, int passTime)
{
	QNetworkRequest request;
	request.setUrl(QUrl(url));
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json;charset=UTF-8");
	//  requestInfo.setRawHeader("Content-Type","application/json");//服务器要求的数据头部
	//  进行网络通信
	bool bRet = false;
	int nTimes = 2;
	for (int i = 0; i < nTimes; ++i)
	{
		QNetworkAccessManager mgr;
		QEventLoop loop;
		connect(&mgr, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);

		QTimer timer;
		timer.setSingleShot(true);
		connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
		timer.start((passTime > 0) ? passTime : 40000);
		request.setAttribute(QNetworkRequest::HTTP2AllowedAttribute, false);
		QNetworkReply* pReply = mgr.post(request, jsonData);
		connect(pReply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
		connect(pReply, SIGNAL(error(QNetworkReply::NetworkError)), &loop, SLOT(quit()));

		loop.exec();

		//  超时，重发
		if (!timer.isActive())
		{
			qDebug() << "post to url: " << url << " over time, time = " << passTime << ", post times: " << i;
			pReply->deleteLater();
			continue;
		}
		else
		{
			timer.stop();
			//  获取发送的返回结果
			QNetworkReply::NetworkError err = pReply->error();
			if (err != QNetworkReply::NoError)
			{
				qDebug() << "Failed: " << pReply->errorString();
				bRet = false;
			}
			else
			{
				//outObj = QJsonDocument::fromJson( pReply->readAll() ).object();
				//resultString = QString::fromUtf8(pReply->readAll());
				qDebug() << resultString;
				bRet = true;
			}
			resultString = QString::fromUtf8(pReply->readAll());
			pReply->deleteLater();
			break;
		}
	}

	return bRet;
}

bool CHttpOper::post2(QString url, const QByteArray& jsonData, QString& resultString, QMap<QString, QString> headerMaps, int passTime)
{
	QNetworkRequest request;
	request.setUrl(QUrl(url));
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json;charset=UTF-8");
	for (QMap<QString, QString>::iterator i = headerMaps.begin(); i != headerMaps.end(); i++)
	{
		request.setRawHeader(i.key().toUtf8(), i.value().toUtf8());
	}
	//  requestInfo.setRawHeader("Content-Type","application/json");//服务器要求的数据头部
	//  进行网络通信
	bool bRet = false;
	int nTimes = 2;
	for (int i = 0; i < nTimes; ++i)
	{
		QNetworkAccessManager mgr;
		QEventLoop loop;
		connect(&mgr, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);

		QTimer timer;
		timer.setSingleShot(true);
		connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
		timer.start((passTime > 0) ? passTime : 40000);

		QNetworkReply* pReply = mgr.post(request, jsonData);
		connect(pReply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
		connect(pReply, SIGNAL(error(QNetworkReply::NetworkError)), &loop, SLOT(quit()));

		loop.exec();

		//  超时，重发
		if (!timer.isActive())
		{
			qDebug() << "post to url: " << url << " over time, time = " << passTime << ", post times: " << i;
			pReply->deleteLater();
			continue;
		}
		else
		{
			timer.stop();

			//  获取发送的返回结果
			QNetworkReply::NetworkError err = pReply->error();
			if (err != QNetworkReply::NoError)
			{
				qDebug() << "Failed: " << pReply->errorString();
				bRet = false;
			}
			else
			{
				//outObj = QJsonDocument::fromJson( pReply->readAll() ).object();
				resultString = QString::fromUtf8(pReply->readAll());
				qDebug() << resultString;
				bRet = true;
			}

			pReply->deleteLater();
			break;
		}
	}

	return bRet;
}

bool CHttpOper::get(QString url, QString& outData, int passTime)
{
	QNetworkRequest request;
	request.setUrl(QUrl(url));
	request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/x-www-form-urlencoded"));

	//  进行网络通信
	bool bRet = false;
	int nTimes = 2;
	for (int i = 0; i < nTimes; ++i)
	{
		QNetworkAccessManager mgr;
		QEventLoop loop;
		connect(&mgr, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);

		QTimer timer;
		timer.setSingleShot(true);
		connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
		timer.start((passTime > 0) ? passTime : 40000);

		QNetworkReply* pReply = mgr.get(request);
		connect(pReply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
		connect(pReply, SIGNAL(error(QNetworkReply::NetworkError)), &loop, SLOT(quit()));

		loop.exec();

		//  超时，重发
		if (!timer.isActive())
		{
			qDebug() << "post to url: " << url << " over time, time = " << passTime << ", post times: " << i;
			pReply->deleteLater();
			continue;
		}
		else
		{
			timer.stop();

			//  获取发送的返回结果
			QNetworkReply::NetworkError err = pReply->error();
			if (err != QNetworkReply::NoError)
			{
				qDebug() << "Failed: " << pReply->errorString();
				bRet = false;
			}
			else
			{
				//outObj = QJsonDocument::fromJson( pReply->readAll() ).object();
				outData = QString::fromUtf8(pReply->readAll());
				qDebug() << outData;
				bRet = true;
			}

			pReply->deleteLater();
			break;
		}
	}

	return bRet;
}

void CHttpOper::PutFile(const QString& url, QMap<QString, QString> headerMaps, const QString& fileName, const QString& filePath)
{
	QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
	//把文件路径解析成文件名
	QHttpPart filePart;
	//QStringList typeHeaderPartList = fileName.split(".");
	//QString typeHeaderPart = QString("%1/%2").arg(typeHeaderPartList.first()).arg(typeHeaderPartList.last());
	//filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant(typeHeaderPart));
	//QString dispositionHeaderPart = QString("form-data; name=\"file\";filename=\"%1\";").arg(fileName);
	filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(QString(fileName).toUtf8()));
	QString tmaFileName = filePath + "/" + fileName;
	m_imgFile = new QFile(tmaFileName);
	m_imgFile->open(QIODevice::ReadOnly);
	//m_imgFile->setParent(multiPart);
	filePart.setBodyDevice(m_imgFile);
	multiPart->append(filePart);
	//
	QUrl httpUrl(url.toUtf8());
	QNetworkRequest request(httpUrl);
	for (QMap<QString, QString>::iterator i = headerMaps.begin(); i != headerMaps.end(); i++)
	{
		request.setRawHeader(i.key().toUtf8(), i.value().toUtf8());
	}

	request.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/octet-stream").toUtf8());
	//
	QByteArray fileByteArray;
	QFile file(tmaFileName);
	bool isOk = file.open(QFile::ReadOnly);
	if (!isOk) {
		return;
	}
	QTextStream stream(&file); // 设置IO设备给流对象
	QString array;
	while (!stream.atEnd()) {
		array += stream.readLine();
	}
	// 读文件
	fileByteArray = array.toUtf8();
	// 关闭文件
	file.close();
	m_uploadFileReply = m_httpManager->put(request, multiPart);

	multiPart->setParent(m_uploadFileReply);
	connect(m_uploadFileReply, SIGNAL(finished()), this, SLOT(HttpUploadFinished()));
	QEventLoop eventLoop;
	connect(m_httpManager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
	eventLoop.exec();
}

void CHttpOper::UploadFile(const QString& url ,const QString& filePath, const QString& idData)
{
	QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
	//把文件路径解析成文件名
	QStringList fileList = filePath.split("/");
	QString aplFileName = fileList.last();
	QHttpPart filePart;
	QStringList typeHeaderPartList = aplFileName.split(".");
	QString typeHeaderPart = QString("%1/%2").arg(typeHeaderPartList.first()).arg(typeHeaderPartList.last());
	filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant(typeHeaderPart));
	QString dispositionHeaderPart = QString("form-data; name=\"file\";filename=\"%1\";").arg(aplFileName);
	filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(dispositionHeaderPart));
	m_imgFile = new QFile(filePath);
	m_imgFile->open(QIODevice::ReadOnly);
	m_imgFile->setParent(multiPart);
	filePart.setBodyDevice(m_imgFile);
	multiPart->append(filePart);
	//url拼接
	QString strUrl = url;
	if (!idData.isEmpty() && idData != NULL)
	{
		if (url != NULL && !url.isEmpty()) {
			if(strUrl.indexOf("?") > 0)
				strUrl.append("&id=" + idData.toUtf8());
			else
				strUrl.append("?id=" + idData.toUtf8());
		}
	}
	/*QHttpPart idDataPart;
	idDataPart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
	idDataPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"id\""));
	idDataPart.setBody(idData.toUtf8());
	multiPart->append(idDataPart);*/
	QUrl httpUrl(strUrl);
	QNetworkRequest request(httpUrl);
	m_uploadFileReply = m_httpManager->post(request, multiPart);

	multiPart->setParent(m_uploadFileReply);
	connect(m_uploadFileReply, SIGNAL(finished()), this, SLOT(HttpUploadFinished()));
	QEventLoop eventLoop;
	connect(m_httpManager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
	eventLoop.exec();
}

void CHttpOper::DownloadFile(const QString& url, const QString& idData)
{
	//url拼接
	QString strUrl = url;
	if (!idData.isEmpty() && idData != NULL)
	{
		if (url != NULL && !url.isEmpty()) {
			if (strUrl.indexOf("?") > 0)
				strUrl.append("&id=" + idData.toUtf8());
			else
				strUrl.append("?id=" + idData.toUtf8());
		}
	}
	QUrl httpUrl(strUrl);
	QNetworkRequest request(httpUrl);
	m_downloadFileReply = m_httpManager->get(request);
	connect(m_downloadFileReply, SIGNAL(finished()), this, SLOT(HttpDownloadFinished()));
	QEventLoop eventLoop;
	connect(m_httpManager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
	eventLoop.exec();
}
