/*
 *  Manjaro Settings Manager
 *  Ramon Buldó <ramon@manjaro.org>
 *
 *  Copyright (C) 2007 Free Software Foundation, Inc.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "KernelModel.h"

#include <QtCore/QElapsedTimer>
#include <QtCore/QRegularExpression>
#include <QtCore/QSet>
#include <QtCore/QProcess>
#include <QDebug>

KernelModel::KernelModel( QObject* parent )
    : QAbstractListModel( parent )
{
}


void
KernelModel::update()
{
    QHash<QString, QString> installedKernelPackages = getInstalledPackages();
    QHash<QString, QString> availableKernelPackages = getAvailablePackages();

    QHash<QString, QString> allKernelPackages;
    allKernelPackages.unite( installedKernelPackages );
    QHashIterator<QString, QString> i( availableKernelPackages );
    while ( i.hasNext() )
    {
        i.next();
        allKernelPackages.insert( i.key(), i.value() );
    }

    QString runningKernel = getRunningKernel();
    QStringList ltsKernels = getLtsKernels();
    QStringList recommendedKernels = getRecommendedKernels();

    QSet<QString> modulesToInstall;
    for ( const QString& module : QStringList( installedKernelPackages.keys() ).filter( QRegularExpression( "^linux[0-9][0-9]?([0-9])-" ) ) )
    {
        QString aux = QString( module ).remove( QRegularExpression( "^linux[0-9][0-9]?([0-9])-" ) );
        modulesToInstall.insert( aux );
    }

    beginResetModel();
    m_kernels.clear();
    for ( const QString& kernel : QStringList( allKernelPackages.keys() ).filter( QRegularExpression( "^linux[0-9][0-9]?([0-9])$" ) ) )
    {
        Kernel newKernel;

        newKernel.setPackage( kernel );
        newKernel.setAvailable( availableKernelPackages.contains( kernel ) );
        newKernel.setInstalled( installedKernelPackages.contains( kernel ) );
        newKernel.setVersion( allKernelPackages.value( kernel ) );
        newKernel.setAvailableModules( QStringList( availableKernelPackages.keys() )
                                       .filter( QRegularExpression( QString( "^%1-" ).arg( kernel ) ) ) );
        if ( newKernel.isInstalled() )
        {
            newKernel.setInstalledModules( QStringList( installedKernelPackages.keys() )
                                           .filter( QRegularExpression( QString( "^%1-" ).arg( kernel ) ) ) );
        }
        else
        {
            QStringList installableModules;
            for ( const QString& module : modulesToInstall )
            {
                QString modulePackage = QString( "%1-%2" ).arg( kernel ).arg( module );
                if ( availableKernelPackages.contains( modulePackage ) )
                    installableModules << modulePackage;
            }
            newKernel.setInstalledModules( installableModules );
        }
        newKernel.setLts( ltsKernels.contains( kernel ) );
        newKernel.setRecommended( recommendedKernels.contains( kernel ) );
        newKernel.setRunning( QString::compare( runningKernel, kernel ) == 0 );

        m_kernels.append( newKernel );
    }
    endResetModel();
}


void
KernelModel::add( const Kernel& kernel )
{
    beginInsertRows( QModelIndex(), rowCount(), rowCount() );
    m_kernels << kernel;
    endInsertRows();
}


bool
KernelModel::remove( int position, int rows, const QModelIndex& parent )
{
    Q_UNUSED( parent )
    beginRemoveRows( QModelIndex(), position, position+rows-1 );
    for ( int row = 0; row < rows; ++row )
        m_kernels.removeAt( position );
    endRemoveRows();
    return true;
}


int
KernelModel::rowCount( const QModelIndex& parent ) const
{
    Q_UNUSED( parent );
    return m_kernels.size();
}


QVariant
KernelModel::data( const QModelIndex& index, int role ) const
{
    if ( index.row() < 0 || index.row() >= m_kernels.count() )
        return QVariant();
    const Kernel& kernel = m_kernels[index.row()];
    switch ( role )
    {
    case PackageRole:
        return kernel.package();
    case VersionRole:
        return kernel.version();
    case MajorVersionRole:
        return kernel.majorVersion();
    case MinorVersionRole:
        return kernel.minorVersion();
    case InstalledModulesRole:
        return kernel.installedModules();
    case AvailableModulesRole:
        return kernel.availableModules();
    case IsAvailableRole:
        return kernel.isAvailable();
    case IsInstalledRole:
        return kernel.isInstalled();
    case IsLtsRole:
        return kernel.isLts();
    case IsRecommendedRole:
        return kernel.isRecommended();
    case IsRunningRole:
        return kernel.isRunning();
    case IsUnsupportedRole:
        return kernel.isUnsupported();
    }
    return QVariant();
}


QHash<int, QByteArray>
KernelModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[PackageRole] = "package";
    roles[VersionRole] = "version";
    roles[MajorVersionRole] = "majorVersion";
    roles[MinorVersionRole] = "minorVersion";
    roles[InstalledModulesRole] = "installedModules";
    roles[AvailableModulesRole] = "availableModules";
    roles[IsAvailableRole] = "isAvailable";
    roles[IsInstalledRole] = "isInstalled";
    roles[IsLtsRole] = "isLts";
    roles[IsRecommendedRole] = "isRecommended";
    roles[IsRunningRole] = "isRunning";
    return roles;
}


QHash<QString, QString>
KernelModel::getAvailablePackages() const
{
    QProcess process;
    process.setEnvironment( QStringList() << "LANG=C" << "LC_MESSAGES=C" );
    process.start( "pacman", QStringList() << "-Ss" << "^linux[0-9][0-9]?([0-9])" );
    if ( !process.waitForFinished( 15000 ) )
        qDebug() << "error: failed to get all installed kernels";
    QString result = process.readAll();

    QHash<QString, QString> packages;
    for ( QString line : result.split( "\n", QString::SkipEmptyParts ) )
    {
        if ( line.isEmpty() )
            continue;
        if ( line[0].isSpace() )
            continue;
        QStringList parts = line.split( ' ' );
        QString repoName = parts.value( 0 );
        int a = repoName.indexOf( "/" );
        QString pkgName = repoName.mid( a+1 );
        QString pkgVersion = parts.value( 1 );
        packages.insert( pkgName, pkgVersion );
    }
    return packages;
}


QHash<QString, QString>
KernelModel::getInstalledPackages() const
{
    QProcess process;
    process.setEnvironment( QStringList() << "LANG=C" << "LC_MESSAGES=C" );
    process.start( "pacman", QStringList() << "-Qs" << "^linux[0-9][0-9]?([0-9])" );
    if ( !process.waitForFinished( 15000 ) )
        qDebug() << "error: failed to get all installed kernels";
    QString result = process.readAll();

    QHash<QString, QString> packages;
    for ( QString line : result.split( "\n", QString::SkipEmptyParts ) )
    {
        if ( line.isEmpty() )
            continue;
        if ( line[0].isSpace() )
            continue;
        QStringList parts = line.split( ' ' );
        QString repoName = parts.value( 0 );
        int a = repoName.indexOf( "/" );
        QString pkgName = repoName.mid( a+1 );
        QString pkgVersion = parts.value( 1 );
        packages.insert( pkgName, pkgVersion );
    }
    return packages;
}


Kernel
KernelModel::latestInstalledKernel()
{
    Kernel auxKernel;
    for ( Kernel& kernel : m_kernels )
    {
        if ( !kernel.isInstalled() )
            continue;
        if ( kernel.majorVersion() > auxKernel.majorVersion() )
            auxKernel = kernel;
        else if ( ( kernel.majorVersion() == auxKernel.majorVersion() )
                  && ( kernel.minorVersion() > auxKernel.minorVersion() ) )
            auxKernel = kernel;
    }
    return auxKernel;
}


/**
 * @brief KernelModel::unsupportedKernels Check m_kernels for unsupported kernels
 * (installed but not available in the repositories anymore).
 * @return QList of unsupported kernels
 */
