#define PYW_SSIZE_T_CLEAN
#include <Python.h>

#include "include/common.h"
#include "include/pydef.h"


static PyObject* randbits(PyObject* self, PyObject* args) {
    
    int k, i, words;
    uint64_t randf;
    uint32_t *wordarray;
    PyObject *result;


    if (!PyArg_ParseTuple(args, "i", &k))
        return NULL;

    if (k < 0) {
        PyGILState_STATE gstate = PyGILState_Ensure();
        PyErr_SetString(PyExc_ValueError, "Number of bits must be non-negative.\n");
        PyGILState_Release(gstate);

        return NULL;
    }

    if (k == 0)
        return PyLong_FromLong(0);

    if (k <= 32) {
        generate_rdrand64(&randf);
        return PyLong_FromUnsignedLong(abs((uint32_t) randf) >> (32 - k));
    }

    words = (k - 1) / 32 + 1;
    wordarray = (uint32_t *) PyMem_Malloc(words * 4);
    
    if (wordarray == NULL) {
        PyErr_NoMemory();
        return NULL;
    }

    uint32_t temp;
#if PY_LITTLE_ENDIAN
    for (i = 0; i < words; i++, k -= 32)
#else
    for (i = words -1; i >= 0; i--, k-= 32)
#endif
    {
        generate_rdrand64(&randf);
        temp = (uint32_t) randf;
        if (temp < 32)
            temp >>= (32 - k);
        wordarray[i] = temp;
    }
    result = _PyLong_FromByteArray((unsigned char*)wordarray, words * 4, PY_LITTLE_ENDIAN, 0);
    PyMem_Free(wordarray);
    return result;
}

/*
static PyObject* randint(PyObject* self, PyObject *args) {
    uint64_t randf;
    int a, b, n;
    int width;
    int step = 1;

    if (!PyArg_ParseTuple(args, "ii", &a, &b)) {
        return NULL;
    }

    if (a > b) {
        PyGILState_STATE gstate = PyGILState_Ensure();
        PyErr_SetString(PyExc_ValueError, "Minimum value is greater than max.\n");
        PyGILState_Release(gstate);

        return NULL;
    }

    if (a < 0 && b < 0) {
        PyGILState_STATE gstate = PyGILState_Ensure();
        PyErr_SetString(PyExc_ValueError, "Negative range isn't supported.");
        PyGILState_Release(gstate);

        return NULL;
    }

    width = b - a;
    n = (width + step - 1) / step;

    if (n <= 0) {
        PyGILState_STATE gstate = PyGILState_Ensure();
        PyErr_SetString(PyExc_ValueError, "empty range for randint.");
        PyGILState_Release(gstate);

        return NULL;
    }

    max++;

    while (1) {
        if (!generate_rdrand64(&randf)) {


            if ((int)randf >= min && (int)randf <= max)
                break;

        } else {
            perror("Failed to get random value.");
            exit(2);
        }
    }

    // if (min < 0 && max < 0 && randf > 0)
    //     randf = ~randf + 1;

    return (PyObject*) PyLong_FromLong(0);

}*/

static PyListObject* n_range_below(PyObject* self, PyObject* args) {

    int length, below, amount;

    if (!PyArg_ParseTuple(args, "iii", &length, &below, &amount)) {
        return NULL;
    }

    if (amount < 0) {
        PyGILState_STATE gstate = PyGILState_Ensure();
        PyErr_SetString(PyExc_ValueError, "Amount must be positive");
        PyGILState_Release(gstate);

        return NULL;
    }

    if (length > below) {
        PyGILState_STATE gstate = PyGILState_Ensure();
        PyErr_SetString(PyExc_ValueError, "Length must be smaller than 'below', because this method will return a non-repeated values.");
        PyGILState_Release(gstate);

        return NULL;
    }

    if (length < 0) {

        PyGILState_STATE gstate = PyGILState_Ensure();
        PyErr_SetString(PyExc_ValueError, "Length must be positive.");
        PyGILState_Release(gstate);

        return NULL;
    }

    int randf;
    int* array = malloc(length * sizeof(int));

    PyListObject* output = (PyListObject*) PyList_New((Py_ssize_t)amount);

    for (int i = 0; i < amount; i++) {
        memset(array, length+1, length * sizeof(int));
        PyListObject* sublist = (PyListObject*) PyList_New((Py_ssize_t)length);

        int pos = 0;

        while (1) {
            
            int insert = 1;
            
            if (!generate_rdrand64_below(&randf, below+1)) {
                for (int j = 0; j < length; j++) {
                    if (array[j] == (int) randf) {
                        insert = 0;
                        break;
                    }
                }

                if (pos == length) {
                    break;
                }

                if (insert) {
                    array[pos++] = (int) randf;
                }
            } else {
                perror("Failed to get random value.");
                exit(2);
            }
        }

        for (int j = 0; j < length; j++) {
            PyList_SetItem((PyObject*) sublist, (Py_ssize_t) j, PyLong_FromLong(array[j]));
        }

        PyList_SetItem((PyObject*) output, (Py_ssize_t) i, (PyObject*) sublist);
        // PyList_Append(output, sublist);

    }

    free(array);
    return output;
}

