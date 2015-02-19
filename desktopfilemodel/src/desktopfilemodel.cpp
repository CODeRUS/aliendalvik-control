#include "desktopfilemodel.h"

#include <QDir>
#include <mlite5/MDesktopEntry>

#include <QTimer>
#include <QCoreApplication>
#include <QDebug>

DesktopFileModel::DesktopFileModel(bool showHidden, QObject *parent) :
    QAbstractListModel(parent)
{
    _roles[NameRole] = "name";
    _roles[IconRole] = "icon";
    _roles[PathRole] = "path";

    fillData(showHidden);
}

DesktopFileModel::~DesktopFileModel()
{

}

int DesktopFileModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return _modelData.count();
}

QVariant DesktopFileModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if (row < 0 || row >= _modelData.size())
        return QVariant();
    return _modelData[index.row()][_roles[role]];
}

QVariantMap DesktopFileModel::get(int index)
{
    if (index > -1 && index < _modelData.size())
        return _modelData[index];
    return QVariantMap();
}

QString DesktopFileModel::getIconPath(const QString &path) const
{
    if (path.isEmpty()) {
        return "";
    }
    else if (QFile(path).exists()) {
        return path;
    }
    else {
        return QString("image://theme/%1").arg(path);
    }
}

void DesktopFileModel::fillDataReally()
{
    beginResetModel();
    _modelData.clear();
    endResetModel();

    QStringList desktopPath;
    desktopPath << "/usr/share/applications";
    desktopPath << QString("%1/.local/share/applications").arg(QDir::homePath());

    foreach (const QString &path, desktopPath) {
        QDir desktopDir(path);
        foreach (const QString &desktop, desktopDir.entryList(QStringList() << "*.desktop", QDir::Files, QDir::NoSort)) {
            MDesktopEntry entry(QString("%1/%2").arg(path).arg(desktop));
            if (entry.isValid() && entry.type() == "Application") {
                if (!_showHidden
                        && (entry.icon().isEmpty()
                        || entry.icon() == "icon-launcher-dummy"
                        || entry.noDisplay())) {
                    continue;
                }
                beginInsertRows(QModelIndex(), rowCount(), rowCount());
                QVariantMap data;
                data["name"] = entry.name();
                data["icon"] = getIconPath(entry.icon());
                data["path"] = entry.fileName();
//                qDebug() << "added:" << entry.fileName();
                _modelData.append(data);
                endInsertRows();
            }
            QCoreApplication::processEvents();
        }
    }
    Q_EMIT dataFillEnd();
}

void DesktopFileModel::fillData(bool showHidden)
{
    _showHidden = showHidden;
    QTimer::singleShot(1, this, SLOT(fillDataReally()));
}
