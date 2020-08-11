# -*- coding: utf-8 -*-
import KBEngine
from KBEDebug import *

class Role(KBEngine.Proxy):
    def __init__(self):
        KBEngine.Proxy.__init__(self)
        self.parent = None
        
    def OnLeaveWorld(self):
        if self.cell is not None:
            self.destroyCellEntity()
        self.giveClientTo( self.parent )

    def logoff(self):
        INFO_MSG( "(%i): OnLogout." %self.id )
        self.onClientDeath()

    def onLoseCell( self ):
        self.destroy()

    def onClientDeath(self):
        INFO_MSG( "(%i): onClientDeath." %self.id )
        if self.cell is not None:
            self.destroyCellEntity()
        if self.parent != None:
            self.parent.destroy()
        
