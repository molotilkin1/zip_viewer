#ifndef ZIPMODEL_H
#define ZIPMODEL_H

#include <QAbstractTableModel>
#include <QString>
#include <QVector>
#include <QVariant>
#include <QFileInfo>

class ZipModel : public QAbstractTableModel
{
    Q_OBJECT


public:
    explicit ZipModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void loadArchive(const QString &archivePath);
private:
    struct FileData {
        QString path;
        qint64 compressedSize;
        qint64 uncompressedSize;
    };

    QVector<FileData> files;
};

#endif // ZIPMODEL_H
