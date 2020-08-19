# -*- coding: utf-8 -*-
import KBEngine
from KBEDebug import *

class MapManager(KBEngine.Entity):
    def __init__(self):
        KBEngine.Entity.__init__(self)
        KBEngine.setSpaceData(self.spaceID,"MetaClass",self.mapType)

