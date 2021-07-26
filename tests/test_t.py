import triangle

t = triangle.Triangle()
pts = [(0.,0.), (1.,0.), (1.,1.)]
t.set_points(pts)
print(f'...points are set')
seg = [(0,1), (1,2), (2,0)]
t.set_segments(seg)
print(f'...segments are set')
t.triangulate(area=0.01, mode='pzq27eQ')
print('number of points/triangles: %d/%d' % 
    (t.get_num_points(), t.get_num_triangles()))
