#include "Python.h"

#include "openssl/crypto.h"
#include "stdio.h"

static PyObject *IdCallback;
static PyObject *LockingCallback;


static void locking_callback_function(int mode, int n, const char *file, int line) {
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

  printf("Leave locking_function\n\n");
}

static unsigned long get_thread_id(void) {
    PyObject *arglist;
    PyObject *result;
    unsigned long  value;

    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();


    printf("Enter id_function\n");

    arglist = Py_BuildValue("()");
    result = PyObject_CallObject(IdCallback, arglist);

    printf("not breaking: %p\n", result);

    value = PyInt_AsUnsignedLongMask(result);
   // if (!PyArg_ParseTuple(result, "i", &value))
   //    return 0;

    printf("id got a value \n");
    printf("obtained id: %lu\n", value);

    Py_DECREF(arglist);
    Py_DECREF(result);

    PyGILState_Release(gstate);

    printf("Leave id_function\n");

    return ((unsigned long)value);
}


void threadid_callback_function(CRYPTO_THREADID* id) {
    printf("Enter threadid\n");
    CRYPTO_THREADID_set_numeric(id, (unsigned long) get_thread_id());
    printf("Leave threadid\n\n");
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

    CRYPTO_THREADID_set_callback(threadid_callback_function);
    CRYPTO_set_locking_callback(locking_callback_function);

    printf("Enabled mutexes\n");

    Py_RETURN_NONE;
}


static PyMethodDef threadConfigs[] = {
    {"enable_mutexes", enable_mutexes, METH_VARARGS,
     "Enable mutexes for openssl"},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};


PyMODINIT_FUNC initopenssl_thread_config(void) {
    if (! PyEval_ThreadsInitialized()) {
        PyEval_InitThreads();
    }

    (void) Py_InitModule("openssl_thread_config", threadConfigs);
}
