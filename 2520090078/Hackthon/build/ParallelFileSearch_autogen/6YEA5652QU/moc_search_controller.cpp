/****************************************************************************
** Meta object code from reading C++ file 'search_controller.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../include/search_controller.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'search_controller.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN3pfs16SearchControllerE_t {};
} // unnamed namespace

template <> constexpr inline auto pfs::SearchController::qt_create_metaobjectdata<qt_meta_tag_ZN3pfs16SearchControllerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "pfs::SearchController",
        "searchStarted",
        "",
        "progressUpdated",
        "pfs::SearchStats",
        "stats",
        "batchResultsFound",
        "std::vector<pfs::SearchResult>",
        "results",
        "singleResultFound",
        "pfs::SearchResult",
        "result",
        "searchFinished",
        "cancelled",
        "logMessageReceived",
        "pfs::LogEntry",
        "entry",
        "errorOccurred",
        "errorMessage",
        "startSearch",
        "pfs::SearchOptions",
        "options",
        "cancelSearch",
        "clearSearch",
        "exportResultsToCsv",
        "filePath",
        "QString&",
        "outError",
        "exportLogsToFile"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'searchStarted'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'progressUpdated'
        QtMocHelpers::SignalData<void(const pfs::SearchStats &)>(3, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 4, 5 },
        }}),
        // Signal 'batchResultsFound'
        QtMocHelpers::SignalData<void(const std::vector<pfs::SearchResult> &)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 7, 8 },
        }}),
        // Signal 'singleResultFound'
        QtMocHelpers::SignalData<void(const pfs::SearchResult &)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 10, 11 },
        }}),
        // Signal 'searchFinished'
        QtMocHelpers::SignalData<void(bool, const pfs::SearchStats &)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 13 }, { 0x80000000 | 4, 5 },
        }}),
        // Signal 'logMessageReceived'
        QtMocHelpers::SignalData<void(const pfs::LogEntry &)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 15, 16 },
        }}),
        // Signal 'errorOccurred'
        QtMocHelpers::SignalData<void(const QString &)>(17, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 18 },
        }}),
        // Slot 'startSearch'
        QtMocHelpers::SlotData<void(const pfs::SearchOptions &)>(19, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 20, 21 },
        }}),
        // Slot 'cancelSearch'
        QtMocHelpers::SlotData<void()>(22, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'clearSearch'
        QtMocHelpers::SlotData<void()>(23, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'exportResultsToCsv'
        QtMocHelpers::SlotData<bool(const QString &, QString &)>(24, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 25 }, { 0x80000000 | 26, 27 },
        }}),
        // Slot 'exportLogsToFile'
        QtMocHelpers::SlotData<bool(const QString &, QString &)>(28, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 25 }, { 0x80000000 | 26, 27 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<SearchController, qt_meta_tag_ZN3pfs16SearchControllerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject pfs::SearchController::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN3pfs16SearchControllerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN3pfs16SearchControllerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN3pfs16SearchControllerE_t>.metaTypes,
    nullptr
} };

void pfs::SearchController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<SearchController *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->searchStarted(); break;
        case 1: _t->progressUpdated((*reinterpret_cast<std::add_pointer_t<pfs::SearchStats>>(_a[1]))); break;
        case 2: _t->batchResultsFound((*reinterpret_cast<std::add_pointer_t<std::vector<pfs::SearchResult>>>(_a[1]))); break;
        case 3: _t->singleResultFound((*reinterpret_cast<std::add_pointer_t<pfs::SearchResult>>(_a[1]))); break;
        case 4: _t->searchFinished((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<pfs::SearchStats>>(_a[2]))); break;
        case 5: _t->logMessageReceived((*reinterpret_cast<std::add_pointer_t<pfs::LogEntry>>(_a[1]))); break;
        case 6: _t->errorOccurred((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->startSearch((*reinterpret_cast<std::add_pointer_t<pfs::SearchOptions>>(_a[1]))); break;
        case 8: _t->cancelSearch(); break;
        case 9: _t->clearSearch(); break;
        case 10: { bool _r = _t->exportResultsToCsv((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString&>>(_a[2])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 11: { bool _r = _t->exportLogsToFile((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString&>>(_a[2])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< pfs::SearchStats >(); break;
            }
            break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< std::vector<pfs::SearchResult> >(); break;
            }
            break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< pfs::SearchResult >(); break;
            }
            break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 1:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< pfs::SearchStats >(); break;
            }
            break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< pfs::LogEntry >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (SearchController::*)()>(_a, &SearchController::searchStarted, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (SearchController::*)(const pfs::SearchStats & )>(_a, &SearchController::progressUpdated, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (SearchController::*)(const std::vector<pfs::SearchResult> & )>(_a, &SearchController::batchResultsFound, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (SearchController::*)(const pfs::SearchResult & )>(_a, &SearchController::singleResultFound, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (SearchController::*)(bool , const pfs::SearchStats & )>(_a, &SearchController::searchFinished, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (SearchController::*)(const pfs::LogEntry & )>(_a, &SearchController::logMessageReceived, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (SearchController::*)(const QString & )>(_a, &SearchController::errorOccurred, 6))
            return;
    }
}

const QMetaObject *pfs::SearchController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *pfs::SearchController::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN3pfs16SearchControllerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int pfs::SearchController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void pfs::SearchController::searchStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void pfs::SearchController::progressUpdated(const pfs::SearchStats & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void pfs::SearchController::batchResultsFound(const std::vector<pfs::SearchResult> & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void pfs::SearchController::singleResultFound(const pfs::SearchResult & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void pfs::SearchController::searchFinished(bool _t1, const pfs::SearchStats & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1, _t2);
}

// SIGNAL 5
void pfs::SearchController::logMessageReceived(const pfs::LogEntry & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}

// SIGNAL 6
void pfs::SearchController::errorOccurred(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1);
}
QT_WARNING_POP
