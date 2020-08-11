# -*- coding: utf-8 -*-
import KBEngine
from KBEDebug import *

class MapManager(KBEngine.Entity):
    def __init__(self):
        KBEngine.Entity.__init__(self)
        KBEngine.setSpaceData(self.spaceID,"MetaClass",self.mapType)
        properties = {"PlayerName":"M%d" % self.spaceID}
        KBEngine.createEntity( "Monster", 1, (-30.564699, 2.283600,13), (0,0,0), properties)

