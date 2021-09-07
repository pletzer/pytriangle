# pytriangle

A python interface to the 2D triangulation program TRIANGLE written by 
Jonathan Richard Shewchuck [https://www.cs.cmu.edu/~quake/triangle.html]

<p align="left">
<a href="https://cirrus-ci.com/github/pletzer/pytriangle">
<img src="https://api.cirrus-ci.com/github/pletzer/pytriangle.svg?branch=master"
     alt="Cirrus-CI" /></a>
 <a href="https://github.com/pletzer/pytriangle/graphs/contributors">
  8 <img src="https://img.shields.io/github/contributors/pletzer/pytriangle.svg"
  9      alt="# contributors" /></a>
 10 <a href="https://github.com/pletzer/pytriangle/releases">
 11 <img src="https://img.shields.io/github/v/release/pletzer/pytriangle"
 12      alt="latest release" /></a>
 13 <a href="https://github.com/pletzer/pytriangle/commits/master">
 14 <img src="https://img.shields.io/github/commits-since/pletzer/pytriangle/latest.svg"
 15      alt="Commits since last release" /></a>
 16 </p>


### Usage ###

Import the triangle module
```python
import triangle
```

Create a Triangle instance
```python
t = triangle.Triangle()
```

Set n boundary points with marker value set to one. Set internal points with marker 
value set to zero.  
```python
pointBoundary = [(x0, y0), (x1, y1), ...]
markerBoundary = [1, 1, ...]
pointInner = [(xn, yn), ...]
markerInner = [0, ...]
points = pointBoundary + pointInner
markers = markerBoundary + markerInner
t.set_points(points, markers=markers)
```

Create boundary segments, order is counterclockwise for the outer boundary. 
```python
segments = [(0, 1), (1, 2), ...(n-1, 0)]
t.set_segments(segments)
```

If there are holes, you will need to add internal segments, which go clockwise 
around each hole. Specify the holes by providing a list of points inside each hole. 
Any point inside the hole will do
```python
holes = [(xh0, yh0), ...]
t.set_holes(holes)
```

Optionally add attributes (floating point values) to each point. There can be any 
of attributes. TRIANGLE will add internal points and the attributes will be interpolated
on these points and cells. As an example, set the attributes by calling an external 
function func, which takes the (x, y) coordinates as argument
```python
attributes = [(func(p), ...) for p in points]
t.set_point_attributes(attributes) 
```

Triangulate the set of points, segments, and holes 
by specifying the maximum cell area, e.g.
```python
t.triangulate(area=0.01)
```

The triangulation can be refined with  
```python
t.refine(area_ratio=1.5)
```
which will add triangles.

The triangulation points and their connectivity can be obtained with
```python
points = t.get_points()
triangles = t.get_triangles()
```
where points is in the format [[(x, y), marker], ...] where marker is 1 on the boundary 
and zero inside and triangles is in the format [[(i0, i1, i2), (k0, k1, k2), [a0, a1, ...]], ...],
i0, i1, and i2 are the vertex indices, k0, k1, and k2 are optional intermediate node indices, and 
a0, a1, ... are the optional attributes interpolated to cell centers. 

### Installation ###

Type 
```bash
python setup.py install
```

Alternatively, you can also install pytriangle with pip
```bash
pip install pytriangle
```

### Support ###

Send bug reports and comments to alexander _at_ gokliya _dot_ net

