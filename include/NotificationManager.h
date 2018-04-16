#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include <QObject>
#include <QtDBus/QDBusArgument>
#include <QtDBus/QDBusInterface>

struct roomEventId {
    QString roomId;
    QString eventId;
};

class NotificationManager : public QObject
{
    Q_OBJECT
public:
        NotificationManager(QObject *parent = nullptr);

        void sendMessage(const QString &roomid,
                         const QString &eventid,
                         const QString &roomname,
                         const QString &sender,
                         const QString &text,
                         const QImage &icon);

private:
        QDBusInterface dbus;
        void connectSlot(QString signal, const char* slot);
        uint showNotification(const QString summary, const QString text,
                const QImage image);

        // notification ID to (room ID, event ID)
        QMap<uint, roomEventId> notificationIds;

signals:
        void notificationClicked(const QString roomid,
                                 const QString eventid);

private slots:
        void actionInvoked(uint id,
                           QString action);
        void notificationClosed(uint id,
                                uint reason);
};

QDBusArgument& operator<<(QDBusArgument& arg, const QImage& image);
const QDBusArgument& operator>>(const QDBusArgument& arg, QImage&);

#endif // NOTIFICATIONMANAGER_H
