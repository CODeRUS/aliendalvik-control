#ifndef DESKTOPFILEMODEL_H
#define DESKTOPFILEMODEL_H

#include <QAbstractListModel>

class DesktopFileModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum FileRoles {
        NameRole = Qt::UserRole + 1,
        IconRole,
        PathRole
    };

    explicit DesktopFileModel(bool showHidden, QObject *parent = 0);
    virtual ~DesktopFileModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QHash<int, QByteArray> roleNames() const { return _roles; }

    QVariantMap get(int index);

    void fillData(bool showHiden);

private:
    QHash<int, QByteArray> _roles;
    QVector<QVariantMap> _modelData;

    bool _showHidden;

    QString getIconPath(const QString &path) const;

public slots:
    void fillDataReally();

signals:
    void dataFillEnd();

};

#endif // DESKTOPFILEMODEL_H
