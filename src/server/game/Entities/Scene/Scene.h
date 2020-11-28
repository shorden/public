/*
 * Copyright (C) 2011-2020 Project SkyFire <http://www.projectskyfire.org/>
 * Copyright (C) 2008-2020 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2020 MaNGOS <https://www.getmangos.eu/>
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

#ifndef SKYFIRESERVER_SCENE_H
#define SKYFIRESERVER_SCENE_H

#include "Object.h"

typedef std::set<uint64> GuidSet;

class Scene : public WorldObject, public GridObject<Scene>
{
public:
    Scene();
    ~Scene() { }

    void AddToWorld() override;
    void RemoveFromWorld() override;

    void Update(uint32 diff) override;
    void Remove();
    int32 GetDuration() const { return m_duration; }

    bool Create(uint32 sceneId, Map* map, Unit* creator, Position const& pos, GuidSet&& participants, SpellInfo const* spellInfo = nullptr);
    void AddParticipant(uint64 const& participantGuid);

private:
    ObjectGuid m_creatorGuid;
    uint32 m_duration;
    GuidSet m_participants;
};

#endif // SKYFIRESERVER_SCENE_H