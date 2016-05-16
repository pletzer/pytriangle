/*

$Id: triangulatemodule.c,v 1.7 2005/11/09 15:18:59 pletzer Exp $

Python interface module to double version of Triangle

*/

#include <stdlib.h>
#include "Python.h"
#define REAL double
#define _NDIM 2
#include "triangle.h"

#define TRIANGULATEIO_NAME "triangulateio"

#if defined(Py_DEBUG) || defined(DEBUG)
extern void _Py_CountReferences(FILE*);
#define CURIOUS(x) { fprintf(stderr, __FILE__ ":%d ", __LINE__); x; }
#else
#define CURIOUS(x)
#endif
#define MARKER()        CURIOUS(fprintf(stderr, "\n"))
#define DESCRIBE(x)     CURIOUS(fprintf(stderr, "  " #x "=%d\n", x))
#define DESCRIBE_HEX(x) CURIOUS(fprintf(stderr, "  " #x "=%08x\n", x))
#define COUNTREFS()     CURIOUS(_Py_CountReferences(stderr))

void destroy_triangulateio(PyObject *address) {

  struct triangulateio *object = NULL;

#if defined(Py_DEBUG) || defined(DEBUG)
  printf("now destroying triangulateio\n");
#endif

  object = PyCapsule_GetPointer(address, TRIANGULATEIO_NAME);

  if( object->pointlist             ) free( object->pointlist             );
  if( object->pointattributelist    ) free( object->pointattributelist    ); 
  if( object->pointmarkerlist       ) free( object->pointmarkerlist       );

  if( object->trianglelist          ) free( object->trianglelist          );
  if( object->triangleattributelist ) free( object->triangleattributelist );
  if( object->trianglearealist      ) free( object->trianglearealist      );
  if( object->neighborlist          ) free( object->neighborlist          );

  if( object->segmentlist           ) free( object->segmentlist           );
  if( object->segmentmarkerlist     ) free( object->segmentmarkerlist     );

  if( object->holelist              ) free( object->holelist              );

  if( object->regionlist            ) free( object->regionlist            );

  if( object->edgelist              ) free( object->edgelist              );
  if( object->edgemarkerlist        ) free( object->edgemarkerlist        );
  if( object->normlist              ) free( object->normlist              );

  free(object);

}

static PyObject *
triangulate_NEW(PyObject *self, PyObject *args) {
  PyObject *address, *result;
  struct triangulateio *object;

  object = malloc(sizeof(struct triangulateio));

  object->pointlist             = NULL;
  object->pointattributelist    = NULL; /* In / out */
  object->pointmarkerlist       = NULL; /* In / out */
  object->numberofpoints         = 0;   /* In / out */
  object->numberofpointattributes= 0;   /* In / out */

  object->trianglelist          = NULL; /* In / out */
  object->triangleattributelist = NULL; /* In / out */
  object->trianglearealist      = NULL; /* In only */
  object->neighborlist          = NULL; /* Out only */
  object->numberoftriangles     = 0;    /* In / out */
  object->numberofcorners       = 0;    /* In / out */
  object->numberoftriangleattributes = 0;    /* In / out */

  object->segmentlist           = NULL; /* In / out */
  object->segmentmarkerlist     = NULL; /* In / out */
  object->numberofsegments      = 0;    /* In / out */

  object->holelist              = NULL; /* In / pointer to array copied out */
  object->numberofholes         = 0;    /* In / copied out */

  object->regionlist            = NULL; /* In / pointer to array copied out */
  object->numberofregions       = 0;    /* In / copied out */

  object->edgelist              = NULL; /* Out only */
  object->edgemarkerlist        = NULL; /* Not used with Voronoi diagram; out only */
  object->normlist              = NULL; /* Used only with Voronoi diagram; out only */
  object->numberofedges         = 0;    /* Out only */

  /* return opaque handle */

  address = PyCapsule_New(object, TRIANGULATEIO_NAME, destroy_triangulateio);
  result = Py_BuildValue("O", address);
  Py_DECREF(address);
  return result;
}

