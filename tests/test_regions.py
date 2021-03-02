import math
import triangle
import numpy

pointBoundary =  [ (-1, -1),
                   (-1, 1.0), 
                   ( 0, 1.0),
                   ( 0, -1),
                   ( 1,  1),
                   ( 1, -3)]

pointInner = [(0.5,0.5)]

points = pointBoundary# + pointInner

#segs = [(i, (i + 1) % len(pointBoundary)) for i in range(len(pointBoundary))]
segs = [(0, 1),(1,2),(2,3),(3,0), (2,4), (4,5),(5,3)]

# these are physical tags to apply on segments, same dimension as segs
_segs = [ 5,5,5,4,7,7,7]

t = triangle.Triangle()
t.set_points(points)

t.set_segments(segs,_segs)
#t.set_segments(segs)



# regions can be defined with a regional attribute 'r' at x,y coordinates.
# Moreover it is possible to specify the area constraint in that region with
# the fourth parmaters 'a'
# regions = [(x,y,r,a),...]

regions = [ (-0.5, 0.5,10,0.1),
            (0.5, 0.5,20,0.5)]


t.set_regions(regions)


t.triangulate(mode='qpzAe',area=1)

# a function to plot the triangulation, for fast check of regional attributes
#  within the mesh triangles
t.plot_mesh().show()

print(t.get_triangles())

# note that to have edges in "t" we need to use the "e" switch in
# triangulate! 
print(t.get_edges())
