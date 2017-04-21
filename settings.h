#ifndef SETTINGS_H
#define SETTINGS_H

#include <QHash>
#include <QWidget>
#include <QSettings>
#include <QDebug>
#include <QListWidget>

class Settings : public QObject
{
    Q_OBJECT
public:
    static Settings* init(QWidget* parentWidget);
    static Settings* getInstance();
    ~Settings();

    void loadTranslations(QString languageName);
    QString getTranslation(QString category, QString key);
    void setupLanguageSelector(QListWidget* selector);

    QString getDataFolderPath() { return dataFolder; }

    QString getFilePath(QString file);
    QIcon getIcon(QString name);

    QVariant get(const QString &key, const QVariant &defaultValue = QVariant());
    void set(const QString &key, const QVariant &value);

    QColor getColor(const QString &key, const QColor &defaultColor = Qt::white);
    void setColor(const QString &key, const QColor &color);

protected:
    Settings(QWidget* parentWidget);

private slots:
    void setLanguage(QString language);

private:
    static Settings* instance;

    QSettings settings;

    QWidget* parentWidget;
    QHash<QString, QHash<QString, QString>*> translations;
    QHash<QString, QString> translatedFiles;

    QString dataFolder;
};

#endif // SETTINGS_H