static PyObject *
triangulate_SET_POINTS(PyObject *self, PyObject *args) {
  PyObject *address, *xy, *mrks, *elem;
  struct triangulateio *object;
  int npts, i;

  if(!PyArg_ParseTuple(args,(char *)"OOO", 
		       &address, &xy, &mrks)) { 
    return NULL;
  }
  if(!PyCapsule_CheckExact(address)) {
    PyErr_SetString(PyExc_TypeError,
      "Wrong 1st argument! triangulateio handle required.");
    return NULL;
  }    
  if(!PySequence_Check(xy)) {
    PyErr_SetString(PyExc_TypeError,
      "Wrong 2nd argument! Sequence required (xy).");
    return NULL;
  }
  if(!PySequence_Check(mrks)) {
    PyErr_SetString(PyExc_TypeError,
      "Wrong 3rd argument! Sequence required (mrks).");
    return NULL;
  }
  object = PyCapsule_GetPointer(address, TRIANGULATEIO_NAME);  

  npts  = PySequence_Length(xy);
  if(npts > 0) {
    if(npts != object->numberofpoints) {
      if(object->pointlist) free(object->pointlist);
      /* if(object->pointattributelist) free(object->pointattributelist); */
      if(object->pointmarkerlist) free(object->pointmarkerlist);
      object->pointlist = malloc(_NDIM * npts * sizeof(REAL));
      /* object->pointattributelist = malloc(natts * npts * sizeof(REAL)); */
      object->pointmarkerlist    = malloc(npts * sizeof(int));
    }
    object->numberofpoints = npts;
  }
  for(i = 0; i < npts; ++i) {
    elem = PySequence_Fast_GET_ITEM(xy, i);
    object->pointlist[_NDIM*i    ] = PyFloat_AsDouble(PySequence_Fast_GET_ITEM(elem, 0));
    object->pointlist[_NDIM*i + 1] = PyFloat_AsDouble(PySequence_Fast_GET_ITEM(elem, 1));
    elem = PySequence_Fast_GET_ITEM(mrks, i);
    object->pointmarkerlist[i] = (int) PyLong_AsLong(elem);
  }
  
  return Py_BuildValue("");
}

static PyObject *
triangulate_SET_ATTRIBUTES(PyObject *self, PyObject *args) {
  PyObject *address, *atts, *elem;
  struct triangulateio *object;
  int npts, natts, i, j;

  if(!PyArg_ParseTuple(args,(char *)"OO", 
		       &address, &atts)) { 
    return NULL;
  }
  if(!PyCapsule_CheckExact(address)) {
    PyErr_SetString(PyExc_TypeError,
      "Wrong 1st argument! triangulateio handle required.");
    return NULL;
  }    
  if(!PySequence_Check(atts)) {
    PyErr_SetString(PyExc_TypeError,
      "Wrong 2nd argument! Sequence required (atts).");
    return NULL;
  }
  object = PyCapsule_GetPointer(address, TRIANGULATEIO_NAME);  

  npts  = PySequence_Length(atts);
  if(npts != object->numberofpoints) {
    PyErr_SetString(PyExc_RuntimeError, 
      "Wrong number of attribute elements.");
    return NULL;
  }
  /* assume number of atts to be the same for all nodes! */
  if(npts > 0) {
    natts = PySequence_Length( PySequence_Fast_GET_ITEM(atts, 0) );
    if(natts != object->numberofpointattributes) {
      if(object->pointattributelist) free(object->pointattributelist);
      object->pointattributelist = malloc(natts * npts * sizeof(REAL));
    }
    object->numberofpointattributes = natts;
    for(i = 0; i < npts; ++i) {
      elem = PySequence_Fast_GET_ITEM(atts, i);
      for(j = 0; j < natts; ++j) {
        object->pointattributelist[natts*i+j] = PyFloat_AsDouble(PySequence_Fast_GET_ITEM(elem, j));
      }
    }
  }
  
  return Py_BuildValue("");
}

static PyObject *
triangulate_GET_ATTRIBUTES(PyObject *self, PyObject *args) {
  PyObject *address, *elem, *val, *result;
  struct triangulateio *object;
  int npts, natts, i, j;

  if(!PyArg_ParseTuple(args,(char *)"O", 
		       &address)) { 
    return NULL;
  }
  if(!PyCapsule_CheckExact(address)) {
    PyErr_SetString(PyExc_TypeError,
      "Wrong 1st argument! triangulateio handle required.");
    return NULL;
  }    
  object = PyCapsule_GetPointer(address, TRIANGULATEIO_NAME);

  result = PyList_New(object->numberofpoints);
  npts   = object->numberofpoints;
  natts  = object->numberofpointattributes;
  for(i = 0; i < npts; ++i) {
    elem = PyTuple_New(natts);
    for(j = 0; j < natts; ++j) {
      val = PyFloat_FromDouble(object->pointattributelist[natts*i+j]);
      PyTuple_SET_ITEM(elem, j, val);
      /* Py_DECREF(val); */
    }
    PyList_SET_ITEM(result, i, elem);
    /* Py_DECREF(elem); */
  }

  return result;
}

