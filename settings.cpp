#include "settings.h"

#include <QApplication>
#include <QFile>
#include <QDir>
#include <QDirIterator>
#include <QTextStream>
#include <QMessageBox>
#include <QStringList>


Settings* Settings::instance = NULL;

Settings* Settings::init(QWidget* parentWidget)
{
    if (instance != NULL)
        throw new std::runtime_error("SettingsManager already inited.");

    instance = new Settings(parentWidget);
    return instance;
}

Settings* Settings::getInstance()
{
    if (instance == NULL)
        throw new std::runtime_error("SettingsManager not inited.");

    return instance;
}


Settings::Settings(QWidget* parentWidget)
{
    this->parentWidget = parentWidget;

    dataFolder = QCoreApplication::applicationDirPath() + "/blargirc_data/";

    // QString language = settings.value("Language", "English").toString();
    // loadTranslations(language);
}

Settings::~Settings()
{
    QList<QHash<QString, QString>*> cats = translations.values();

    for (int i = 0; i < cats.length(); i++)
        delete cats[i];
}

void Settings::loadTranslations(QString languageName)
{
    QFile file(QCoreApplication::applicationDirPath() + "/blargirc_data/languages/"+languageName+"/translations.txt");

    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(parentWidget, "BlargIRC", "The language \""+languageName+"\" is not present in the /blargirc_data/languages/ folder.\n\nEnglish will be selected as fallback language.");
        loadTranslations("English");
    }

    QTextStream in(&file);
    in.setCodec("UTF-8");

    translations.clear();

    QHash<QString, QString>* actualCat;

    while (!in.atEnd())
    {
        QString line = in.readLine();

        if (line.startsWith(';'))
            continue;

        else if (line.startsWith('['))
        {
            QString actualCatName = line.mid(1, line.length()-2);
            QHash<QString, QString>* cat = new QHash<QString, QString>;
            actualCat = cat;
            translations.insert(actualCatName, cat);
        }

        else if (!line.isEmpty())
        {
            QStringList fields = line.split('=');
            if (fields.size() == 2)
                actualCat->insert(fields[0], fields[1].replace("\\n", "\n"));
        }
    }

    QStringList translateFiles;
    /*translateFiles << "entrancetypes.txt"
                   << "levelnames.xml"
                   << "musicids.txt"
                   << "spritecategories.xml"
                   << "spritedata.xml"
                   << "tilebehaviors.xml"
                   << "tilesetnames.txt";*/

    foreach (QString transFile, translateFiles)
    {
        QString path = QCoreApplication::applicationDirPath() + "/blargirc_data/languages/"+languageName+"/"+transFile;
        if(!QFile(path).exists())
            path = QCoreApplication::applicationDirPath() + "/blargirc_data/" + transFile;

        translatedFiles.insert(transFile, path);
    }

    file.close();
}

QString Settings::getTranslation(QString category, QString key)
{
    return translations.value(category, new QHash<QString, QString>)->value(key, "<NOT LOADED>");
}

void Settings::setupLanguageSelector(QListWidget* selector)
{
    selector->blockSignals(true);
    selector->clear();

    QDir translationsFolder(QCoreApplication::applicationDirPath() + "/blargirc_data/languages/");
    translationsFolder.setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
    QDirIterator directories(translationsFolder, QDirIterator::NoIteratorFlags);

    while(directories.hasNext())
    {
        directories.next();
        if (QFile(directories.filePath()+"/translations.txt").exists())
        {
            QListWidgetItem* item = new QListWidgetItem(directories.fileName());

            selector->addItem(item);

            if (directories.fileName() == settings.value("Language", "English").toString())
                selector->setCurrentItem(item);
        }
    }

    connect(selector, SIGNAL(currentTextChanged(QString)), this, SLOT(setLanguage(QString)));
    selector->blockSignals(false);
}

QString Settings::getFilePath(QString file)
{
    return translatedFiles.value(file);
}

void Settings::setLanguage(QString language)
{
    settings.setValue("Language", language);
    QMessageBox::information(parentWidget, "BlargIRC", QString("The Language has been changed to %1.\n\nPlease restart BlargIRC for the changes to take effect.").arg(language));
}

QVariant Settings::get(const QString &key, const QVariant &defaultValue)
{
    return settings.value(key, defaultValue);
}

void Settings::set(const QString &key, const QVariant &value)
{
    settings.setValue(key, value);
}

QColor Settings::getColor(const QString &key, const QColor &defaultColor)
{
   return QColor::fromRgba(settings.value(key, defaultColor.rgba()).toUInt());
}

void Settings::setColor(const QString &key, const QColor &color)
{
    settings.setValue(key, color.rgba());
}

QIcon Settings::getIcon(QString name)
{
    return QIcon(dataFolder + "icons/" + name);
}
