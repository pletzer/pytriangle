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
t.setPoints(points)
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

