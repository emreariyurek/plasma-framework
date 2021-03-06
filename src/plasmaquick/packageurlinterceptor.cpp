/*
 *   Copyright 2013 Marco Martin <notmart@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "packageurlinterceptor.h"

#include <QDebug>
#include <QQmlEngine>
#include <QFile>
#include <QFileInfo>
#include <QFileSelector>
#include <QStandardPaths>
#include <QRegularExpression>

#include <Plasma/PluginLoader>
#include <Plasma/Package>
#include <KPackage/Package>

#include <kdeclarative/kdeclarative.h>

namespace PlasmaQuick
{

class PackageUrlInterceptorPrivate {
public:
    PackageUrlInterceptorPrivate(QQmlEngine *engine, PackageUrlInterceptor *interceptor, const KPackage::Package &p)
        : q(interceptor),
          package(p),
          engine(engine)
    {
        selector = new QFileSelector;
    }

    ~PackageUrlInterceptorPrivate()
    {
        engine->setUrlInterceptor(nullptr);
        delete selector;
    }

    PackageUrlInterceptor *q;
    KPackage::Package package;
    QStringList allowedPaths;
    QQmlEngine *engine;
    QFileSelector *selector;
    bool forcePlasmaStyle = false;
};


PackageUrlInterceptor::PackageUrlInterceptor(QQmlEngine *engine, const KPackage::Package &p)
    : QQmlAbstractUrlInterceptor(),
      d(new PackageUrlInterceptorPrivate(engine, this, p))
{
    //d->allowedPaths << d->engine->importPathList();
}

PackageUrlInterceptor::~PackageUrlInterceptor()
{
    delete d;
}

void PackageUrlInterceptor::addAllowedPath(const QString &path)
{
    d->allowedPaths << path;
}

void PackageUrlInterceptor::removeAllowedPath(const QString &path)
{
    d->allowedPaths.removeAll(path);
}

QStringList PackageUrlInterceptor::allowedPaths() const
{
    return d->allowedPaths;
}

bool PackageUrlInterceptor::forcePlasmaStyle() const
{
    return d->forcePlasmaStyle;
}

void PackageUrlInterceptor::setForcePlasmaStyle(bool force)
{
    d->forcePlasmaStyle = force;
}

QUrl PackageUrlInterceptor::intercept(const QUrl &path, QQmlAbstractUrlInterceptor::DataType type)
{
    //qDebug() << "Intercepted URL:" << path << type;

    // Don't intercept qmldir files, to prevent double interception
    if (path.path().endsWith(QStringLiteral("qmldir"))) {
        return path;
    }
    // We assume we never rewritten qml/qmldir files
    if (path.path().endsWith(QLatin1String("qml"))
            || path.path().endsWith(QLatin1String("/inline"))) {
        return d->selector->select(path);
    }
    const QString prefix = QString::fromUtf8(prefixForType(type, path.path()));
    QRegularExpression match(QStringLiteral("/ui/(.*)"));
    // TODO KF6: Kill this hack
    QString plainPath = path.toString();
    if (plainPath.contains(match)) {
        QString rewritten = plainPath.replace(match, QStringLiteral("/%1/\\1").arg(prefix));

        //search it in a resource or as a file on disk
        if (!(rewritten.contains(QLatin1String("qrc")) &&
              QFile::exists(QStringLiteral(":") + QUrl(rewritten).path())) &&
            !QFile::exists(QUrl(rewritten).path())) {
            return d->selector->select(path);
        }
        qWarning()<<"Warning: all files used by qml by the plasmoid should be in ui/. The file in the path" << rewritten << "was expected at" <<path;
        // This deprecated code path doesn't support selectors
        return QUrl(rewritten);
    }
    return d->selector->select(path);
}

}

