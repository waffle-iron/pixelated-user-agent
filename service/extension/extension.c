#include "Python.h"

#include "openssl/crypto.h"
#include "stdio.h"

static PyObject *IdCallback;
static PyObject *LockingCallback;


static void locking_function(int mode, int n, const char *file, int line) {
  PyObject *arglist;
  PyObject *result;

  PyGILState_STATE gstate;
  gstate = PyGILState_Ensure();

  printf("Enter locking_function\n");
  printf("mode %i n %i file %s und line %i \n", mode, n, file, line);
  arglist = Py_BuildValue("iisi", mode, n, file, line);
  printf("did not break on build arglist: %p, %p\n", LockingCallback, arglist);
  result = PyObject_CallObject(LockingCallback, arglist);
  printf("result: %p\n", result);

  Py_DECREF(arglist);
  Py_DECREF(result);

  PyGILState_Release(gstate);

  printf("Leave locking_function\n");
}


static unsigned long id_function(void) {
    PyObject *arglist;
    PyObject *result;
    int value;

    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();


    printf("Enter id_function\n");

    arglist = Py_BuildValue(NULL);
    result = PyObject_CallObject(IdCallback, arglist);

    if (!PyArg_ParseTuple(result, "i", &value))
       return 0;

    Py_DECREF(arglist);
    Py_DECREF(result);

    PyGILState_Release(gstate);

    return ((unsigned long)value);
}


static PyObject * enable_mutexes(PyObject *self, PyObject *args) {
    PyObject *pIdCallback, *pLockingCallback;

    if (!PyArg_UnpackTuple(args, "enable_mutexes", 2, 2, &pIdCallback, &pLockingCallback)) {
		return NULL;
	}
	Py_INCREF(pIdCallback);
	Py_INCREF(pLockingCallback);
	IdCallback = pIdCallback;
	LockingCallback = pLockingCallback;

    CRYPTO_set_id_callback(id_function);
    CRYPTO_set_locking_callback(locking_function);

    printf("Enabled mutexes\n");

    Py_RETURN_NONE;
}


static PyMethodDef SpamMethods[] = {
    {"enable_mutexes", enable_mutexes, METH_VARARGS,
     "Enable mutexes for openssl"},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};


PyMODINIT_FUNC initfoobar(void) {
    if (! PyEval_ThreadsInitialized()) {
        PyEval_InitThreads();
    }

    (void) Py_InitModule("foobar", SpamMethods);
}
