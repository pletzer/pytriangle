
import os
import sys

sys.path.insert(0, os.getcwd())

import math
import triangle
import numpy
    
def xtest_bad():
    
    pts =  [(0.0, 0.0), (0.00012747326699555811, 0.0), 
            (-0.0084667962363287295, 0.0035466702907514828), 
            (-0.017061080350639229, 0.0070933376551251849), 
            (-0.025655349853963515, 0.010640007945876669), 
            (-0.034249619357287805, 0.014186678236628149), 
            (-0.042843896166105191, 0.01773334706419074), 
            (-0.042843896166105191, 0.01773334706419074), 
            (-0.034275119855081401, 0.014186677066077038), 
            (-0.025706336238564498, 0.010640008531152224), 
            (-0.017137552622047594, 0.0070933399962274102), 
            (-0.0085687836165169017, 0.0035466685349248139)]
    segs = [(i, (i + 1) % len(pts)) for i in range(len(pts))]
    t = triangle.Triangle()
    t.set_points(pts)
    t.set_segments(segs)
    t.triangulate(mode='pz', area=None)


def test_simple():

    t = triangle.Triangle()
    print('number of points/triangles: %d/%d' % 
        (t.get_num_points(), t.get_num_triangles()))

def test_simple2():

    # a single triangle
    t = triangle.Triangle()
    pts = [(0.,0.), (1.,0.), (1., 2.)]
    t.set_points(pts)
    seg = [(0,1), (1,2), (2,0)]
    t.set_segments(seg)
    t.triangulate(area=0.5, mode='pzq27eQ')
    print('number of points/triangles: %d/%d' % 
        (t.get_num_points(), t.get_num_triangles()))

    
def test_circle_with_hole():
    
    # number of outer points
    nto = 32
    dto = 2*math.pi/float(nto)
    
    # number of inner points
    nti = 16
    dti = 2*math.pi/float(nti)

    # construct the outer boundary
    ptso = [(math.cos(i*dto), math.sin(i*dto)) for i in range(nto)]
    # set outer markers to one
    mrko = [1 for i in range(nto)]

    # construct the inner boundary
    ptsi = [(0.5 + 0.1*math.cos(i*dti), 0.1*math.sin(i*dti)) \
            for i in range(nti)]
    # set inner markers to zero
    mrki = [0 for i in range(nti)]

    # add some points inside the domain
    pts_inside = [(0., 0.), (0.1, 0.)]
    
    # outer segments, loop closes
    sgo = [(i, i + 1) for i in range(nto - 1)] + [(nto - 1, 0)]
    # inner segments, loop closes
    sgi = [(i, i + 1) for i in range(nto, nto + nti - 1)] + [(nto + nti - 1, nto)]
    
    # set all points
    pts = ptso + ptsi + pts_inside
    # all segments
    seg = sgo + sgi
    # set all markers
    mrk = mrko + mrki + [-1 for i in range(len(pts_inside))]

    # set attributes
    att = [ (p[0], p[1], p[0]**2,) for p in pts ]

    # punch a hole, any point inside the hole will do
    hls = [(0.5, 0.)]

    # triangulate
    t = triangle.Triangle()
    t.set_points(pts, mrk)
    t.set_segments(seg)
    t.set_holes(hls)
    t.set_point_attributes(att)

    # checking backward compatibility
    t.set_nodes(pts, mrk)  # same as set_points
    t.set_attributes(att) # same as set_point_attributes

    t.triangulate(area=0.01, mode='pzq27eQ')
    print('number of points/triangles before refinement: %d/%d' % \
                           (t.get_num_points(), t.get_num_triangles()))

    pts1 = t.get_points(level=1)
    ijks1 = [tri[0] for tri in t.get_triangles(level=1)]

    # mid positions of the triangles
    xy_mid1 = [ ((pts1[ijk[0]][0][0] + pts1[ijk[1]][0][0] + pts1[ijk[2]][0][0])/3.,
                 (pts1[ijk[0]][0][1] + pts1[ijk[1]][0][1] + pts1[ijk[2]][0][1])/3.) for ijk in ijks1]

    t.set_triangle_attributes(xy_mid1)
    
    # refine multiple times the triangulation
    for i in range(10):
        t.refine(1.2)

    print('number of points/triangles after refinement: %d/%d' % \
                           (t.get_num_points(), t.get_num_triangles()))
    print('number of points/triangles for coarsest level: %d/%d' % \
                           (t.get_num_points(1), t.get_num_triangles(1)))
    
    # take the last level
    points = t.get_points(level=-1)
    point_attributes = t.get_point_attributes(level=-1)

    # checking backwards compatibility for point attributes
    point_attributes = t.get_attributes(level=-1)

    triangle_attributes = t.get_triangle_attributes(level=-1)
    
    # compute the interpolation error
    error = 0.
    for i in range(len(points)):
        x, y = points[i][0]
        error += (point_attributes[i][0] - x)**2 + (point_attributes[i][1] - y)**2
    error = math.sqrt(error/float(len(pts)))
    print('error = %g' % error)
    assert(abs(error) < 1.e-10)

  

if __name__ == '__main__':
    test_simple()
    test_simple2()
    test_circle_with_hole()
