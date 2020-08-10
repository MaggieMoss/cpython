
#ifndef Py_OPTIONALTYPEOBJECT_H
#define Py_OPTIONALTYPEOBJECT_H
#ifdef __cplusplus
extern "C"
{
#endif

    PyAPI_FUNC(PyObject *) Py_Optional(PyObject *);
    PyAPI_DATA(PyTypeObject) Py_OptionalType;

#ifdef __cplusplus
}
#endif
#endif /* !Py_OPTIONALTYPEOBJECT_H */
