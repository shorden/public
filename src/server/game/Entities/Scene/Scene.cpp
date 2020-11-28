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

#include "Scene.h"
#include "Creature.h"
#include "Log.h"
#include "Map.h"
#include "ScriptMgr.h"
#include "Unit.h"
#include "UpdateData.h"

Scene::Scene() : WorldObject(false), m_duration(0)
{
    m_objectType |= TYPEMASK_SCENEOBJECT;
    m_objectTypeId = TypeID::TYPEID_SCENEOBJECT;

    m_updateFlag = UPDATEFLAG_STATIONARY_POSITION /*| UPDATEFLAG_SCENE_OBJECT*/;
}

void Scene::AddToWorld()
{
    ///- Register the Scene for guid lookup
    if (!IsInWorld())
        sObjectAccessor->AddObject(this);

    Object::AddToWorld();
}

void Scene::RemoveFromWorld()
{
    ///- Remove the Ccene from the accessor
    if (IsInWorld())
        sObjectAccessor->RemoveObject(this);

    Object::RemoveFromWorld();
}

void Scene::Update(uint32 diff)
{
    if (GetDuration() > int32(diff))
        m_duration -= diff;
    else
        Remove(); // expired

    WorldObject::Update(diff);
}

void Scene::Remove()
{
    if (IsInWorld())
        AddObjectToRemoveList(); // calls RemoveFromWorld
}

bool Scene::Create(uint32 SceneId, Map* map, Unit* creator, Position const& pos, GuidSet&& participants, SpellInfo const* /*spellInfo = nullptr*/)
{
    uint32 lowGuid = sObjectMgr->GenerateLowGuid(HIGHGUID_SCENEOBJECT);

    m_creatorGuid = creator->GetGUID();
    m_participants = std::move(participants);

    Object::_Create(lowGuid, 0, HIGHGUID_SCENEOBJECT);

    SetMap(map);
    Relocate(pos);

    SetEntry(SceneId);
    SetObjectScale(1.0f);

    if (!GetMap()->AddToMap(this))
        return false;

    return true;
}

void Scene::AddParticipant(uint64 const& participantGuid)
{
    m_participants.insert(participantGuid);
}