static PyObject *
triangulate_SET_SEGMENTS(PyObject *self, PyObject *args) {
  PyObject *address, *segs, *elem;
  struct triangulateio *object;
  int ns, i;

  if(!PyArg_ParseTuple(args,(char *)"OO", 
		       &address, &segs)) { 
    return NULL;
  }
  if(!PyCapsule_CheckExact(address)) {
    PyErr_SetString(PyExc_TypeError,
      "Wrong 1st argument! triangulateio handle required.");
    return NULL;
  }    
  if(!PySequence_Check(segs)) {
    PyErr_SetString(PyExc_TypeError,
      "Wrong 2nd argument! Sequence required (segs).");
    return NULL;
  }
  object = PyCapsule_GetPointer(address, TRIANGULATEIO_NAME);  

  ns = PySequence_Length(segs);
  if(ns != object->numberofsegments) {
    if(object->segmentlist) free(object->segmentlist);
    object->segmentlist = malloc(_NDIM * object->numberofpoints * sizeof(int));
  }
  object->numberofsegments = ns;
  for(i = 0; i < ns; ++i) {
    elem = PySequence_Fast_GET_ITEM(segs, i);
    object->segmentlist[_NDIM*i  ] = (int) PyLong_AsLong(PySequence_Fast_GET_ITEM(elem,0));
    object->segmentlist[_NDIM*i+1] = (int) PyLong_AsLong(PySequence_Fast_GET_ITEM(elem,1));
  }

  return Py_BuildValue("");
}

static PyObject *
triangulate_SET_HOLES(PyObject *self, PyObject *args) {
  PyObject *address, *xy, *elem;
  struct triangulateio *object;
  int nh, i;

  if(!PyArg_ParseTuple(args,(char *)"OO", 
		       &address, &xy)) { 
    return NULL;
  }
  if(!PyCapsule_CheckExact(address)) {
    PyErr_SetString(PyExc_TypeError,
      "Wrong 1st argument! triangulateio handle required.");
    return NULL;
  }    
  if(!PySequence_Check(xy)) {
    PyErr_SetString(PyExc_TypeError,
      "Wrong 2nd argument! Sequence required (xy).");
    return NULL;
  }
  object = PyCapsule_GetPointer(address, TRIANGULATEIO_NAME);

  nh = PySequence_Length(xy);
  if(nh != object->numberofholes) {
    if(object->holelist) free(object->holelist);
    object->holelist = malloc(nh * _NDIM * sizeof(REAL));
  }
  object->numberofholes = nh;
  for(i = 0; i < nh; ++i) {
    elem = PySequence_Fast_GET_ITEM(xy, i);
    object->holelist[_NDIM*i  ] = PyFloat_AsDouble(PySequence_Fast_GET_ITEM(elem, 0));
    object->holelist[_NDIM*i+1] = PyFloat_AsDouble(PySequence_Fast_GET_ITEM(elem, 1));
  }

  return Py_BuildValue("");
}

