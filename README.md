# pytriangle

A 2D triangulation package based on the TRIANGLE program written by 
Jonathan Richard Shewchuck.

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
value zero.  
```python
pointBoundary = [(x0, y0), (x1, y1), ...]
markerBoundary = [1, 1, ...]
pointInner = [(xn, yn), ...]
markerInner = [0, ...]
points = pointBoundary + pointInner
markers = markerBoundary + markerInner
t.setPoints(points, markers=markers)
```

Create boundary segments, order is counterclockwise. Optionally add internal 
segments, order is clockwise. 
```python
segments = [(0, 1), (1, 2), ...(n-1, 0)]
t.set_segments(segments)
```

Optionally add list of points for each hole. Any point inside the hole will do
```python
holes = [(xh0, yh0), ...]
t.set_holes(holes)
```

Optionally add attributes (floating point values) for each point. There can be any 
of attributes. TRIANGLE will add internal points and the attributes will be interpolated
on these points (func is a function of the (x, y) coordinates)
```python
attributes = [(func(p), ...) for p in points]
t.set_attributes(attributes) 
```

Triangulate by specifying the maximum cell area, e.g.
```python
t.triangulate(area=0.01)
```

The triangulation can be refined with  
```python
t.refine(area_ratio=1.5)
```

The triangulation points can be obtained with
```python
nodes = t.get_nodes()
```
where nodes is in the format [[(x, y), marker], ...] where marker is 1 on the boundary 
and zero inside.

Triangle connectivity can be obtained with 
```python
triangles = t.get_triangles()
```
where triangles is in the format [[(i0, i1, i2), (k0, k1, k2), [a0, a1, ...]], ...],
i0, i1, and i2 are the node indices, k0, k1, and k2 are the neighbor node indices, and 
a0, a1, ... are the optional attributes interpolated to cell centers. 


