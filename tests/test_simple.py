import triangle

def test_simple():

    # a single triangle
    t = triangle.Triangle()
    pts = [(0.,0.), (1.,0.), (1., 2.)]
    t.set_points(pts)
    seg = [(0,1), (1,2), (2,0)]
    t.set_segments(seg)
    t.triangulate(area=0.5, mode='pzq27eQ')
    print('number of points/triangles: %d/%d' % 
        (t.get_num_points(), t.get_num_triangles()))

if __name__ == '__main__':
    test_simple()