static PyObject *
triangulate_TRIANGULATE(PyObject *self, PyObject *args) {
  PyObject *switches, *address_in, *address_out, *address_vor;
  struct triangulateio *object_in, *object_out, *object_vor;
  char *swtch;
  int i;

  if(!PyArg_ParseTuple(args,(char *)"OOOO", 
		       &switches, &address_in, &address_out, &address_vor)) { 
    return NULL;
  }
  if(!PyBytes_Check(switches)) {
    PyErr_SetString(PyExc_TypeError,
      "Wrong 1st argument! String required.");
    return NULL;
  }    
  if(!PyCapsule_CheckExact(address_in)) {
    PyErr_SetString(PyExc_TypeError,
      "Wrong 2nd argument! input triangulateio handle required.");
    return NULL;
  }    
  if(!PyCapsule_CheckExact(address_out)) {
    PyErr_SetString(PyExc_TypeError,
      "Wrong 3rd argument! output triangulateio handle required.");
    return NULL;
  }    
  if(!PyCapsule_CheckExact(address_vor)) {
    PyErr_SetString(PyExc_TypeError,
      "Wrong 4th argument! voronoi triangulateio handle required.");
    return NULL;
  }    
  object_in  = PyCapsule_GetPointer(address_in,  TRIANGULATEIO_NAME);
  object_out = PyCapsule_GetPointer(address_out, TRIANGULATEIO_NAME);
  object_vor = PyCapsule_GetPointer(address_vor, TRIANGULATEIO_NAME);

  swtch = PyBytes_AS_STRING(switches);
  triangulate(swtch, object_in, object_out, object_vor);

  /* Copy holelist and regionlist. These are input only with the pointer
     sharing the address of the input struct. By copying these, we
     will ensure that the individual handles can be destroyed gracefully */

  object_out->holelist = NULL;
  object_out->holelist = malloc(_NDIM * object_in->numberofholes * sizeof(REAL));
  for(i = 0; i < 2*object_in->numberofholes; ++i) {
    object_out->holelist[i] = object_in->holelist[i];
  }
  object_out->regionlist = NULL;
  if(object_in->numberofregions > 0) {
    /* x, y, region_attribute, max_area, four_area_constraints */
    object_out->regionlist = malloc((_NDIM+2+4) * object_in->numberofregions * sizeof(REAL));
    for(i = 0; i < (_NDIM+2+4)*object_in->numberofregions; ++i) {
      object_out->regionlist[i] = object_in->regionlist[i];
    }
  }

  return Py_BuildValue("");
}

static PyObject *
triangulate_GET_NODES(PyObject *self, PyObject *args) {

  /* Return a [ [(x,y), marker],..] */

  PyObject *address, *holder, *mlist;
  struct triangulateio *object;
  int i, m;
  REAL x, y;
  
  if(!PyArg_ParseTuple(args, "O", 
		       &address)) { 
    return NULL;
  }
  if(!PyCapsule_CheckExact(address)) {
    PyErr_SetString(PyExc_TypeError,
		    "Wrong argument! triangulateio handle required.");
    return NULL;
  }
  object = PyCapsule_GetPointer(address, TRIANGULATEIO_NAME);

  holder = PyList_New(object->numberofpoints);

  for (i = 0; i < object->numberofpoints; ++i) {
    x = object->pointlist[_NDIM*i  ];
    y = object->pointlist[_NDIM*i + 1];
    m = object->pointmarkerlist[i];
    mlist = Py_BuildValue("[(d,d),i]", x, y, m);
    PyList_SET_ITEM(holder, i, mlist);
  }
    
  return holder;  
}

static PyObject *
triangulate_GET_EDGES(PyObject *self, PyObject *args) {
  PyObject *address, *holder, *i1_i2_m;
  struct triangulateio *object;
  int i, i1, i2, m;
  
  if (!PyArg_ParseTuple(args, "O",
		       &address)) { 
    return NULL;
  }
  if (!PyCapsule_CheckExact(address)) {
    PyErr_SetString(PyExc_TypeError,
		    "Wrong argument! triangulateio handle required.");
    return NULL;
  }
  object = PyCapsule_GetPointer(address, TRIANGULATEIO_NAME);

  holder = PyList_New(object->numberofedges);

  for (i = 0; i < object->numberofedges; ++i) {
    i1 = object->edgelist[2*i  ];
    i2 = object->edgelist[2*i+1];
    m  = object->edgemarkerlist[i];
    i1_i2_m = Py_BuildValue("((i,i),i)", i1, i2, m);
    PyList_SET_ITEM(holder, i, i1_i2_m);
  }
     
  return holder;  
}

static PyObject *
triangulate_GET_TRIANGLES(PyObject *self, PyObject *args) {
  PyObject *address, *holder, *nodes, *kk, *atts, *aa, *elem, *neigh, *mm;
  struct triangulateio *object;
  int i, j, k, m, nc, na, nt;
  REAL a;
  
  if(!PyArg_ParseTuple(args, "O", 
		       &address)) { 
    return NULL;
  }
  if(!PyCapsule_CheckExact(address)) {
    PyErr_SetString(PyExc_TypeError,
		    "Wrong argument! triangulateio handle required.");
    return NULL;
  }
  object = PyCapsule_GetPointer(address, TRIANGULATEIO_NAME);

  holder = PyList_New(object->numberoftriangles);
  nc = object->numberofcorners;
  if(object->neighborlist) {
    nt = 3;
  } else{
    nt = 0;
  }
  na = object->numberoftriangleattributes;

  for(i = 0; i < object->numberoftriangles; ++i) {
    nodes = PyList_New(nc);
    neigh = PyTuple_New(nt);
    atts  = PyList_New(na);
    for(j = 0; j < nc; ++j) {
      k = object->trianglelist[nc*i+j];
      kk = PyLong_FromLong((long) k);
      PyList_SET_ITEM(nodes, j, kk);
    }
    for(j = 0; j < nt; ++j) {
      m = object->neighborlist[nt*i+j];
      mm = PyLong_FromLong((long)m);
      PyTuple_SET_ITEM(neigh, j, mm);
    }
    for(j = 0; j < na; ++j) {
      a = object->triangleattributelist[na*i+j];
      aa = PyFloat_FromDouble(a);
      PyList_SET_ITEM(atts, j, aa);
    }
    elem = PyTuple_New(3);
    PyTuple_SET_ITEM(elem, 0, nodes);
    PyTuple_SET_ITEM(elem, 1, neigh);
    PyTuple_SET_ITEM(elem, 2, atts );
    PyList_SET_ITEM(holder, i, elem);
  }
     
  return holder;  
}

