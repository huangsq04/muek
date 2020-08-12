# -*- coding: utf-8 -*-
import KBEngine
from KBEDebug import *

class Agent(KBEngine.Entity):
    def __init__(self):
        KBEngine.Entity.__init__(self)
        #观察范围是前后300米
        self.setViewRadius(300, 5.0)

    def SetAgentLocation(self, arg, Position):
        self.position = Position
        self.client.OnSetAgentLocation()

    def CreateMonster(self, arg, EntityClass, PlayerName, Position):
        """
        创建实体
        """
        Direction = ( 0.0, 0.0, 0.0 )
        Properties = {"PlayerName":PlayerName}
        KBEngine.createEntity( EntityClass, self.spaceID, Position, Direction, Properties ) 

    def CreateControllMonster(self, arg, Arg0):
        TempDict = {}
        for Item in Arg0:
            TempDict[Item["PlayerName"]] = Item
        for Key, Entity in KBEngine.entities.items():
            if Entity.className != "Monster":
                continue
            if Entity.PlayerName in TempDict:
                Entity.controlledBy = self.base
                TempDict[Entity.PlayerName] = None

        for Key, Item in TempDict.items():
            if Item == None:
                continue
            Direction = ( 0.0, 0.0, 0.0 )
            Properties = {"PlayerName":Key}
            Entity = KBEngine.createEntity( Item["ClassName"], self.spaceID, Item["Position"], 
                    Direction, Properties ) 
            Entity.controlledBy = self.base
