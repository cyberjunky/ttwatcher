#include "downloaddialog.h"
#include "ui_downloaddialog.h"


#include <QNetworkReply>
#include <QNetworkRequest>
#include <QByteArray>
#include <QUrl>
#include <QDebug>
#include <QNetworkProxy>
#include <QDir>
#include <QStringList>
#include <QMessageBox>
#include "singleshot.h"

void DownloadDialog::showEvent(QShowEvent *e)
{
    QDialog::showEvent(e);
    // we have to run process a bit after showEvent is done, otherwise
    // we might call accepted while still in showEvent,
    // which apparently fails.
    SingleShot::go([this]{
        process();
    }, 250, true, this);
}

DownloadDialog::DownloadDialog(Settings *settings, TTManager *ttManager, QWidget *parent) :
    QDialog(parent),
    m_Settings(settings),
    m_TTManager(ttManager),
    ui(new Ui::DownloadDialog)
{
    ui->setupUi(this);
    connect(&m_Manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onFinished(QNetworkReply*)));
    connect(m_TTManager, SIGNAL(allExportingFinished()), this, SLOT(onExportingFinished()));
    connect(m_TTManager, SIGNAL(exportError(QString)), this, SLOT(onExportError(QString)));
    QNetworkProxy p;
    p.setHostName("localhost");
    p.setPort(8888);
    p.setType(QNetworkProxy::HttpProxy);
    m_Manager.setProxy(p);
}

DownloadDialog::~DownloadDialog()
{
    delete ui;
}

int DownloadDialog::processWatches()
{
    // if the window is already visible we don't need to process.
    if ( this->isVisible() )
    {
        qDebug() << "DownloadDialog::processWatches / already visible.";
        return QDialog::Rejected;
    }

    return exec();
}

QStringList DownloadDialog::filesDownloaded() const
{
    return m_Files;
}

void DownloadDialog::process()
{
    ui->logWidget->clear();
    bool shouldDownloadQuickFix = false;

    if ( m_TTManager->watches().count() == 0 )
    {
        workInfo(tr("Done"), true);
        return;
    }

    foreach ( TTWatch * watch, m_TTManager->watches())
    {
        QByteArray prefData;

        /**********************************************/
        /* 1. LOADING PREFERENCES */
        /**********************************************/

        emit workInfo(tr("Loading Preferences"), false);

        if ( !watch->downloadPreferences(prefData) )
        {
            qCritical() << "MainWindow::onWatchesChanged / unable to load preferences file.";
            workInfo(tr("Failed to load preferences"), true);
            continue;
        }


        /*QFile tempf("tempf.xml");
        tempf.open(QIODevice::WriteOnly);
        tempf.write(prefData);
        tempf.close();*/

        WatchPreferencesPtr preferences = m_TTManager->preferences( watch->serial() );
        if ( !preferences )
        {
            qCritical() << "MainWindow::onWatchesChanged / did not get a preferences ptr.";
            continue;
        }

        if ( !preferences->parsePreferences( prefData ) )
        {
            qCritical() << "MainWindow::onWatchesChanged / failed to parse preferences.";
            workInfo(tr("Failed to parse preferences"), true);
            continue;
        }

        if ( !m_Settings->autoDownload() )
        {
            continue;
        }


        /**********************************************/
        /* 2. LOAD TTBINS */
        /**********************************************/

        workInfo(tr("Downloading .ttbins"), false);

        QStringList files = watch->download(Settings::ttdir() + QDir::separator() + preferences->name(), true);

        if ( files.count() > 0 )
        {
            workInfo(tr("Exporting .ttbins"), false);

            /**********************************************/
            /* 3. EXPORT TTBINS */
            /**********************************************/

            foreach ( const QString & filename, files )
            {
                workInfo(tr("Exporting .ttbin . %1").arg(filename), false);

                if ( !preferences->exportFile(filename) )
                {
                    workInfo(tr("Exporting .ttbin failed. %1").arg(filename), false);
                }
            }

            m_Files.append( files );
        }
        else
        {
            workInfo(tr("No new workouts."), false);
        }

        QDateTime lastFixUpload = m_Settings->lastQuickFix(watch->serial());
        QDateTime now = QDateTime::currentDateTime();
        if ( lastFixUpload.secsTo(now) > 24 * 3600 )
        {
            shouldDownloadQuickFix = true;
        }

    }

    /**********************************************/
    /* 4. Save preferences data. */
    /**********************************************/
    m_TTManager->savePreferences();

    /**********************************************/
    /* 5. APPLY GPS QUICK FIX DATA */
    /**********************************************/

    if ( shouldDownloadQuickFix )
    {
        workInfo(tr("Downloading GPS Quick Fix data"), false);
        QNetworkRequest r;
        r.setUrl(QUrl( "http://gpsquickfix.services.tomtom.com/fitness/sifgps.f2p3enc.ee"));
        m_Manager.get(r);
    }
    else
    {
        onExportingFinished();
    }
}

void DownloadDialog::workInfo(const QString &message, bool done)
{
    qDebug() << "DownloadDialog::workInfo" << message<< done;
    ui->logWidget->addItem(message);
    qApp->processEvents();
    if ( done )
    {
        accept();
    }
}

void DownloadDialog::onFinished(QNetworkReply *reply)
{
    if ( reply->error() != QNetworkReply::NoError )
    {
       workInfo(tr("Download QuickFix Data Failed."),true);
       return;
    }

    QByteArray data = reply->readAll();
    reply->deleteLater();

    foreach ( TTWatch * watch, m_TTManager->watches())
    {
        workInfo(tr("Writing GPS Quick Fix data to %1...").arg(watch->serial()), false);
        watch->writeFile( data, FILE_GPSQUICKFIX_DATA, true );
        qDebug() << "PostGPS FIX" << watch->postGPSFix();
        m_Settings->setQuickFixDate( watch->serial(), QDateTime::currentDateTime());
    }

    onExportingFinished();
}

void DownloadDialog::onExportingFinished()
{
    bool stillExporting = false;
    PreferencesMap::iterator i = m_TTManager->preferences().begin();
    for(;i!=m_TTManager->preferences().end();i++)
    {
        if ( i.value()->isExporting() )
        {
            stillExporting = true;
            break;
        }
    }
    if (!stillExporting )
    {
        workInfo(tr("Done"), true);
    }
}

void DownloadDialog::onExportError(QString message)
{
    QMessageBox::warning(this, tr("Error Exporting"), message);
}
