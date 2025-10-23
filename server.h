#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include <QVariantList>

class Server : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList states READ states NOTIFY statesChanged)   // <-- bindable in QML
public:
    explicit Server(quint16 port, QObject* parent=nullptr);

    Q_INVOKABLE void send(int index, bool checked);   // QML → Android
    QVariantList states() const;                      // for QML binding

signals:
    void statesChanged();                             // notify QML a value changed
    void stateUpdated(int index, bool checked);       // optional per-index signal

private slots:
    void onNewConnection();
    void onReadyRead();
    void onDisconnected();

private:
    QTcpServer server_;
    QList<QTcpSocket*> clients_;
    QVector<bool> stateVec_{QVector<bool>(16, false)};  // 16 checkboxes
};