static PyMethodDef triangulate_methods[] = {
  {"new", triangulate_NEW, METH_VARARGS, "Return new handle to triangulateio structure ()->h."},
  {"set_points", triangulate_SET_POINTS, METH_VARARGS, 
   "Set points and markers (h, [(x1,y1),(x2,y2)..], [m1,m2..])->None. \nh: handle.\n[(x1,y1),(x2,y2)..]: coordinates.\n[m1,m2,..]: node markers (1 per node)."},
  {"set_attributes", triangulate_SET_ATTRIBUTES, METH_VARARGS, 
   "Set node attributes (h, [(a1,a2,..),..])->None. \nh: handle.\n[(a1,a2,..),..]: atributes (a1,a2,..)."},
  {"get_attributes", triangulate_GET_ATTRIBUTES, METH_VARARGS, 
   "Get node attributes (h)->[(a1,a2,..),..]. \nh: handle."},
  {"set_segments", triangulate_SET_SEGMENTS, METH_VARARGS, 
   "Set segments (h, [(i,j),..])->None. \nh: handle.\n[(i,j),..]: segments."},
  {"set_holes", triangulate_SET_HOLES, METH_VARARGS, 
   "Set holes (h, [(x1,y1),(x2,y2),..])->None. \nh: handle.\n[(x1,y1),(x2,y2),..]: hole coordinates."},
  {"triangulate", triangulate_TRIANGULATE, METH_VARARGS, 
   "Triangulate or refine an existing triangulation (switches, h_in, h_out, h_vor)->None.\nswitches: a string (see Triangle doc).\nh_in, h_out, h_vor: handles to the input, output and Voronoi triangulateio structs."},
  {"get_nodes", triangulate_GET_NODES, METH_VARARGS, 
   "Return node dict from handle (h)->{i: [(x,y),[i1,i2,..], m],..}.\nh: handle.\n(x,y): node coordinates.\n[i1,i2..]: neighboring node indices.\nm: node marker (0=interior, 1=boundary)."},
  {"get_edges", triangulate_GET_EDGES, METH_VARARGS, 
   "Return list of edge nodes with edge markers (h)->[((i1,i2),m),..].\nh: handle.\n(i1,i2): node indices.\nm: edge markers."},
  {"get_triangles", triangulate_GET_TRIANGLES, METH_VARARGS, 
   "Return list of triangles (h)->[([i1,i2,i3,..],(k1,k2,k3), [a1,a2,..]),..].\nh: handle.\ni1,i2,i3,..: node indices at the triangle corners, optionally followed by intermediate nodes.\n(k1,k2,k3): neighboring triangle indices;\na1,a2..: triangle cell attributes."},
  {NULL, NULL, 0, NULL}
};

struct triangulate_state {
  PyObject *object;
};

#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef triangulateio_moduledef = {
        PyModuleDef_HEAD_INIT,
        "triangulate",
        NULL,
        sizeof(struct triangulate_state),
        triangulate_methods,
        NULL,
        NULL,
        NULL,
        NULL
};

PyMODINIT_FUNC
PyInit_triangulate(void)
#else
void 
inittriangulate() 
#endif
{

//Py_InitModule( "triangulate", triangulate_methods);

#if PY_MAJOR_VERSION >= 3
    PyObject *module = PyModule_Create(&triangulateio_moduledef);
#else
    Py_InitModule("triangulate", triangulate_methods);
#endif

#if PY_MAJOR_VERSION >= 3
    return module;
#endif
}
