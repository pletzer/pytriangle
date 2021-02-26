#!/usr/bin/env python

import triangulate
import sys

"""
Interface to the TRIANGLE program by Jonathan Richard Shewchuck
"""

class Triangle:


    def __init__(self):

        """
        Constructor
        """

        # create handles to hold the
        # triangulation structures
        self.hndls = [triangulate.new(),]
        self.h_vor =  triangulate.new()
        
        self.area  = None
        self.mode  = ''

        self.has_points = False
        self.has_segmts = False
        self.has_trgltd = False


    def set_points(self, pts, markers=[]):

        """
        Set the points

        @param pts [(x, y),...]
        @param markers [m, ...] where m is 1 on the outer boundary and 0 in the interior or internal boundary)
        """

        if not markers:
            # set all the markers to zero
            mrks = [0 for i in range(len(pts))]
        else:
            npts = len(pts)
            nmrk = len(markers)
            if npts != nmrk:
                print('%s: Warning. Incompatible size between marker and point lists len(pts)=%d != len(markers)=%d.' % \
                      (__file__, npts, nmrk))
                n1 = min(npts, nmrk)
                n2 = npts - nmrk
                mrks = [markers[i] for i in range(n1)] + [0 for i in range(n2)]
            else:
                mrks = markers
                
        triangulate.set_points(self.hndls[0], pts, mrks)
        self.has_points = True


    def set_segments(self, segs):

        """
        Set the boundary contour. 

        @param segs [(p0, p1), ....] where p0 and p1 are point indices. The ordering is counterclockwise for an outer boundary
                    and clockwise for an internal boundary.

        @note invoke this method after 'set_points'.
        """
        triangulate.set_segments(self.hndls[0], segs)
        self.has_sgmts = True
        

    def set_holes(self, xy):

        """
        Set the list of points in the holes. 

        @param xy [ (x0, y0), ... ] where (x0,y0) is a point inside a hole
        """
        triangulate.set_holes(self.hndls[0], xy)

    def set_regions(self, xy):

        """
        Set the list of regions. 

        @param xy [ (x0, y0, r, a), ... ] where (x0,y0) is a point inside a region
                                                r is the region attribute (tag)
                                                a is the area constraint
        """
        triangulate.set_regions(self.hndls[0], xy)


    def set_point_attributes(self, att):

        """
        Set the point attributes.

        @param att [(a0,..), ...]
        """
        triangulate.set_point_attributes(self.hndls[0], att)
        

    def set_triangle_attributes(self, att):

        """
        Set the triangle attributes.

        @param att [(a0,..), ...]
        """
        triangulate.set_triangle_attributes(self.hndls[1], att)


    def triangulate(self, area=None, mode='pzq27eQ'):

        """
        Perform an initial triangulation.

        @param area is a max area constraint
        @param mode a string of TRIANGLE switches. Refer to the TRIANGLE doc for more info about mode:
        http://www.cs.cmu.edu/~quake/triangle.switch.html

        @note invoke this after setting the boundary points, segments, and optionally hole positions.
        """

        if not self.has_points and not self.has_segmts:
            print('%s: Error. Must set points, segments, and optionally holes prior to calling "triangulate"' \
                  % (__file__))
            return

        # mode string
        # z: start indexing with zero
        # q<angle>: quality mesh
        # e: edge
        # p: planar straight line graph
        # Q: quiet mode

        self.mode = mode
        if area:
            self.area = area
            mode += 'a%f'% area

        if len(self.hndls) <= 1: self.hndls.append( triangulate.new() )
        triangulate.triangulate(mode, self.hndls[0], self.hndls[1], self.h_vor)
        self.has_trgltd = True


    def get_num_points(self, level=-1):
        """
        Get the number of nodes/points.

        @param level refinement level (-1 for the last level). The coarsest level is 1.
        @return number
        """
        return triangulate.get_num_points(self.hndls[level])

        
    def get_num_triangles(self, level=-1):
        """
        Get the number of cells/triangles.

        @param level refinement level (-1 for the last level). The coarsest level is 1
        @return number
        """
        return triangulate.get_num_triangles(self.hndls[level])


    def refine(self, area_ratio=2.0):

        """
        Refine the triangulation.

        @param area_ratio represents the max triangle area reduction factor

        @note should be called after performing an initial triangulation.
        """

        if not self.has_trgltd:
            print('%s: Error. Must triangulate prior to calling "refine"' \
                  % (__file__))
            return

        self.hndls.append( triangulate.new() )

        mode = self.mode + 'cr'
        if self.area:
            self.area /= area_ratio
            mode += 'a%f' % self.area

        triangulate.triangulate(mode, self.hndls[-2],
                                self.hndls[-1], self.h_vor)


    def get_points(self, level=-1):

        """
        Get the points and their markers.

        @param level refinement level (-1 for the last level). The coarsest level is 1. The level can be used
                     to retrieve previous triangulation refinements: level=-1 will retrieve the last, 
                     level=-2 the previous one, etc.
        @return [ [(x, y), marker], ...] where marker is 1 on the boundary and 0 inside. Here, 
        """
        return triangulate.get_points(self.hndls[level])


    def get_edges(self, level=-1):

        """
        Get the list of edges.

        @param level refinement level (-1 for the last level). The coarsest level is 1. The level can be used
                     to retrieve previous triangulation refinements: level=-1 will retrieve the last, 
                     level=-2 the previous one, etc.
        @return [((p0, p1), m),..) where (p0, p1) are point indices and 
        m is the boundary marker (0=interior, 1=boundary)
        """
        return triangulate.get_edges(self.hndls[level])
        

    def get_triangles(self, level=-1):

        """
        Get the list of triangles.

        @param level refinement level (-1 for the last level). The coarsest level is 1. The level can be used
                     to retrieve previous triangulation refinements: level=-1 will retrieve the last, 
                     level=-2 the previous one, etc.
        @return [([p0, p1, p2,..], (k0,k1,k2), [a0,a1,..]),..] where p0, p1, p2,.. are the 
        point indices at the triangle corners, optionally followed by intermediate points (k0, k1, k2) and triangle cell attributes a1,a2.. 
        """
        return triangulate.get_triangles(self.hndls[level])
        

    def get_point_attributes(self, level=-1):

        """
        Get the point attributes.

        @param level refinement level (-1 for the last level). The coarsest level is 1. The level can be used
                     to retrieve previous triangulation refinements: level=-1 will retrieve the last, 
                     level=-2 the previous one, etc.
        @return [(a0,...), ....]
        """
        return triangulate.get_point_attributes(self.hndls[level])
        

    def get_triangle_attributes(self, level=-1):

        """
        Get the triangle attributes.

        @param level refinement level (-1 for the last level). The coarsest level is 1. The level can be used
                     to retrieve previous triangulation refinements: level=-1 will retrieve the last, 
                     level=-2 the previous one, etc.
        @return [(a0,...), ....]
        """
        return triangulate.get_triangle_attributes(self.hndls[level])


    # backward compatibility
    get_num_nodes = get_num_points
    set_nodes = set_points
    get_nodes = get_points
    set_attributes = set_point_attributes
    get_attributes = get_point_attributes
        
        
    # add some visualization capability to fast check the mesh
    
    
    def plot_mesh(self,level=-1):
        import matplotlib.pyplot as plt
        from matplotlib.path import Path
        import matplotlib.patches as patches
        
        mesh = self.get_triangles(level)
        points = self.get_points(level)
        verts = []
        codes = []
 
        fig, ax = plt.subplots()
        
        for _t in mesh:
            #([n1, n2, n3], (), [regiona_tag])
            p = _t[0]
            x1,y1 = points[p[0]][0]
            x2,y2 = points[p[1]][0]
            x3,y3 = points[p[2]][0]

            verts.append((x1, y1))
            verts.append((x2, y2))
            verts.append((x3, y3))
            verts.append((x1, y1))
            codes.append(Path.MOVETO)
            codes.append(Path.LINETO)
            codes.append(Path.LINETO)
            codes.append(Path.CLOSEPOLY)
            
            ax.text((x1+x2+x3)/3, (y1+y2+y3)/3, str(_t[2]))
        
        path = Path(verts, codes)

        patch = patches.PathPatch(path, facecolor='None', lw=1)
        ax.add_patch(patch)
        ax.margins(0.05)
        ax.axis('equal')

        return plt
    



    
    
    
