import triangle

def test_simple():

    # a single triangle
    t = triangle.Triangle()
    pts = [(0.,0.), (1.,0.), (1., 2.)]
    sgs = [(0,1), (1,2), (2,0),]
    t.set_points(pts)
    t.set_segments(sgs)
    t.triangulate(area=1.0, mode='pzq10eQ')
    print('number of points/triangles: %d/%d' %
        (t.get_num_points(), t.get_num_triangles()))

if __name__ == '__main__':
    test_simple()
