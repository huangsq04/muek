# -*- coding: utf-8 -*-
import KBEngine
from KBEDebug import *

class Account(KBEngine.Proxy):
    def __init__(self):
        KBEngine.Proxy.__init__(self)
        self.PlayerName = self.__ACCOUNT_NAME__
        self.map_ = ""
        #以名字判断AuthorityType
        if self.PlayerName.startswith("Agent_"):
            self.AuthorityType = 1

            
    def PlayerEnterCallBack(self, baseRef, databaseID,wasActive):
        player = baseRef
        if self.cell is None:
            cellMailbox = KBEngine.globalData[self.map_]
            INFO_MSG( "PlayerEnterCallBack: (%s)." % self.map_ )
            if cellMailbox is not None and cellMailbox.cell is not None:
                self.giveClientTo( player )
                player.parent = self
                player.createCellEntity( cellMailbox.cell )

    def PlayerEnter(self, dbid, mapname):
        """
        玩家进入场景
        dbid 角色数据库id
        mapname  地图名称
        """
        INFO_MSG( "(%i)-(%s): PlayerEnter." % (self.AuthorityType, mapname) )
        self.map_ = mapname
        if self.AuthorityType == 0:
            #正常角色
            KBEngine.createEntityFromDBID( "Role", dbid, self.PlayerEnterCallBack)
        elif self.AuthorityType == 1: 
            #代理
            KBEngine.createEntityFromDBID( "Agent", dbid, self.PlayerEnterCallBack)

    def Logoff(self):
        INFO_MSG( "(%i): logoff." %self.id )
        if self.cell is not None:
            self.destroyCellEntity()
        else:
            self.destroy()


    def onLogOnAttempt(self, ip, port, password):
        INFO_MSG(ip, port, password)
        return KBEngine.LOG_ON_ACCEPT

    def OnCreatePlayer(self, name):
        """
        玩家创建角色
        """
        if name == "":
            self.client.RespCreatePlayer(-2)
        else:
            paramDict = {"PlayerName" : name, 
                         "ParentDBID" : self.databaseID,
                         "Map" : "MapA",
                         "position" : (0,100,0)}

            PlayerType = "Role"
            if self.AuthorityType == 1:
                PlayerType = "Agent"

            avatar = KBEngine.createEntityLocally( PlayerType, paramDict )
            avatar.writeToDB( self.OnCreatePlayerCallback )

    def OnCreatePlayerCallback( self, success, avatar ):
        if success:
            item = {"id" : avatar.databaseID, "PlayerName" : avatar.cellData["PlayerName"] , \
                    "SpaceName":"MapA"}
            #只有赋值才能把属性同步到客户端
            self.client.RespCreatePlayer(avatar.databaseID, item["PlayerName"])
        else:
            self.client.RespCreatePlayer(-1, "Error")

        # destroy entity
        avatar.destroy( writeToDB = False ) 

    def GetRoleInfo(self):

        PlayerType = "tbl_Role"
        if self.AuthorityType == 1:
            PlayerType = "tbl_Agent"

        query = "SELECT `id`, `sm_PlayerName` , `sm_Map` FROM `%s` WHERE `sm_ParentDBID` = %d" \
                % (PlayerType, self.databaseID)

        KBEngine.executeRawDatabaseCommand( query, self.OnGetRoleInfoCallback )

    def OnGetRoleInfoCallback( self, resultSet, rows, insertid, errstr ):
        v = {"Items":[]}
        for Item in resultSet:
            v["Items"].append({"Id": int(Item[0]), "Name":"\"%s\"" % Item[1], "Map":"\"%s\"" % Item[2]})
        self.RoleList = v
        self.client.RespGetRoleInfo(v)




