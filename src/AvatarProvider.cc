/*
 * nheko Copyright (C) 2017  Konstantinos Sideris <siderisk@auth.gr>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "AvatarProvider.h"
#include "MatrixClient.h"
#include "TimelineItem.h"

QSharedPointer<MatrixClient> AvatarProvider::client_;

QMap<QString, QImage> AvatarProvider::userAvatars_;
QMap<QString, QUrl> AvatarProvider::avatarUrls_;
QMap<QString, QList<TimelineItem *>> AvatarProvider::toBeResolved_;

void
AvatarProvider::init(QSharedPointer<MatrixClient> client)
{
        client_ = client;

        connect(client_.data(), &MatrixClient::userAvatarRetrieved, &AvatarProvider::updateAvatar);
}

void
AvatarProvider::updateAvatar(const QString &uid, const QImage &img)
{
        if (toBeResolved_.contains(uid)) {
                auto items = toBeResolved_[uid];

                // Update all the timeline items with the resolved avatar.
                for (const auto item : items)
                        item->setUserAvatar(img);

                toBeResolved_.remove(uid);
        }

        userAvatars_.insert(uid, img);
}

void
AvatarProvider::resolve(const QString &userId, TimelineItem *item)
{
        if (userAvatars_.contains(userId)) {
                auto img = userAvatars_[userId];

                item->setUserAvatar(img);

                return;
        }

        if (avatarUrls_.contains(userId)) {
                // Add the current timeline item to the waiting list for this avatar.
                if (!toBeResolved_.contains(userId)) {
                        client_->fetchUserAvatar(userId, avatarUrls_[userId]);

                        QList<TimelineItem *> timelineItems;
                        timelineItems.push_back(item);

                        toBeResolved_.insert(userId, timelineItems);
                } else {
                        toBeResolved_[userId].push_back(item);
                }
        }
}

void
AvatarProvider::setAvatarUrl(const QString &userId, const QUrl &url)
{
        avatarUrls_.insert(userId, url);
}

void
AvatarProvider::clear()
{
        userAvatars_.clear();
        avatarUrls_.clear();
        toBeResolved_.clear();
}
