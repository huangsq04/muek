# -*- coding: utf-8 -*-
import KBEngine
from KBEDebug import *

class Role(KBEngine.Entity):
    def __init__(self):
        KBEngine.Entity.__init__(self)

    def LevelTeleport(self, arg, MapName, Position):
        """
        客户端调用传送到指定地图
        """
        nid = KBEngine.globalData[MapName].cell.id
        self.Map = MapName
        if nid in KBEngine.entities:
            self.teleport(KBEngine.entities[nid], Position, (0,0,0)) 

    def CreateMonster(self, arg, PlayerName, Position):
        """
        创建实体
        """
        Direction = ( 0.0, 0.0, 0.0 )
        Properties = {"PlayerName":PlayerName}
        KBEngine.createEntity( entityclass, self.spaceID, Position, Direction, Properties ) 

