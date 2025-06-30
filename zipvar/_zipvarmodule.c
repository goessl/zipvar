#define PY_SSIZE_T_CLEAN
#include <Python.h>



typedef struct {
    PyTypeObject* zipvar_type;
} zipvar_state;

static inline zipvar_state* get_module_state(PyObject* mod)
{
    void* state = PyModule_GetState(mod);
    assert(state != NULL);
    return (zipvar_state*)state;
}

static inline zipvar_state* get_module_state_by_cls(PyTypeObject* cls)
{
    void* state = PyType_GetModuleState(cls);
    assert(state != NULL);
    return (zipvar_state*)state;
}

static struct PyModuleDef zipvarmodule;

static inline zipvar_state* find_state_by_type(PyTypeObject* tp)
{
    PyObject* mod = PyType_GetModuleByDef(tp, &zipvarmodule);
    assert(mod != NULL);
    return get_module_state(mod);
}



/* zipvar object *********************************************************/

typedef struct {
    PyObject_HEAD
    PyObject* itlist; //list of iterators that are still active
    PyObject* reslist; //result list
} zipvarobject;

#define zipvarobject_CAST(op)   ((zipvarobject *)(op))


//https://github.com/python/cpython/blob/c55512311b7cb8b7c27c19f56cd8f872be29aedc/Python/getargs.c#L2873
/* For type constructors that don't take keyword args
 *
 * Sets a TypeError and returns 0 if the args/kwargs is
 * not empty, returns 1 otherwise
 */
int my_PyArg_NoKeywords(const char* funcname, PyObject *kwargs) {
    if(kwargs == NULL) {
        return 1;
    }
    if(!PyDict_CheckExact(kwargs)) {
        PyErr_BadInternalCall();
        return 0;
    }
    if(PyDict_GET_SIZE(kwargs) == 0) {
        return 1;
    }
    
    PyErr_Format(PyExc_TypeError, "%.200s() takes no keyword arguments",
                    funcname);
    return 0;
}

static PyObject* zip_var_new(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
    zipvarobject* vz;
    Py_ssize_t i;
    PyObject* itlist;
    PyObject* reslist;
    Py_ssize_t listsize;
    
    //no kw args
    if(!my_PyArg_NoKeywords("zip_var", kwds)) {
        return NULL;
    }
    
    //iterables to iterators
    listsize = PyTuple_GET_SIZE(args);
    itlist = PyList_New(listsize);
    if(itlist == NULL) {
        return NULL;
    }
    //store in itlist but in reverse order (they are fetched from back to front in next)
    for(i=0; i<listsize; ++i) {
        PyObject* item = PyTuple_GET_ITEM(args, i);
        PyObject* it = PyObject_GetIter(item);
        if(it == NULL) {
            Py_DECREF(itlist);
            return NULL;
        }
        PyList_SET_ITEM(itlist, listsize-i-1, it);
    }
    
    //create result list
    reslist = PyList_New(0);
    if(reslist == NULL) {
        Py_DECREF(itlist);
        return NULL;
    }
    
    //create zipvarobject structure
    vz = (zipvarobject*)type->tp_alloc(type, 0);
    if(vz == NULL) {
        Py_DECREF(itlist);
        Py_DECREF(reslist);
        return NULL;
    }
    vz->itlist = itlist;
    vz->reslist = reslist;
    return (PyObject*)vz;
}

static void zip_var_dealloc(PyObject* op)
{
    zipvarobject* vz = zipvarobject_CAST(op);
    PyTypeObject* tp = Py_TYPE(vz);
    PyObject_GC_UnTrack(vz);
    Py_XDECREF(vz->itlist);
    Py_XDECREF(vz->reslist);
    tp->tp_free(vz);
    Py_DECREF(tp);
}

static int zip_var_traverse(PyObject* op, visitproc visit, void* arg)
{
    zipvarobject* vz = zipvarobject_CAST(op);
    Py_VISIT(Py_TYPE(vz));
    Py_VISIT(vz->itlist);
    Py_VISIT(vz->reslist);
    return 0;
}

