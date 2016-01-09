#include "util/version.h"

#include <QCoreApplication>
#include <QDesktopServices>
#include <QStringList>
#include <QtDebug>
#include <QtGlobal>

// Fixes redefinition warnings from SoundTouch.
#undef TRUE
#undef FALSE
#include <SoundTouch.h>

// shout.h checks for WIN32 to see if we are on Windows.
#ifdef WIN64
#define WIN32
#endif
#ifdef __SHOUTCAST__
#include <shout/shout.h>
#endif
#ifdef WIN64
#undef WIN32
#endif

#include <FLAC/format.h>
#include <chromaprint.h>
#include <rubberband/RubberBandStretcher.h>
#include <taglib/taglib.h>
#include <vorbis/codec.h>
#ifdef __PORTAUDIO__
#include <portaudio.h>
#endif

#include "defs_version.h"
#include "build.h" // Generated by SCons

// static
QString Version::version() {
    return VERSION;
}

// static
QString Version::applicationName() {
    return "Mixxx";
}

// static
QString Version::applicationTitle() {
#ifdef __APPLE__
    QString base("Mixxx");
#elif defined(AMD64) || defined(EM64T) || defined(x86_64)
    QString base("Mixxx " VERSION " x64");
#elif defined(IA64)
    QString base("Mixxx " VERSION " Itanium");
#else
    QString base("Mixxx " VERSION);
#endif

#ifdef MIXXX_BUILD_NUMBER_IN_TITLE_BAR
    QString branch = developmentBranch();
    QString branch_revision = developmentRevision();
    if (!branch.isEmpty() && !branch_revision.isEmpty()) {
        base.append(QString(" (build %1-r%2)")
                    .arg(branch).arg(branch_revision));
    }
#endif
    return base;
}

// static
QString Version::developmentBranch() {
#ifdef BUILD_BRANCH
    return BUILD_BRANCH;
#else
    return QString();
#endif
}

// static
QString Version::developmentRevision() {
#ifdef BUILD_REV
    return BUILD_REV;
#else
    return QString();
#endif
}

// static
QString Version::buildFlags() {
#ifdef BUILD_FLAGS
    return BUILD_FLAGS;
#else
    return QString();
#endif
}

QStringList Version::dependencyVersions() {
    // WARNING: may be inaccurate since some come from compile-time header
    // definitions instead of the actual dynamically loaded library).
    QStringList result;
    result
            // Should be accurate.
            << QString("Qt: %1").arg(qVersion())
#ifdef __SHOUTCAST__
            // Should be accurate.
            << QString("libshout: %1").arg(shout_version(NULL, NULL, NULL))
#endif
#ifdef __PORTAUDIO__
            << QString("PortAudio: %1 %2").arg(Pa_GetVersion()).arg(Pa_GetVersionText())
#endif
            // The version of the RubberBand headers Mixxx was compiled with.
            << QString("RubberBand: %1").arg(RUBBERBAND_VERSION)
            // The version of the SoundTouch headers Mixxx was compiled with.
            << QString("SoundTouch: %1").arg(SOUNDTOUCH_VERSION)
            // The version of the TagLib headers Mixxx was compiled with.
            << QString("TagLib: %1.%2.%3").arg(QString::number(TAGLIB_MAJOR_VERSION),
                                               QString::number(TAGLIB_MINOR_VERSION),
                                               QString::number(TAGLIB_PATCH_VERSION))
            // The version of the ChromaPrint headers Mixxx was compiled with.
            << QString("ChromaPrint: %1.%2.%3").arg(QString::number(CHROMAPRINT_VERSION_MAJOR),
                                                    QString::number(CHROMAPRINT_VERSION_MINOR),
                                                    QString::number(CHROMAPRINT_VERSION_PATCH))
            // Should be accurate.
            << QString("Vorbis: %1").arg(vorbis_version_string())
            // The version of the FLAC headers Mixxx was compiled with.
            << QString("FLAC: %1").arg(FLAC__VERSION_STRING);

    return result;
}

void Version::logBuildDetails() {
    QString version = Version::version();
    QString buildBranch = developmentBranch();
    QString buildRevision = developmentRevision();
    QString buildFlags = Version::buildFlags();

    QStringList buildInfo;
    if (!buildBranch.isEmpty() && !buildRevision.isEmpty()) {
        buildInfo.append(
            QString("git %1 r%2").arg(buildBranch, buildRevision));
    } else if (!buildRevision.isEmpty()) {
        buildInfo.append(
            QString("git r%2").arg(buildRevision));
    }
#ifndef DISABLE_BUILDTIME // buildtime=1, on by default
    buildInfo.append("built on: " __DATE__ " @ " __TIME__);
#endif
    if (!buildFlags.isEmpty()) {
        buildInfo.append(QString("flags: %1").arg(buildFlags.trimmed()));
    }
    QString buildInfoFormatted = QString("(%1)").arg(buildInfo.join("; "));

    // This is the first line in mixxx.log
    qDebug() << applicationName() << version << buildInfoFormatted << "is starting...";

    QStringList depVersions = dependencyVersions();
    qDebug() << "Library versions:";
    foreach (const QString& depVersion, depVersions) {
        qDebug() << qPrintable(depVersion);
    }

    qDebug() << "QDesktopServices::storageLocation(HomeLocation):"
             << QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
    qDebug() << "QDesktopServices::storageLocation(DataLocation):"
             << QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    qDebug() << "QCoreApplication::applicationDirPath()"
             << QCoreApplication::applicationDirPath();
}
