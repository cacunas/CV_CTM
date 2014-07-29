/****************************************************************************
** Meta object code from reading C++ file 'apply_display.h'
**
** Created: Wed Dec 9 16:48:18 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "apply_display.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'apply_display.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ApplyDialog[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      16,   13,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
      34,   12,   12,   12, 0x08,
      57,   12,   12,   12, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ApplyDialog[] = {
    "ApplyDialog\0\0id\0dialogClosed(int)\0"
    "on_saveImage_clicked()\0on_next_clicked()\0"
};

const QMetaObject ApplyDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_ApplyDialog,
      qt_meta_data_ApplyDialog, 0 }
};

const QMetaObject *ApplyDialog::metaObject() const
{
    return &staticMetaObject;
}

void *ApplyDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ApplyDialog))
        return static_cast<void*>(const_cast< ApplyDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int ApplyDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: dialogClosed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: on_saveImage_clicked(); break;
        case 2: on_next_clicked(); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void ApplyDialog::dialogClosed(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