static PyObject* zip_var_next(PyObject *op)
{
    zipvarobject* vz = zipvarobject_CAST(op);
    Py_ssize_t i;
    Py_ssize_t listsize;
    PyObject* it;
    PyObject* item;
    PyObject* reslist = vz->reslist;
    
    listsize = PyList_GET_SIZE(vz->itlist);
    if(listsize == 0) { //no iterators left
        return NULL; //StopIteration
    }
    
    if(PyList_Clear(reslist)) { //clear list failed
        return NULL;
    }
    
    for(i=listsize-1; i>=0; --i) {
        it = PyList_GET_ITEM(vz->itlist, i);
        item = PyIter_Next(it);
        if(item == NULL) { //next(it) failed or iterator is exhausted
            //if no error occured, try removing the iterator from the iterator list
            if(PyErr_Occurred() || PySequence_DelItem(vz->itlist, i)) { //next(it) or del iterators[i] failed
                return NULL;
            }
        } else {
            if(PyList_Append(reslist, item)) { //error during result build
                Py_DECREF(item);
                return NULL;
            }
            Py_DECREF(item);
        }
    }
    
    //no elements gotten
    if(PyList_GET_SIZE(reslist) == 0) {
        return NULL; //StopIteration
    }
    
    return PyList_AsTuple(reslist);
}

PyDoc_STRVAR(zip_var_doc,
"zip_var(*iterables)\n\
--\n\
\n\
Return a `zip_var` object whose `.__next__()` method returns tuples\n\
containing one element from each of the given iterables.  The order of\n\
elements in each tuple always matches the order of the input iterables.\n\
Iteration continues until all iterables are exhausted, then a StopIteration\n\
is raised.  Once an iterable is exhausted, it is excluded from the resulting\n\
tuples - they become shorter over time.\n\
");

static PyType_Slot zipvar_slots[] = {
    {Py_tp_dealloc, zip_var_dealloc},
    {Py_tp_getattro, PyObject_GenericGetAttr},
    {Py_tp_doc, (void*)zip_var_doc},
    {Py_tp_traverse, zip_var_traverse},
    {Py_tp_iter, PyObject_SelfIter},
    {Py_tp_iternext, zip_var_next},
    {Py_tp_new, zip_var_new},
    {Py_tp_free, PyObject_GC_Del},
    {0, NULL}
};

static PyType_Spec zipvar_spec = {
    .name = "_zipvar.zip_var",
    .basicsize = sizeof(zipvarobject),
    .flags = (Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC | Py_TPFLAGS_BASETYPE |
              Py_TPFLAGS_IMMUTABLETYPE),
    .slots = zipvar_slots
};


/* module level code ********************************************************/

PyDoc_STRVAR(module_doc, "zipvar module C implementation.");

static int zipvarmodule_traverse(PyObject* mod, visitproc visit, void* arg)
{
    zipvar_state* state = get_module_state(mod);
    Py_VISIT(state->zipvar_type);
    return 0;
}

static int zipvarmodule_clear(PyObject* mod)
{
    zipvar_state* state = get_module_state(mod);
    Py_CLEAR(state->zipvar_type);
    return 0;
}

static void zipvarmodule_free(void* mod)
{
    (void)zipvarmodule_clear((PyObject*)mod);
}

static int zipvarmodule_exec(PyObject* mod)
{
    zipvar_state* state = get_module_state(mod);
    
    state->zipvar_type =
            (PyTypeObject*)PyType_FromModuleAndSpec(mod, &zipvar_spec, NULL);
    if(state->zipvar_type == NULL) {
        return -1;
    }
    if(PyModule_AddType(mod, state->zipvar_type) < 0) {
        return -1;
    }
    
    return 0;
}

static struct PyModuleDef_Slot zipvarmodule_slots[] = {
    {Py_mod_exec, zipvarmodule_exec},
    {Py_mod_multiple_interpreters, Py_MOD_PER_INTERPRETER_GIL_SUPPORTED},
    {0, NULL}
};

static struct PyModuleDef zipvarmodule = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "_zipvar",
    .m_doc = module_doc,
    .m_size = sizeof(zipvar_state),
    .m_methods = NULL,
    .m_slots = zipvarmodule_slots,
    .m_traverse = zipvarmodule_traverse,
    .m_clear = zipvarmodule_clear,
    .m_free = zipvarmodule_free
};

PyMODINIT_FUNC PyInit__zipvar(void)
{
    return PyModuleDef_Init(&zipvarmodule);
}
