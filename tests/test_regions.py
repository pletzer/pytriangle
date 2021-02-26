import math
import triangle
import numpy

pointBoundary =  [ (-1, -1),
                   (-1, 1.0), 
                   ( 0, 1.0),
                   ( 0, -1),
                   ( 1,  1),
                   ( 1, -3)]

points = pointBoundary

segs = [(0, 1),(1, 2),(2, 3),(3, 0), (2, 4), (4, 5), (5, 3)]

t = triangle.Triangle()
t.set_points(points)
t.set_segments(segs)

# regions can be defined with a regional attribute 'r' at x,y coordinates.
# Moreover it is possible to specify the area constraint in that region with
# the fourth parmaters 'a'
# regions = [(x,y,r,a),...]

regions = [ (-0.5, 0.5, 10, 0.1),
            (0.5, 0.5, 20, 0.5)]


t.set_regions(regions)


t.triangulate(mode='qpzAa')

# a function to plot the triangulation, for fast check of regional attributes
#  within the mesh triangles
t.plot_mesh().show()
