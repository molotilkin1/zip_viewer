#include "ZipModel.h"
#include "minizip-ng/mz.h"
#include "minizip-ng/mz_zip.h"
#include "minizip-ng/mz_strm_os.h"
#include <QFileInfo>
#include <QDebug>

ZipModel::ZipModel(QObject *parent)
    : QAbstractTableModel(parent) {}

int ZipModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return files.count();
}

int ZipModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return 3; // Путь, сжатый размер, расжатый размер
}

QVariant ZipModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || role != Qt::DisplayRole) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        const FileData &file = files[index.row()];
        switch (index.column()) {
        case 0:
            return file.path;
        case 1:
            return file.compressedSize;
        case 2:
            return file.uncompressedSize;
        default:
            return QVariant();
        }
    }
        return QVariant();
}

QVariant ZipModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return "Имя файла";
        case 1:
            return "Сжатый размер";
        case 2:
            return "Несжатый размер";
        default:
            return QVariant();
        }
    }
    return QVariant();
}
void ZipModel::loadArchive(const QString &archivePath) {

    void *zip_handle = mz_zip_create();  // Создаем объект для работы с архивом
       if (!zip_handle) {
           qWarning() << "Не удалось создать объект mz_zip";
           return;
       }

       // Создаем поток для работы с файлом
       void *stream = mz_stream_os_create();
       if (!stream) {
           qWarning() << "Не удалось создать поток mz_stream_os";
           mz_zip_delete(&zip_handle);
           return;
       }

       if (mz_stream_os_open(stream, archivePath.toUtf8().constData(), MZ_OPEN_MODE_READ) != MZ_OK) {
           qWarning() << "Не удалось открыть файл как поток:" << archivePath;
           mz_zip_delete(&zip_handle);
           mz_stream_os_delete(&stream);
           return;
       }

       if (mz_zip_open(zip_handle, stream, MZ_OPEN_MODE_READ) != MZ_OK) {
           qWarning() << "Не удалось открыть ZIP-архив:" << archivePath;
           mz_stream_os_close(stream);
           mz_stream_os_delete(&stream);
           mz_zip_delete(&zip_handle);
           return;
       }

       uint64_t numberOfEntries = 0;
       int32_t numResult = mz_zip_get_number_entry(zip_handle, &numberOfEntries);
       if (numResult != MZ_OK) {
           qWarning() << "Ошибка получения количества файлов в архиве | Код ошибки:" << numResult;
           mz_zip_close(zip_handle);
           mz_zip_delete(&zip_handle);
           return;
       }



       if (mz_zip_get_number_entry(zip_handle, &numberOfEntries) == MZ_OK) {
           qDebug() << "Количество файлов в архиве:" << numberOfEntries;
           files.clear();
           mz_zip_file *fileInfo = nullptr;
           int32_t result = mz_zip_goto_first_entry(zip_handle);
           if (result != MZ_OK) {
               qWarning() << "Не удалось перейти к первой записи | Код ошибки:" << result;
               mz_zip_close(zip_handle);
               mz_zip_delete(&zip_handle);
               return;
           }

           for (uint64_t i = 0; i < numberOfEntries; ++i) {
               if (mz_zip_entry_get_info(zip_handle, &fileInfo) == MZ_OK && fileInfo) {
                   FileData data;
                   data.path = QString::fromUtf8(fileInfo->filename);
                   data.compressedSize = fileInfo->compressed_size;
                   data.uncompressedSize = fileInfo->uncompressed_size;

                   files.append(data);
                   qDebug() << "Файл:" << data.path
                            << "| Сжатый размер:" << data.compressedSize
                            << "| Несжатый размер:" << data.uncompressedSize;

               } else {
                   qWarning() << "Не удалось получить информацию о файле" << i;
               }
               mz_zip_goto_next_entry(zip_handle);
           }

       } else {
           qWarning() << "Не удалось получить количество файлов в архиве";
       }

       // Закрываем zip
       mz_zip_close(zip_handle);
       mz_zip_delete(&zip_handle);

       // Закрываем поток
       mz_stream_os_close(stream);
       mz_stream_os_delete(&stream);
       beginResetModel();
       endResetModel();  // Обновить модель
       qDebug() << "Архив успешно обработан";
}
