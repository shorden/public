/*
 * Copyright (C) 2011-2018 Project SkyFire <http://www.projectskyfire.org/>
 * Copyright (C) 2008-2018 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2018 MaNGOS <https://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "WorldSession.h"

void WorldSession::HandleQueryScenarioPOIOpcode(WorldPacket& recvPacket)
{
    std::vector<int32> MissingScenarioPOITreeIDs;

    MissingScenarioPOITreeIDs.resize(recvPacket.ReadBits(22));

    for (int i = 0; i < MissingScenarioPOITreeIDs.size(); ++i)
        MissingScenarioPOITreeIDs[i] = recvPacket.read<int32>();
}

void WorldSession::SendScenarioState()
{
    struct JamCriteriaProgress
    {
        int32       Id;                    ///< Offset 0x0000 Type _MJVTBL_INT32
        ObjectGuid  Quantity;              ///< Offset 0x0008 Type _MJVTBL_UINT64
        ObjectGuid  Player;                ///< Offset 0x0010 Type _MJVTBL_UINT64
        int32       Flags;                 ///< Offset 0x0018 Type _MJVTBL_INT32
        time_t      Date;                  ///< Offset 0x001C Type _MJVTBL_NOT_IMPLEMENTED
        uint32      TimeFromStart;         ///< Offset 0x0040 Type _MJVTBL_TIME_T
        uint32      TimeFromCreate;        ///< Offset 0x0048 Type _MJVTBL_TIME_T
    };

    int32                              BonusObjectiveID;                          ///< Offset 0x0000 Type _MJVTBL_INT32
    bool                               ObjectiveComplete;                         ///< Offset 0x0004 Type _MJVTBL_BOOL
    bool                               ScenarioComplete;                          ///< Offset 0x0018 Type _MJVTBL_BOOL
    std::vector<JamCriteriaProgress>   CriteriaProgress;                          ///< Offset 0x0020 Type MakeMJVTblForArrayOfStruct<JamCriteriaProgress>::MJVTBL
    int32                              CurrentStep;                               ///< Offset 0x0038 Type _MJVTBL_INT32
    uint32                             WaveCurrent;                               ///< Offset 0x003C Type _MJVTBL_UINT32
    uint32                             DifficultyID;                              ///< Offset 0x0040 Type _MJVTBL_UINT32
    uint32                             TimerDuration;                             ///< Offset 0x0044 Type _MJVTBL_UINT32
    int32                              ScenarioID;                                ///< Offset 0x0048 Type _MJVTBL_INT32
    uint32                             WaveMax;                                   ///< Offset 0x004C Type _MJVTBL_UINT32

    WorldPacket data(SMSG_SCENARIO_STATE, 4 + 1 + 1 + (CriteriaProgress.size() * (4 + 8 + 8 + 4 + 4 + 4 + 4)) + 4 + 4 + 4 + 4 + 4 + 4);

    data << WaveCurrent;
    data << TimerDuration;
    data << WaveMax;
    data << ScenarioID;
    data << DifficultyID;
    data << BonusObjectiveID;
    data << CurrentStep;

    data.WriteBits(CriteriaProgress.size(), 19);
    data.WriteBit(ScenarioComplete);

    for (auto const& progress : CriteriaProgress)
    {
        data.WriteGuidMask(progress.Quantity, 3, 5);
        data.WriteGuidMask(progress.Player, 2);
        data.WriteGuidMask(progress.Quantity, 6);
        data.WriteGuidMask(progress.Player, 7);
        data.WriteGuidMask(progress.Quantity, 1, 7, 0);
        data.WriteGuidMask(progress.Player, 3);
        data.WriteGuidMask(progress.Quantity, 4);
        data.WriteGuidMask(progress.Player, 0, 5, 1, 4);
        data.WriteGuidMask(progress.Quantity, 2);
        data.WriteGuidMask(progress.Player, 6);

        data.WriteBits(progress.Flags, 4);
    }

    data.WriteBit(ObjectiveComplete);

    data.FlushBits();

    for (auto const& progress : CriteriaProgress)
    {
        data.WriteGuidBytes(progress.Player, 3, 7, 5);
        data << progress.TimeFromStart;
        data.WriteGuidBytes(progress.Player, 2);
        data << progress.TimeFromCreate;
        data.WriteGuidBytes(progress.Player, 6, 4);
        data.AppendPackedTime(progress.Date);
        data.WriteGuidBytes(progress.Player, 1);
        data.WriteGuidBytes(progress.Quantity, 3, 2, 1, 5, 4, 7);
        data.WriteGuidBytes(progress.Player, 0);
        data.WriteGuidBytes(progress.Quantity, 6, 0);
        data << progress.Id;
    }

    SendPacket(&data);
}

void WorldSession::SendScenarioProgressUpdate()
{
    int32      Id;                    ///< Offset 0x0000 Type _MJVTBL_INT32
    ObjectGuid Quantity;              ///< Offset 0x0008 Type _MJVTBL_UINT64
    ObjectGuid Player;                ///< Offset 0x0010 Type _MJVTBL_UINT64
    int32      Flags;                 ///< Offset 0x0018 Type _MJVTBL_INT32
    time_t     Date;                  ///< Offset 0x001C Type _MJVTBL_NOT_IMPLEMENTED
    uint32     TimeFromStart;         ///< Offset 0x0040 Type _MJVTBL_TIME_T
    uint32     TimeFromCreate;        ///< Offset 0x0048 Type _MJVTBL_TIME_T

    WorldPacket data(SMSG_SCENARIO_PROGRESS_UPDATE, 4 + 8 + 8 + 4 + 4 + 4 + 4);

    data.WriteGuidMask(Player, 6, 4, 5);
    data.WriteGuidMask(Quantity, 1, 5, 6, 7, 4);
    data.WriteGuidMask(Player, 1);
    data.WriteGuidMask(Quantity, 0);
    data.WriteGuidMask(Player, 2, 3);
    data.WriteGuidMask(Quantity, 2, 3);
    data.WriteGuidMask(Player, 0);
    data.WriteBits(Flags, 4);
    data.WriteGuidMask(Player, 7);

    data.FlushBits();

    data.WriteGuidBytes(Quantity, 5, 2);
    data.WriteGuidBytes(Player, 7, 4);
    data.WriteGuidBytes(Quantity, 4, 0);
    data.WriteGuidBytes(Player, 1, 2);
    data << TimeFromStart;
    data.WriteGuidBytes(Quantity, 3);
    data << Id;
    data.WriteGuidBytes(Quantity, 1);
    data.AppendPackedTime(Date);
    data << TimeFromCreate;
    data.WriteGuidBytes(Player, 0, 3, 6);
    data.WriteGuidBytes(Quantity, 7, 6);
    data.WriteGuidBytes(Player, 5);

    SendPacket(&data);
}

void WorldSession::SendScenarioPOIs()
{
    struct ScenarioPOIData
    {
        struct ScenarioBlobData
        {
            struct ScenarioPOIPointData
            {
                int32 X;            ///< Offset 0x0000 Type _MJVTBL_INT32
                int32 Y;            ///< Offset 0x0004 Type _MJVTBL_INT32
            };

            int32                             BlobID;                                   ///< Offset 0x0000 Type _MJVTBL_INT32
            int32                             MapID;                                    ///< Offset 0x0004 Type _MJVTBL_INT32
            int32                             WorldMapAreaID;                           ///< Offset 0x0008 Type _MJVTBL_INT32
            int32                             Floor;                                    ///< Offset 0x000C Type _MJVTBL_INT32
            int32                             Priority;                                 ///< Offset 0x0010 Type _MJVTBL_INT32
            int32                             Flags;                                    ///< Offset 0x0014 Type _MJVTBL_INT32
            int32                             WorldEffectID;                            ///< Offset 0x0018 Type _MJVTBL_INT32
            int32                             PlayerConditionID;                        ///< Offset 0x001C Type _MJVTBL_INT32
            std::vector<ScenarioPOIPointData> Points;                                   ///< Offset 0x0020 Type MakeMJVTblForArrayOfStruct<ScenarioPOIPointData>::MJVTBL
        };

        int32                         CriteriaTreeID;                       ///< Offset 0x0000 Type _MJVTBL_INT32
        std::vector<ScenarioBlobData> BlobData;                             ///< Offset 0x0008 Type MakeMJVTblForArrayOfStruct<ScenarioBlobData>::MJVTBL
    };

    std::vector<ScenarioPOIData> PoiInfo;                             ///< Offset 0x0000 Type MakeMJVTblForArrayOfStruct<ScenarioPOIData>::MJVTBL

    WorldPacket data(SMSG_SCENARIO_POIS, 2 * (4 + 4 + 4 + 4 + 4 + 4 + 4 + 4 + 4 + 4 + 4));

    data.WriteBits(PoiInfo.size(), 21);

    for (auto const& poiData : PoiInfo)
    {
        data.WriteBits(poiData.BlobData.size(), 19);

        for (auto const& blob : poiData.BlobData)
            data.WriteBits(blob.Points.size(), 21);
    }

    data.FlushBits();

    for (auto const& poiData : PoiInfo)
    {
        data.WriteBits(poiData.BlobData.size(), 19);

        for (auto const& blob : poiData.BlobData)
        {
            data << blob.BlobID;
            data << blob.Floor;
            data << blob.WorldEffectID;

            for (auto const& point : blob.Points)
            {
                data << point.Y;
                data << point.X;
            }

            data << blob.Flags;
            data << blob.PlayerConditionID;
            data << blob.MapID;
            data << blob.Priority;
            data << blob.WorldMapAreaID;
        }

        data << poiData.CriteriaTreeID;
    }
}