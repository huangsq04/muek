# -*- coding: utf-8 -*-
import KBEngine
from KBEDebug import *
class Monster(KBEngine.Entity):
    def __init__(self):
        KBEngine.Entity.__init__(self)

    def ControllMonster(self, arg, ID):
        """
        控制一个实体
        """
        self.controlledBy = KBEngine.entities[ID].base

