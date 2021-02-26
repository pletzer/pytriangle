/*
Python interface module to double version of Triangle
*/

#include <stdlib.h>
#include "Python.h"
#define REAL double
#define _NDIM 2
#include "triangle.h"

static char MSG[1024];
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
    sprintf(MSG, "ERROR in %s at line %d: wrong argument #1 (triangulateio handle required)\n", __FILE__, __LINE__);
    PyErr_SetString(PyExc_TypeError, MSG);
    return NULL;
  }    
  if(!PySequence_Check(xy)) {
    sprintf(MSG, "ERROR in %s at line %d: wrong argument #2 ([(x, y), ...] required)\n", __FILE__, __LINE__);
    PyErr_SetString(PyExc_TypeError, MSG);
    return NULL;
  }
  if(!PySequence_Check(mrks)) {
    sprintf(MSG, "ERROR in %s at line %d: wrong argument #3 ([m, ...] required)\n", __FILE__, __LINE__);
    PyErr_SetString(PyExc_TypeError, MSG);
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
      object->pointmarkerlist = malloc(npts * sizeof(int));
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
triangulate_SET_POINT_ATTRIBUTES(PyObject *self, PyObject *args) {
  PyObject *address, *atts, *elem;
  struct triangulateio *object;
  int npts, natts, i, j;

  if(!PyArg_ParseTuple(args,(char *)"OO", 
               &address, &atts)) { 
    return NULL;
  }
  if(!PyCapsule_CheckExact(address)) {
    sprintf(MSG, "ERROR in %s at line %d: wrong argument #1 (triangulateio handle required)\n", __FILE__, __LINE__);
    PyErr_SetString(PyExc_TypeError, MSG);
    return NULL;
  }    
  if(!PySequence_Check(atts)) {
    sprintf(MSG, "ERROR in %s at line %d: wrong argument #2 ([(a0, ...),...] required)\n", __FILE__, __LINE__);
    PyErr_SetString(PyExc_TypeError, MSG);
    return NULL;
  }
  object = PyCapsule_GetPointer(address, TRIANGULATEIO_NAME);  

  npts  = PySequence_Length(atts);
  if(npts != object->numberofpoints) {
    PyErr_SetString(PyExc_RuntimeError, 
      "Wrong number of attribute elements.");
    return NULL;
  }

  natts = 0;
  /* assume number of atts to be the same for all points! */
  if(npts > 0) {
    natts = PySequence_Length( PySequence_Fast_GET_ITEM(atts, 0) );
  }
  object->numberofpointattributes = natts;

  if(object->pointattributelist) free(object->pointattributelist);
  object->pointattributelist = malloc(natts * npts * sizeof(REAL));

  for(i = 0; i < npts; ++i) {
    elem = PySequence_Fast_GET_ITEM(atts, i);
    for(j = 0; j < natts; ++j) {
    object->pointattributelist[natts*i+j] = PyFloat_AsDouble(PySequence_Fast_GET_ITEM(elem, j));
    }
  }
  
  return Py_BuildValue("");
}


static PyObject *
triangulate_SET_TRIANGLE_ATTRIBUTES(PyObject *self, PyObject *args) {
  PyObject *address, *atts, *elem;
  struct triangulateio *object;
  int ntri, natts, i, j;

  if(!PyArg_ParseTuple(args,(char *)"OO", 
               &address, &atts)) { 
    return NULL;
  }
  if(!PyCapsule_CheckExact(address)) {
    sprintf(MSG, "ERROR in %s at line %d: wrong argument #1 (triangulateio handle required)\n", __FILE__, __LINE__);
    PyErr_SetString(PyExc_TypeError, MSG);
    return NULL;
  }    
  if(!PySequence_Check(atts)) {
    sprintf(MSG, "ERROR in %s at line %d: wrong argument #2 ([(a0, ...),...] required)\n", __FILE__, __LINE__);
    PyErr_SetString(PyExc_TypeError, MSG);
    return NULL;
  }
  object = PyCapsule_GetPointer(address, TRIANGULATEIO_NAME);  

  ntri  = PySequence_Length(atts);
  if(ntri != object->numberoftriangles) {
    sprintf(MSG, "ERROR in %s at line %d: wrong number of attributes (%d != %d)\n", __FILE__, __LINE__, ntri, object->numberoftriangles);
    PyErr_SetString(PyExc_RuntimeError, MSG);
    return NULL;
  }

  /* assume number of atts to be the same for all points! */
  natts = 0;
  if(ntri > 0) {
    natts = PySequence_Length( PySequence_Fast_GET_ITEM(atts, 0) );
  }    
  object->numberoftriangleattributes = natts;

  if(object->triangleattributelist) free(object->triangleattributelist);
  object->triangleattributelist = malloc(natts * ntri * sizeof(REAL));

  for(i = 0; i < ntri; ++i) {
    elem = PySequence_Fast_GET_ITEM(atts, i);
    for(j = 0; j < natts; ++j) {
      object->triangleattributelist[natts*i + j] = PyFloat_AsDouble(PySequence_Fast_GET_ITEM(elem, j));
    }
  }
  
  return Py_BuildValue("");
}


static PyObject *
triangulate_GET_POINT_ATTRIBUTES(PyObject *self, PyObject *args) {
  PyObject *address, *elem, *val, *result;
  struct triangulateio *object;
  int npts, natts, i, j;

  if(!PyArg_ParseTuple(args,(char *)"O", 
               &address)) { 
    return NULL;
  }
  if(!PyCapsule_CheckExact(address)) {
    sprintf(MSG, "ERROR in %s at line %d: wrong argument #1 (triangulateio handle required)\n", __FILE__, __LINE__);
    PyErr_SetString(PyExc_TypeError, MSG);
    return NULL;
  }    
  object = PyCapsule_GetPointer(address, TRIANGULATEIO_NAME);

  npts   = object->numberofpoints;
  result = PyList_New(npts);
  natts  = object->numberofpointattributes;
  for(i = 0; i < npts; ++i) {
    elem = PyTuple_New(natts);
    for(j = 0; j < natts; ++j) {
      val = PyFloat_FromDouble(object->pointattributelist[natts*i+j]);
      PyTuple_SET_ITEM(elem, j, val);
    }
    PyList_SET_ITEM(result, i, elem);
  }

  return result;
}


static PyObject *
triangulate_GET_TRIANGLE_ATTRIBUTES(PyObject *self, PyObject *args) {
  PyObject *address, *elem, *val, *result;
  struct triangulateio *object;
  int ntri, natts, i, j;

  if(!PyArg_ParseTuple(args,(char *)"O", 
               &address)) { 
    return NULL;
  }
  if(!PyCapsule_CheckExact(address)) {
    sprintf(MSG, "ERROR in %s at line %d: wrong argument #1 (triangulateio handle required)\n", __FILE__, __LINE__);
    PyErr_SetString(PyExc_TypeError, MSG);
    return NULL;
  }    
  object = PyCapsule_GetPointer(address, TRIANGULATEIO_NAME);

  ntri = object->numberoftriangles;
  result = PyList_New(ntri);
  natts  = object->numberoftriangleattributes;
  for(i = 0; i < ntri; ++i) {
    elem = PyTuple_New(natts);
    for(j = 0; j < natts; ++j) {
      val = PyFloat_FromDouble(object->triangleattributelist[natts*i + j]);
      PyTuple_SET_ITEM(elem, j, val);
    }
    PyList_SET_ITEM(result, i, elem);
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
    sprintf(MSG, "ERROR in %s at line %d: wrong argument #1 (triangulateio handle required)\n", __FILE__, __LINE__);
    PyErr_SetString(PyExc_TypeError, MSG);
    return NULL;
  }    
  if(!PySequence_Check(segs)) {
    sprintf(MSG, "ERROR in %s at line %d: wrong argument #2 ([(ia, ib),...] required)\n", __FILE__, __LINE__);
    PyErr_SetString(PyExc_TypeError, MSG);
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
    sprintf(MSG, "ERROR in %s at line %d: wrong argument #1 (triangulateio handle required)\n", __FILE__, __LINE__);
    PyErr_SetString(PyExc_TypeError, MSG);
    return NULL;
  }    
  if(!PySequence_Check(xy)) {
    sprintf(MSG, "ERROR in %s at line %d: wrong argument #2 ([(x, y),...] required)\n", __FILE__, __LINE__);
    PyErr_SetString(PyExc_TypeError, MSG);
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
triangulate_SET_REGIONS(PyObject *self, PyObject *args) {
  PyObject *address, *xy, *elem;
  struct triangulateio *object;
  int nr, i;

  if(!PyArg_ParseTuple(args,(char *)"OO", 
               &address, &xy)) { 
    return NULL;
  }
  if(!PyCapsule_CheckExact(address)) {
    sprintf(MSG, "ERROR in %s at line %d: wrong argument #1 (triangulateio handle required)\n", __FILE__, __LINE__);
    PyErr_SetString(PyExc_TypeError, MSG);
    return NULL;
  }    
  if(!PySequence_Check(xy)) {
    sprintf(MSG, "ERROR in %s at line %d: wrong argument #2 ([(x, y, r , a),...] required)\n", __FILE__, __LINE__);
    PyErr_SetString(PyExc_TypeError, MSG);
    return NULL;
  }
  object = PyCapsule_GetPointer(address, TRIANGULATEIO_NAME);

  nr = PySequence_Length(xy);
  if(nr != object->numberofregions) {
    if(object->regionlist) free(object->regionlist);
    object->regionlist = malloc(nr * 4 * sizeof(REAL));
  }
  object->numberofregions = nr;
  for(i = 0; i < nr; ++i) {
    elem = PySequence_Fast_GET_ITEM(xy, i);
    object->regionlist[4*i  ] = PyFloat_AsDouble(PySequence_Fast_GET_ITEM(elem, 0));
    object->regionlist[4*i+1] = PyFloat_AsDouble(PySequence_Fast_GET_ITEM(elem, 1));
    object->regionlist[4*i+2] = PyFloat_AsDouble(PySequence_Fast_GET_ITEM(elem, 2));
    object->regionlist[4*i+3] = PyFloat_AsDouble(PySequence_Fast_GET_ITEM(elem, 3));
  }

  return Py_BuildValue("");
}

static PyObject *
triangulate_TRIANGULATE(PyObject *self, PyObject *args) {
  PyObject *address_in, *address_out, *address_vor;
  struct triangulateio *object_in, *object_out, *object_vor;
  char *swtch;
  int i;

  if(!PyArg_ParseTuple(args,(char *)"sOOO",
               &swtch, &address_in, &address_out, &address_vor)) {
    return NULL;
  }
  if(!PyCapsule_CheckExact(address_in)) {
    sprintf(MSG, "ERROR in %s at line %d: wrong argument #2 (input triangulateio handle required)\n", __FILE__, __LINE__);
    PyErr_SetString(PyExc_TypeError, MSG);
    return NULL;
  }    
  if(!PyCapsule_CheckExact(address_out)) {
    sprintf(MSG, "ERROR in %s at line %d: wrong argument #3 (output triangulateio handle required)\n", __FILE__, __LINE__);
    PyErr_SetString(PyExc_TypeError, MSG);
    return NULL;
  }    
  if(!PyCapsule_CheckExact(address_vor)) {
    sprintf(MSG, "ERROR in %s at line %d: wrong argument #4 (Voronoi handle required)\n", __FILE__, __LINE__);
    PyErr_SetString(PyExc_TypeError, MSG);
    return NULL;
  }    
  object_in  = PyCapsule_GetPointer(address_in,  TRIANGULATEIO_NAME);
  object_out = PyCapsule_GetPointer(address_out, TRIANGULATEIO_NAME);
  object_vor = PyCapsule_GetPointer(address_vor, TRIANGULATEIO_NAME);

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
    /* x, y, region_attribute, max_area */
    object_out->regionlist = malloc(4 * object_in->numberofregions * sizeof(REAL));
    for(i = 0; i < 4 * object_in->numberofregions; ++i) {
      object_out->regionlist[i] = object_in->regionlist[i];
    }
  }

  return Py_BuildValue("");
}

static PyObject *
triangulate_GET_NUM_POINTS(PyObject *self, PyObject *args) {
  PyObject *address;
  struct triangulateio *object;
  int n;
  
  if(!PyArg_ParseTuple(args, "O", 
               &address)) { 
    return NULL;
  }
  if(!PyCapsule_CheckExact(address)) {
    sprintf(MSG, "ERROR in %s at line %d: wrong argument (triangulateio handle required)\n", __FILE__, __LINE__);
    PyErr_SetString(PyExc_TypeError, MSG);
    return NULL;
  }
  object = PyCapsule_GetPointer(address, TRIANGULATEIO_NAME);
  n = object->numberofpoints;

  return Py_BuildValue("i", n);
}

static PyObject *
triangulate_GET_NUM_TRIANGLES(PyObject *self, PyObject *args) {
  PyObject *address;
  struct triangulateio *object;
  int n;
  
  if(!PyArg_ParseTuple(args, "O", 
               &address)) { 
    return NULL;
  }
  if(!PyCapsule_CheckExact(address)) {
    sprintf(MSG, "ERROR in %s at line %d: wrong argument (triangulateio handle required)\n", __FILE__, __LINE__);
    PyErr_SetString(PyExc_TypeError, MSG);
    return NULL;
  }
  object = PyCapsule_GetPointer(address, TRIANGULATEIO_NAME);
  n = object->numberoftriangles;

  return Py_BuildValue("i", n);
}

static PyObject *
triangulate_GET_POINTS(PyObject *self, PyObject *args) {

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
    sprintf(MSG, "ERROR in %s at line %d: wrong argument (triangulateio handle required)\n", __FILE__, __LINE__);
    PyErr_SetString(PyExc_TypeError, MSG);
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
    sprintf(MSG, "ERROR in %s at line %d: wrong argument (triangulateio handle required)\n", __FILE__, __LINE__);
    PyErr_SetString(PyExc_TypeError, MSG);
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
  PyObject *address, *holder, *points, *kk, *atts, *aa, *elem, *neigh, *mm;
  struct triangulateio *object;
  int i, j, k, m, nc, na, nt;
  REAL a;
  
  if(!PyArg_ParseTuple(args, "O", 
               &address)) { 
    return NULL;
  }
  if(!PyCapsule_CheckExact(address)) {
    sprintf(MSG, "ERROR in %s at line %d: wrong argument (triangulateio handle required)\n", __FILE__, __LINE__);
    PyErr_SetString(PyExc_TypeError, MSG);
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
    points = PyList_New(nc);
    neigh = PyTuple_New(nt);
    atts  = PyList_New(na);
    for(j = 0; j < nc; ++j) {
      k = object->trianglelist[nc*i+j];
      kk = PyLong_FromLong((long) k);
      PyList_SET_ITEM(points, j, kk);
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
    PyTuple_SET_ITEM(elem, 0, points);
    PyTuple_SET_ITEM(elem, 1, neigh);
    PyTuple_SET_ITEM(elem, 2, atts );
    PyList_SET_ITEM(holder, i, elem);
  }
     
  return holder;  
}

static PyMethodDef triangulate_methods[] = {
  {"new", triangulate_NEW, METH_VARARGS, "Return new handle to triangulateio structure ()->h."},
  {"set_points", triangulate_SET_POINTS, METH_VARARGS, 
   "Set points and markers (h, [(x1,y1),(x2,y2)..], [m1,m2..])->None. \nh: handle.\n[(x1,y1),(x2,y2)..]: coordinates.\n[m1,m2,..]: point markers (1 per point)."},
  {"set_point_attributes", triangulate_SET_POINT_ATTRIBUTES, METH_VARARGS, 
   "Set point attributes (h, [(a1,a2,..),..])->None. \nh: handle.\n[(a1,a2,..),..]: atributes (a1,a2,..)."},
  {"set_triangle_attributes", triangulate_SET_TRIANGLE_ATTRIBUTES, METH_VARARGS, 
   "Set triangle attributes (h, [(a1,a2,..),..])->None. \nh: handle.\n[(a1,a2,..),..]: atributes (a1,a2,..)."},
  {"get_point_attributes", triangulate_GET_POINT_ATTRIBUTES, METH_VARARGS, 
   "Get point attributes (h)->[(a1,a2,..),..]. \nh: handle."},
  {"get_triangle_attributes", triangulate_GET_TRIANGLE_ATTRIBUTES, METH_VARARGS, 
   "Get triangle attributes (h)->[(a1,a2,..),..]. \nh: handle."},
  {"set_segments", triangulate_SET_SEGMENTS, METH_VARARGS, 
   "Set segments (h, [(i,j),..])->None. \nh: handle.\n[(i,j),..]: segments."},
  {"set_holes", triangulate_SET_HOLES, METH_VARARGS, 
   "Set holes (h, [(x1,y1),(x2,y2),..])->None. \nh: handle.\n[(x1,y1),(x2,y2),..]: hole coordinates."},
  {"set_regions", triangulate_SET_REGIONS, METH_VARARGS, 
   "Set regions (h, [(x1,y1,r1,a1),(x2,y2,r2,a2),..])->None. \nh: handle.\n[(x1,y1,r1,a1),(x2,y2,r2,a2),..]: region tag coordinates, id and area constraint."},
  {"triangulate", triangulate_TRIANGULATE, METH_VARARGS, 
   "Triangulate or refine an existing triangulation (switches, h_in, h_out, h_vor)->None.\nswitches: a string (see Triangle doc).\nh_in, h_out, h_vor: handles to the input, output and Voronoi triangulateio structs."},
  {"get_num_points", triangulate_GET_NUM_POINTS, METH_VARARGS, 
   "Return number of points."},
  {"get_num_triangles", triangulate_GET_NUM_TRIANGLES, METH_VARARGS, 
   "Return number of triangles."},
  {"get_points", triangulate_GET_POINTS, METH_VARARGS, 
   "Return dict from handle (h)->{i: [(x,y),[i1,i2,..], m],..}.\nh: handle.\n(x,y): point coordinates.\n[i1,i2..]: neighboring point indices.\nm: point marker (0=interior, 1=boundary)."},
  {"get_edges", triangulate_GET_EDGES, METH_VARARGS, 
   "Return list of edge points with edge markers (h)->[((i1,i2),m),..].\nh: handle.\n(i1,i2): point indices.\nm: edge markers."},
  {"get_triangles", triangulate_GET_TRIANGLES, METH_VARARGS, 
   "Return list of triangles (h)->[([i1,i2,i3,..],(k1,k2,k3), [a1,a2,..]),..].\nh: handle.\ni1,i2,i3,..: point indices at the triangle corners, optionally followed by intermediate points.\n(k1,k2,k3) and neighboring triangle indices;\na1,a2..: triangle cell attributes."},
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


#if PY_MAJOR_VERSION >= 3
    PyObject *module = PyModule_Create(&triangulateio_moduledef);
    return module;
#else
    Py_InitModule("triangulate", triangulate_methods);
#endif
}
