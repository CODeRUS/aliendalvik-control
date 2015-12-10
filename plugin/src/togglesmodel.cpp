#include "togglesmodel.h"
#include <QDir>
#include <QFile>

TogglesModel::TogglesModel(QObject *parent):
    QAbstractListModel(parent)
{
    _roles[Qt::DisplayRole] = "source";
    _roles[Qt::UserRole + 1] = "name";
    _roles[Qt::UserRole + 2] = "icon";
    _roles[Qt::UserRole + 3] = "path";

    shortcutsConf = new MGConfItem("/apps/powermenu/shortcuts", this);
    QObject::connect(shortcutsConf, SIGNAL(valueChanged()), this, SLOT(shortcutsChanged()));
    orderingConf = new MGConfItem("/apps/powermenu/ordering", this);
    hiddenConf = new MGConfItem("/apps/powermenu/hidden", this);

    _editMode = false;

    update();
}

void TogglesModel::update()
{
    beginResetModel();

    _modelData.clear();

    QStringList hiddenList = hidden();

    QStringList temp;
    QList<QVariantMap> tempModel;

    QDir dir("/usr/lib/qt5/qml/org/coderus/powermenu/toggles", "*.qml");
    foreach (const QString &toggle, dir.entryList()) {
        QString path = QString("toggles/%1").arg(toggle);

        QVariantMap item;
        item["source"] = path;
        item["name"] = QString();
        item["icon"] = QString();
        item["path"] = path;
        tempModel.append(item);

        temp.append(path);
    }

    QStringList shortcuts = shortcutsConf->value().toStringList();
    foreach (const QString &shortcut, shortcuts) {
        MDesktopEntry entry(shortcut);
        if (entry.isValid() && entry.type() == "Application") {
            QString path = entry.fileName();

            QVariantMap item;
            item["source"] = QString("ApplicationItem.qml");
            item["name"] = entry.name();
            item["icon"] = getIconPath(entry.icon());
            item["path"] = path;
            tempModel.append(item);

            temp.append(path);
        }
    }

    QStringList ordered = orderingConf->value().toStringList();
    if (ordered.isEmpty()) {
        _modelData = tempModel;
        _sourceData = temp;
    }
    else {
        foreach (const QString &item, ordered) {
            int index = temp.indexOf(item);
            if (index >= 0) {
                if (_editMode || !hiddenList.contains(item)) {
                    _modelData.append(tempModel[index]);
                }
                _sourceData.append(item);
            }
        }
        for (int i = 0; i < temp.size(); i++) {
            QString item = temp[i];
            if (!ordered.contains(item)) {
                if (_editMode || !hiddenList.contains(temp[i])) {
                    _modelData.append(tempModel[i]);
                }
                _sourceData.append(temp[i]);
            }
        }
    }

    endResetModel();
}

int TogglesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return _modelData.count();
}

QVariant TogglesModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if (row < 0 || row >= _modelData.size())
        return QVariant();
    return _modelData[row][_roles[role]];;
}

void TogglesModel::shortcutsChanged()
{
    QStringList shortcuts = shortcutsConf->value().toStringList();
    foreach (const QString &shortcut, shortcuts) {
        if (!_sourceData.contains(shortcut)) {
            MDesktopEntry entry(shortcut);
            if (entry.isValid() && entry.type() == "Application") {
                QString path = entry.fileName();

                QVariantMap item;
                item["source"] = QString("ApplicationItem.qml");
                item["name"] = entry.name();
                item["icon"] = getIconPath(entry.icon());
                item["path"] = path;
                beginInsertRows(QModelIndex(), _modelData.size(), _modelData.size());
                _modelData.append(item);
                endInsertRows();

                _sourceData.append(path);
                orderingConf->set(_sourceData);
            }
        }
    }
    for (int i = _sourceData.size() - 1; i > -1; i--) {
        if (_sourceData[i].endsWith(".desktop") && !shortcuts.contains(_sourceData[i])) {
            beginRemoveRows(QModelIndex(), i, i);
            for (int j = 0; j < _modelData.size(); j++) {
                if (_modelData[j]["path"].toString() == _sourceData[i]) {
                    beginRemoveRows(QModelIndex(), j, j);
                    _modelData.removeAt(j);
                    endRemoveRows();
                }
            }
            _sourceData.removeAt(i);
            endRemoveRows();
        }
    }
}

void TogglesModel::move(int from, int to)
{
    if (from != to) {
        if (to < from) {
            beginMoveRows(QModelIndex(), from, from, QModelIndex(), to);
        }
        else {
            beginMoveRows(QModelIndex(), from, from, QModelIndex(), to + 1);
        }
        _modelData.move(from, to);
        _sourceData.move(from, to);
        endMoveRows();

        orderingConf->set(_sourceData);
    }
}

QStringList TogglesModel::hidden() const
{
    return hiddenConf->value().toStringList();
}

void TogglesModel::hideToggle(const QString &source)
{
    QStringList temp = hidden();
    if (temp.contains(source)) {
        temp.removeAll(source);
    }
    else {
        temp.append(source);
    }
    hiddenConf->set(temp);
    Q_EMIT hiddenChanged();
}

bool TogglesModel::editMode() const
{
    return _editMode;
}

void TogglesModel::setEditMode(bool editMode)
{
    if (_editMode != editMode) {
        _editMode = editMode;
        Q_EMIT editModeChanged();

        QStringList hiddenList = hidden();
        if (_editMode) {
            for (int i = 0; i < _sourceData.size(); i++) {
                if (hiddenList.contains(_sourceData[i])) {
                    beginInsertRows(QModelIndex(), i, i);
                    QVariantMap item;
                    item["source"] = _sourceData[i];
                    item["name"] = QString();
                    item["icon"] = QString();
                    item["path"] = _sourceData[i];
                    _modelData.insert(i, item);
                    endInsertRows();
                }
            }
        }
        else {
            for (int i = _sourceData.size() - 1; i > -1; i--) {
                if (hiddenList.contains(_sourceData[i])) {
                    beginRemoveRows(QModelIndex(), i, i);
                    _modelData.removeAt(i);
                    endRemoveRows();
                }
            }
        }
    }
}

QString TogglesModel::getIconPath(const QString &path) const
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

