#!/usr/bin/env python

import triangulate
import sys

"""
Interface to the TRIANGLE program by Jonathan Richard Shewchuck
"""

class Triangle:

    def __init__(self):

        """
        Constructor. 
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
        self.has_atts   = False

    def set_points(self, pts, markers=[]):

        """
        Set points. Use this to set the boundary points. Can
        also set interior points. 
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
        Invoke this method after 'set_points' to set the boundary
        contour. Here, segs is a list [ (i1,i2),.. ] of segments
        with i1,i2 being node indices. Ordering need not go
        counterclockwise.
        """

        triangulate.set_segments(self.hndls[0], segs)
        self.has_sgmts = True
        
    def set_holes(self, xy):

        """
        Optionally invoked when the domain contains holes. Here,
        xy = [ (x1, y1), ... ] where (x1,y1) is a point inside
        a hole
        """

        triangulate.set_holes(self.hndls[0], xy)

    def set_attributes(self, att):

        """
        Optionally invoked to set node attributes att=[(a1,..), ...]
        """
        if len(att) == 0:
            return
        triangulate.set_attributes(self.hndls[0], att)
        self.has_atts = True
        
    def triangulate(self, area=None, mode='pzq27eQ'):

        """
        Perform an initial triangulation. Invoke this after setting
        the boundary points, segments, and optionally hole positions.
        Here, area is a max area constraint and mode a string of TRIANGLE
        switches. Check the TRIANGLE doc for more info about mode:
        http://www.cs.cmu.edu/~quake/triangle.switch.html
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
        
    def refine(self, area_ratio=2.0):

        """
        Apply a refinement to the triangulation. Should be called after
        performing an initial triangulation. Here, area_ratio represents
        the max triangle area reduction factor.
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

    def get_nodes(self, level=-1):

        """
        Will return list [ [(x, y), marker], ...] where marker is 1
        on the boundary and 0 inside. Here, level can be used
        to retrieve previous triangulation refinements: level=-1
        will retrive the last, level=-2 the previous one, etc.
        """
        return triangulate.get_nodes(self.hndls[level])


    def get_edges(self, level=-1):

        """
        Return list of edges [((i1, i2), m),..)
        (i1,i2): node indices
        m: boundary marker (0=interior, 1=boundary)
        """
        return triangulate.get_edges(self.hndls[level])
        
    def get_triangles(self, level=-1):

        """
        Return list of triangles [([i1,i2,i3,..],(k1,k2,k3), [a1,a2,..]),..]
        i1,i2,i3,..: node indices at the triangle corners, optionally
        followed by intermediate nodes
        (k1,k2,k3): neighboring triangle indices
        a1,a2..: triangle cell attributes
        """
        return triangulate.get_triangles(self.hndls[level])
        
    def get_attributes(self, level=-1):

        """
        Will return node attributes [(a1,...), ....]. Here, level
        can be used to retrieve previous triangulation refinements.
        """
        return triangulate.get_attributes(self.hndls[-1])
        

        
        
        

        

    
