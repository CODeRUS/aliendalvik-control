#ifndef TOGGLESMODEL_H
#define TOGGLESMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QStringList>
#include <mlite5/MDesktopEntry>
#include <mlite5/MGConfItem>

class TogglesModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit TogglesModel(QObject *parent = 0);

    Q_INVOKABLE void move(int from, int to);

    Q_PROPERTY(QStringList hidden READ hidden NOTIFY hiddenChanged)
    QStringList hidden() const;
    Q_INVOKABLE void hideToggle(const QString &source);

    Q_PROPERTY(bool editMode READ editMode WRITE setEditMode NOTIFY editModeChanged)
    bool editMode() const;
    void setEditMode(bool editMode);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QHash<int, QByteArray> roleNames() const { return _roles; }

private slots:
    void shortcutsChanged();

private:
    void update();
    QString getIconPath(const QString &path) const;

    QHash<int, QByteArray> _roles;
    QStringList _sourceData;
    QList<QVariantMap> _sourceModel;
    QList<QVariantMap> _modelData;

    MGConfItem *shortcutsConf;
    MGConfItem *orderingConf;
    MGConfItem *hiddenConf;

    bool _editMode;

signals:
    void hiddenChanged();
    void editModeChanged();
};

#endif // TOGGLESMODEL_H
