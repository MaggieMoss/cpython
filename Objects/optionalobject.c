// types.Optional -- used to represent typing.Optional[int] as ?int
#include "Python.h"
#include "pycore_object.h"
#include "structmember.h"


typedef struct {
    PyObject_HEAD
    PyObject *args;
} optionalobject;

static void
optionalobject_dealloc(PyObject *self)
{
    optionalobject *alias = (optionalobject *)self;

    _PyObject_GC_UNTRACK(self);
    Py_XDECREF(alias->args);
    self->ob_type->tp_free(self);
}

static Py_hash_t
optional_hash(PyObject *self)
{
    optionalobject *alias = (optionalobject *)self;
    Py_hash_t h1 = PyObject_Hash(alias->args);
    if (h1 == -1) {
        return -1;
    }
    return h1;
}

static int
optional_traverse(PyObject *self, visitproc visit, void *arg)
{
    optionalobject *alias = (optionalobject *)self;
    Py_VISIT(alias->args);
    return 0;
}

static PyMemberDef optional_members[] = {
    {"__args__", T_OBJECT, offsetof(optionalobject, args), READONLY},
    {0}
};

static PyObject *
optional_getattro(PyObject *self, PyObject *name)
{
    return PyObject_GenericGetAttr(self, name);
}

static PyObject *
optional_instancecheck(PyObject *self, PyObject *instance)
{
    // TODO: MM: Implement this.
    return instance;
}

static PyObject *
optional_subclasscheck(PyObject *self, PyObject *instance)
{
    // TODO: MM: Implement this.
    return instance;
}

static PyMethodDef optional_methods[] = {
    {"__instancecheck__", optional_instancecheck, METH_O},
    {"__subclasscheck__", optional_subclasscheck, METH_O},
    {0}};


static PyObject *
optional_richcompare(PyObject *a, PyObject *b, int op)
{
    return a;
}


static int
optional_repr_item(_PyUnicodeWriter *writer, PyObject *p)
{
    _Py_IDENTIFIER(__module__);
    _Py_IDENTIFIER(__qualname__);
    _Py_IDENTIFIER(__origin__);
    _Py_IDENTIFIER(__args__);
    PyObject *qualname = NULL;
    PyObject *module = NULL;
    PyObject *r = NULL;
    PyObject *tmp;
    int err;

    if (p == Py_Ellipsis) {
        // The Ellipsis object
        r = PyUnicode_FromString("...");
        goto done;
    }

    if (_PyObject_LookupAttrId(p, &PyId___origin__, &tmp) < 0) {
        goto done;
    }
    if (tmp != NULL) {
        Py_DECREF(tmp);
        if (_PyObject_LookupAttrId(p, &PyId___args__, &tmp) < 0) {
            goto done;
        }
        if (tmp != NULL) {
            Py_DECREF(tmp);
            // It looks like a GenericAlias
            goto use_repr;
        }
    }

    if (_PyObject_LookupAttrId(p, &PyId___qualname__, &qualname) < 0) {
        goto done;
    }
    if (qualname == NULL) {
        goto use_repr;
    }
    if (_PyObject_LookupAttrId(p, &PyId___module__, &module) < 0) {
        goto done;
    }
    if (module == NULL || module == Py_None) {
        goto use_repr;
    }

    // Looks like a class
    if (PyUnicode_Check(module) &&
        _PyUnicode_EqualToASCIIString(module, "builtins"))
    {
        // builtins don't need a module name
        r = PyObject_Str(qualname);
        goto done;
    }
    else {
        r = PyUnicode_FromFormat("%S.%S", module, qualname);
        goto done;
    }

use_repr:
    r = PyObject_Repr(p);

done:
    Py_XDECREF(qualname);
    Py_XDECREF(module);
    if (r == NULL) {
        // error if any of the above PyObject_Repr/PyUnicode_From* fail
        err = -1;
    }
    else {
        err = _PyUnicodeWriter_WriteStr(writer, r);
        Py_DECREF(r);
    }
    return err;
}

static PyObject *
optional_repr(PyObject *self)
{
    optionalobject *alias = (optionalobject *)self;
    _PyUnicodeWriter writer;
    _PyUnicodeWriter_Init(&writer);
    if (_PyUnicodeWriter_WriteASCIIString(&writer, "?", 1) < 0) {
        goto error;
    }
    if (optional_repr_item(&writer, alias->args) < 0) {
        goto error;
    }
    return _PyUnicodeWriter_Finish(&writer);

error:
    _PyUnicodeWriter_Dealloc(&writer);
    return NULL;
}

PyTypeObject Py_OptionalType = {
    PyVarObject_HEAD_INIT(&PyType_Type, 0)
    .tp_name = "types.Optional",
    .tp_doc = "Represent an Optional type\n"
              "\n"
              "E.g. ?int",
    .tp_basicsize = sizeof(optionalobject),
    .tp_dealloc = optionalobject_dealloc,
    .tp_alloc = PyType_GenericAlloc,
    .tp_free = PyObject_GC_Del,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC,
    .tp_hash = optional_hash,
    .tp_traverse = optional_traverse,
    .tp_getattro = optional_getattro,
    .tp_members = optional_members,
    .tp_methods = optional_methods,
    .tp_richcompare = optional_richcompare,
    .tp_repr = optional_repr,
};

PyObject *
Py_Optional(PyObject *args)
{
    optionalobject *alias = PyObject_GC_New(optionalobject, &Py_OptionalType);
    if (alias == NULL) {
        Py_DECREF(args);
        return NULL;
    }

    alias->args = args;
    _PyObject_GC_TRACK(alias);
    return (PyObject *) alias;
}