QList<Kernel>
KernelModel::unsupportedKernels() const
{
    QList<Kernel> auxList;
    for ( const Kernel& kernel : m_kernels )
    {
        if ( kernel.isUnsupported() )
            auxList << kernel;
    }
    return auxList;
}


/**
 * @brief KernelModel::newerKernels Returns a list of all kernels with a higher
 * version than the kernel supplied and available in the repos
 * @param auxKernel a kernel
 * @return QList of kernels with higher version comapre to auxKernel
 */
QList<Kernel>
KernelModel::newerKernels( const Kernel& auxKernel )
{
    QList<Kernel> auxList;
    for ( Kernel& kernel : m_kernels )
    {
        if ( !kernel.isAvailable() )
            continue;
        if ( kernel.majorVersion() > auxKernel.majorVersion() )
            auxList << kernel;
        else if ( ( kernel.majorVersion() == auxKernel.majorVersion() )
                  && ( kernel.minorVersion() > auxKernel.minorVersion() ) )
            auxList << kernel;
    }
    return auxList;
}


/**
 * @brief KernelModel::getRunningKernel get running kernel in the system
 * @return string with the version of running kernel
 */
QString
KernelModel::getRunningKernel() const
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert( "LANG", "C" );
    env.insert( "LC_MESSAGES", "C" );
    env.insert( "LC_ALL", "C" );

    QProcess uname;
    uname.setProcessEnvironment( env );

    uname.start( "uname", QStringList() << "-r" );
    uname.waitForFinished();
    QString result = uname.readAllStandardOutput();
    uname.close();
    QStringList aux = result.split( ".", QString::SkipEmptyParts );
    return QString( "linux%1%2" ).arg( aux.at( 0 ) ).arg( aux.at( 1 ) );
}


QStringList
KernelModel::getLtsKernels() const
{
    return QStringList() << "linux310" << "linux312" << "linux314" << "linux318" << "linux41" << "linux44";
}


QStringList
KernelModel::getRecommendedKernels() const
{
    return QStringList() << "linux44";
}



/* KernelSortFilterProxyModel class */
KernelSortFilterProxyModel::KernelSortFilterProxyModel( QObject* parent )
    : QSortFilterProxyModel( parent )
{
}


bool
KernelSortFilterProxyModel::lessThan( const QModelIndex& left,
                                      const QModelIndex& right ) const
{
    QVariant leftData = sourceModel()->data( left, sortRole() );
    QVariant rightData = sourceModel()->data( right, sortRole() );

    if ( sortRole() == KernelModel::VersionRole )
    {
        int leftMajor = sourceModel()->data( left, KernelModel::MajorVersionRole ).toInt();
        int rightMajor = sourceModel()->data( right, KernelModel::MajorVersionRole ).toInt();
        int leftMinor = sourceModel()->data( left, KernelModel::MinorVersionRole ).toInt();
        int rightMinor = sourceModel()->data( right, KernelModel::MinorVersionRole ).toInt();

        if ( leftMajor == rightMajor )
        {
            if ( leftMinor < rightMinor )
                return true;
            else
                return false;
        }
        else
        {
            if ( leftMajor < rightMajor )
                return true;
            else
                return false;
        }
    }
    else
        return QString::localeAwareCompare( leftData.toString(), rightData.toString() ) < 0;
}