static PyListObject* range_below(PyObject* self, PyObject* args) {

    int length, below;

    if (!PyArg_ParseTuple(args, "ii", &length, &below)) {
        return NULL;
    }

    if (length > below) {
        PyGILState_STATE gstate = PyGILState_Ensure();
        PyErr_SetString(PyExc_ValueError, "Length must be smaller than 'below', because this method will return a non-repeated values.");
        PyGILState_Release(gstate);

        return NULL;
    }

    if (length < 0) {

        PyGILState_STATE gstate = PyGILState_Ensure();
        PyErr_SetString(PyExc_ValueError, "Length must be positive.");
        PyGILState_Release(gstate);

        return NULL;
    }

    int randf;
    PyListObject* output = (PyListObject*) PyList_New((Py_ssize_t)length);

    int* array = malloc(length * sizeof(int));
    memset(array, length+1, length * sizeof(int));

    int pos = 0;

    while (1) {
        
        int insert = 1;
        
        if (!generate_rdrand64_below(&randf, below+1)) {
            for (int i = 0; i < length; i++) {
                if (array[i] == (int) randf) {
                    insert = 0;
                    break;
                }
            }

            if (pos == length) {
                break;
            }

            if (insert) {
                array[pos++] = (int) randf;
            }
        } else {
            perror("Failed to get random value.");
            exit(2);
        }
    }

    for (int i = 0; i < length; i++) {
        PyList_SetItem((PyObject*) output, (Py_ssize_t) i, PyLong_FromLong(array[i]));
    }

    free(array);
    return output;
}

static PyListObject* range(PyObject* self, PyObject* args, PyObject* kwargs) {

    int length;
    int boundary = 90;

    char* keywords[] = {"length", "boundary", NULL};
    
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "i|i", keywords, &length, &boundary)) {
        return NULL;
    }

    if (length > boundary) {

        char exception_text[256];
        sprintf(exception_text, "Lenght must be in the following range 0-%d.\n", abs(boundary));

        PyGILState_STATE gstate = PyGILState_Ensure();
        PyErr_SetString(PyExc_ValueError, exception_text);
        PyGILState_Release(gstate);

        return NULL;
    }

    if (length < 0) {
        PyGILState_STATE gstate = PyGILState_Ensure();
        PyErr_SetString(PyExc_ValueError, "length must be positive");
        PyGILState_Release(gstate);

        return NULL;
    }

    int randf;
    PyListObject* output = (PyListObject*) PyList_New((Py_ssize_t)length);
    
    // int array[91] = { 0 };
    int* array = (int*) malloc(boundary * sizeof(int));
    memset(array, 0, boundary * sizeof(int));
    
    int pos = 0;

    while (1) {
        
        int insert = 1;
        
        if (!generate_rdrand64_boundary(&randf, boundary)) {
            for (int i = 0; i < length; i++) {
                if (array[i] == (int) randf) {
                    insert = 0;
                    break;
                }
            }

            if (pos == length) {
                break;
            }

            if (insert) {
                array[pos] = (int) randf;
                pos++;
            }
        } else {
            perror("Failed to get random value.");
            exit(2);
        }
    }

    for (int i = 0; i < length; i++) {
        PyList_SetItem((PyObject*) output, (Py_ssize_t) i, PyLong_FromLong(array[i]));
    }

    free(array);
    return output;
}

static PyObject* is_rdrand_supported(PyObject* self) {
    if (rdrand_check_support() == 1)
        return (PyObject*)Py_True;
    return (PyObject*)Py_False;
}

static PyObject* is_rdseed_supported(PyObject* self) {
    if (rdseed_check_support() == 1)
        return (PyObject*)Py_True;
    return (PyObject*)Py_False;
}

static PyObject* rdseed(PyObject* self) {

    uint64_t retorno = 0;
    generate_rdseed(&retorno);

    return (PyObject*) PyLong_FromLong(retorno);
}

static PyMethodDef methods[] = {
    {"range", (PyCFunction)range, METH_VARARGS | METH_KEYWORDS, range__doc__},
    //{"randint", (PyCFunction)randint, METH_VARARGS, randint__doc__},
    {"randbits", (PyCFunction)randbits, METH_VARARGS, randbits__doc__},
    {"range_below", (PyCFunction)range_below, METH_VARARGS, range_below__doc__},
    {"n_range_below", (PyCFunction)n_range_below, METH_VARARGS, n_range_below__doc__},
    {"is_rdrand_supported", (PyCFunction)is_rdrand_supported, METH_NOARGS, is_rdrand_supported__doc__},
    {"is_rdseed_supported", (PyCFunction)is_rdseed_supported, METH_NOARGS, is_rdseed_supported__doc__},
    {"rdseed", (PyCFunction)rdseed, METH_NOARGS, "Return a int64 long using rdseed cpu instruction."},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef Module = {
    PyModuleDef_HEAD_INIT,
    "_rdrand",
    rdrand__doc__,
    -1,
    methods
};

PyMODINIT_FUNC PyInit__rdrand(void) {
    
    /* Check rdrand/rdseed compatibiltiy before initialize the module */
    if (rdrand_check_support() != 1 || rdseed_check_support() != 1) {

        PyGILState_STATE gstate = PyGILState_Ensure();
        PyErr_SetString(PyExc_ImportError, "This CPU does not support the rdrand/rdseed instruction.\n");
        PyGILState_Release(gstate);

        return NULL;
    }

    return PyModule_Create(&Module);
}

// int main(int argc, char** argv) {
//     wchar_t *program = Py_DecodeLocale(argv[0], NULL);
    
//     if (program == NULL) {
//         fprintf(stderr, "Fatal error: cannot decode argv[0].\n");
//         exit(1);
//     }

//     /* Add a built-in module, before Py_Initialize */
//     if (PyImport_AppendInittab("rdrand", PyInit_rdrand) == -1) {
//         fprintf(stderr, "Error: could not extend in-built modules table.\n");
//         exit(1);
//     }

//     Py_SetProgramName(program);

//     /* Initialize the Python interpreter */
//     Py_Initialize();

//     PyObject *pmodule = PyImport_ImportModule("rdrand");
//     if (!pmodule) {
//         PyErr_Print();
//         fprintf(stderr, "Error: could not import module 'rdrand'.\n");
//     }

//     PyMem_RawFree(program);
//     return 0;
// }