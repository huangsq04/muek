# -*- coding: utf-8 -*-
import KBEngine
from KBEDebug import *

class MapManager(KBEngine.Proxy):
    def __init__(self):
        KBEngine.Proxy.__init__(self)
        self.cellData["position"] = (0.0, 0.0, 0.0)
        self.createCellEntityInNewSpace(None)
        
    def onGetCell( self ):
        """
        创建cell实体
        """
        INFO_MSG("MapManager onGetCell Entity")

