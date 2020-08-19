# -*- coding: utf-8 -*-
import KBEngine
from KBEDebug import *
class Monster(KBEngine.Entity):
    def __init__(self):
        KBEngine.Entity.__init__(self)

    def CellComeHere(self, arg):
        self.allClients.OnComeHere()

    def CellFollowMe(self, arg):
        self.allClients.OnFollowMe()

    def CellStopFollowMe(self, arg):
        self.allClients.OnStopFollowMe()
