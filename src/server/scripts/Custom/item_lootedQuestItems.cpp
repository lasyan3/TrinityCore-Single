#include "Define.h"
#include "ScriptMgr.h"
#include "SharedDefines.h"
#include "Unit.h"
#include "MapManager.h"
#include "Player.h"
#include "GossipDef.h"
#include "ScriptedGossip.h"

class item_lootedQuestItems : public UnitScript
{
	struct StrItemList
	{
		std::string name;
		uint32 requiredCount;
	};
	typedef std::list<struct StrItemList> ItemList;
	enum EntityType
	{
		NPC_STARTER = 0,
		ITEM_STARTER = 1,
		NPC_ENDER = 2
	};
	struct StrQuestInfo
	{
		EntityType entity_type;
		std::string entity, quest, area, zone;
		ItemList items;
	};
	typedef std::unordered_map<uint32, struct StrQuestInfo> GossipItemMap;
public:
	item_lootedQuestItems() : UnitScript("item_lootedQuestItems") {}
	GossipItemMap m_entries;
	uint8 m_displayMode;

	bool OnUse(Player* player, Item* item, SpellCastTargets const& targets) // Any hook here
	{
		m_entries.clear();
		player->PlayerTalkClass->ClearMenus();

		for (uint8 i = INVENTORY_SLOT_ITEM_START; i < INVENTORY_SLOT_ITEM_END; ++i)
		{
			Item* pItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i);
			if (pItem == NULL) continue;
			ProcessItem(player, pItem);
		}

		for (uint8 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; ++i)
		{
			Bag* pBag = player->GetBagByPos(i);
			if (pBag == NULL) continue;
			for (uint32 j = 0; j < pBag->GetBagSize(); ++j)
			{
				Item* pItem = player->GetItemByPos(i, j);
				if (pItem == NULL) continue;
				ProcessItem(player, pItem);
			}
		}
		DisplayQuests(player);
		/*player->ADD_GOSSIP_ITEM(0, "Display by quest", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
		player->ADD_GOSSIP_ITEM(0, "Display by quest's giver", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
		player->ADD_GOSSIP_ITEM(0, "Display by zone's quest's giver", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
		player->ADD_GOSSIP_ITEM(0, "Display by item", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);*/
		player->PlayerTalkClass->SendGossipMenu(DEFAULT_GOSSIP_MESSAGE, item->GetGUID());
		return false;
	}

	void DisplayQuests(Player* pPlayer, uint32 pQuestId = 0)
	{
		for (auto itr = m_entries.begin(); itr != m_entries.end(); ++itr)
		{
			std::ostringstream msg;
			//msg << "|cffFF2020" << itr->second.quest << "\r\n";
			msg << itr->second.quest;
			for (auto it = itr->second.items.begin(); it != itr->second.items.end(); ++it)
			{
				msg << "\r\n  - " << it->name.c_str();
				if (it->requiredCount > 0)
					msg << " (" << it->requiredCount << ")";
			}
			if (pQuestId > 0 && itr->first == pQuestId)
			{
				if (itr->second.entity.size() > 0)
				{
					switch (itr->second.entity_type)
					{
					case NPC_STARTER: msg << "\r\nGiven by ";  break;
					case NPC_ENDER: msg << "\r\nEnded by "; break;
					case ITEM_STARTER: msg << "\r\nStarted with "; break;
					}
					msg << itr->second.entity;
					if (itr->second.area.size() > 0) msg << " from " << itr->second.area;
					if (itr->second.zone.size() > 0) msg << " (in " << itr->second.zone << ")";
					//msg << "\r\n-----------------------------------------------";
				}
				else
				{
					msg << "|cffFF0000\r\nNo information found for this quest!";
				}
			}
			pPlayer->ADD_GOSSIP_ITEM(0, msg.str(), itr->first, GOSSIP_ACTION_INFO_DEF + 1);
		}
	}

	void OnGossipSelect(Player* player, Item* item, uint32 sender, uint32 action)
	{
		player->PlayerTalkClass->ClearMenus();
		TC_LOG_DEBUG("lasyan3", "Action=%d, Sender=%d", action, sender);
		/*if (sender == GOSSIP_SENDER_MAIN)
		{
		m_displayMode = action;
		DisplayQuests(player);
		} else*/ if (sender != GOSSIP_SENDER_INFO && action == GOSSIP_ACTION_INFO_DEF + 1)
		{
			DisplayQuests(player, sender);
		}
		else if (sender == GOSSIP_SENDER_INFO && action == GOSSIP_ACTION_INFO_DEF + 9)
		{
			DisplayQuests(player);
		}
		//player->ADD_GOSSIP_ITEM(0, "ERROR BAD ENTRY!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
		player->PlayerTalkClass->SendGossipMenu(DEFAULT_GOSSIP_MESSAGE, item->GetGUID());
	}
private:

	void ProcessItem(Player * pPlayer, Item *pItem)
	{
		if (pItem->GetTemplate()->Class == ITEM_CLASS_QUEST && !pPlayer->HasQuestForItem(pItem->GetTemplate()->ItemId))
		{
			ObjectMgr::QuestMap _allQuests = pPlayer->GetAvailableQuestsForItem(pItem->GetTemplate()->ItemId);
			if (_allQuests.size() > 0)
			{
				ObjectMgr::QuestMap::const_iterator iter = _allQuests.begin();
				uint32 questid = iter->first;
				Quest const* qInfo = iter->second;
				//TC_LOG_INFO("lasyan3", "Quest %s, status = %d", qInfo->GetTitle().c_str(), pPlayer->GetQuestStatus(qInfo->GetQuestId()));

				// Get item count
				uint32 itemCountNeeded = 0;
				for (uint8 i = 0; i < QUEST_ITEM_OBJECTIVES_COUNT; i++)
				{
					if (qInfo->RequiredItemId[i] != pItem->GetTemplate()->ItemId)
						continue;

					if (qInfo->RequiredItemCount[i] != 0 && pPlayer->GetItemCount(pItem->GetTemplate()->ItemId, false) < qInfo->RequiredItemCount[i])
					{
						itemCountNeeded = qInfo->RequiredItemCount[i];
						break;
					}
				}

				// Get first available quest of chain
				while (qInfo->prevQuests.size() > 0)
				{
					Quest::PrevQuests::const_iterator iter = qInfo->prevQuests.begin();
					Quest const * qTemp = sObjectMgr->GetQuestTemplate(*iter);
					if (qTemp == NULL)
						break;
					//TC_LOG_INFO("lasyan3", " |- Quest %s, status = %d", qTemp->GetTitle().c_str(), pPlayer->GetQuestStatus(qTemp->GetQuestId()));
					/*if (pPlayer->GetQuestStatus(*iter) == QUEST_STATUS_COMPLETE)
					break;*/
					if (pPlayer->GetQuestStatus(*iter) == QUEST_STATUS_REWARDED)
						break;
					qInfo = qTemp;
				}

				std::string entity_name, area_name, zone_name;
				std::ostringstream sql;
				QueryResult result;
				EntityType entity_type;
				if (pPlayer->GetQuestStatus(qInfo->GetQuestId()) == QUEST_STATUS_COMPLETE)
				{
					entity_type = NPC_ENDER;
					sql << "SELECT ct.name, c.map, c.position_x, c.position_y, c.position_z FROM creature c"
						<< " INNER JOIN creature_questender e ON e.id = c.id"
						<< " INNER JOIN creature_template ct ON ct.entry = c.id"
						<< " WHERE e.quest = %d";
					result = WorldDatabase.PQuery(sql.str().c_str(), qInfo->GetQuestId());
				}
				else
				{
					entity_type = NPC_STARTER;
					sql << "SELECT ct.name, c.map, c.position_x, c.position_y, c.position_z FROM creature c"
						<< " INNER JOIN creature_queststarter s ON s.id = c.id"
						<< " INNER JOIN creature_template ct ON ct.entry = c.id"
						<< " WHERE s.quest = %d";
					result = WorldDatabase.PQuery(sql.str().c_str(), qInfo->GetQuestId());
					if (!result || result->GetRowCount() == 0)
					{
						entity_type = ITEM_STARTER;
						std::ostringstream sql2;
						sql2 << "SELECT gt.name, g.map, g.position_x, g.position_y, g.position_z FROM gameobject g"
							<< " INNER JOIN gameobject_queststarter s ON s.id = g.id"
							<< " INNER JOIN gameobject_template gt ON gt.entry = g.id"
							<< " WHERE s.quest = " << qInfo->GetQuestId();
						result = WorldDatabase.Query(sql2.str().c_str());
					}
				}
				if (result && result->GetRowCount() > 0)
				{

					entity_name = (*result)[0].GetString();
					uint32 _map_id = (*result)[1].GetUInt32();
					float _pos_x = (*result)[2].GetFloat();
					float _pos_y = (*result)[3].GetFloat();
					float _pos_z = (*result)[4].GetFloat();

					Map* map = sMapMgr->CreateBaseMap(_map_id);
					if (map)
					{
						AreaTableEntry const* area = sAreaTableStore.LookupEntry(map->GetAreaId(_pos_x, _pos_y, _pos_z));
						AreaTableEntry const* zone = sAreaTableStore.LookupEntry(map->GetZoneId(_pos_x, _pos_y, _pos_z));
						if (area) area_name = area->area_name[sObjectMgr->GetDBCLocaleIndex()];
						if (zone) {
							if (!area || area && stricmp(zone->area_name[sObjectMgr->GetDBCLocaleIndex()], area->area_name[sObjectMgr->GetDBCLocaleIndex()]) != 0)
							{
								zone_name = zone->area_name[sObjectMgr->GetDBCLocaleIndex()];
							}
						}
					}
				}
				struct StrItemList _sil;
				if (itemCountNeeded > 0)
					_sil.requiredCount = itemCountNeeded;
				_sil.name = pItem->GetTemplate()->Name1;
				//std::ostringstream msg;
				GossipItemMap::const_iterator got = m_entries.find(qInfo->GetQuestId());
				//GossipItemMap::const_iterator got = m_entries.find(questid);
				if (got == m_entries.end())
				{
					struct StrQuestInfo _info;
					_info.quest = qInfo->GetTitle();
					_info.items.push_back(_sil);
					if (entity_name.size() > 0)
					{
						_info.entity_type = entity_type;
						_info.entity = entity_name;
						if (area_name.size() > 0) _info.area = area_name;
						if (zone_name.size() > 0) _info.zone = zone_name;
					}
					m_entries[qInfo->GetQuestId()] = _info;
					/*msg << qInfo->GetTitle();
					msg << "\r\n - " << pItem->GetTemplate()->Name1;*/
				}
				else
				{
					//m_entries[questid].items.push_back(pItem->GetTemplate()->Name1);
					m_entries[qInfo->GetQuestId()].items.push_back(_sil);
					/*msg << got->second;
					msg << "\r\n - " << pItem->GetTemplate()->Name1;*/
				}
				//m_entries[qInfo->GetQuestId()] = msg.str();
				//m_entries[questid] = msg.str();
				/*msg << "Item: " << pItem->GetTemplate()->Name1;
				msg << "\r\nQuest: " << qInfo->GetTitle();
				if (giver_name.size() > 0)
				{
				msg << "\r\nGiver: " << giver_name;
				if (area_name.size() > 0) msg << "\r\nArea: " << area_name;
				if (zone_name.size() > 0) msg << " (" << zone_name << ")";
				}*/
				//ChatHandler ch(player->GetSession());
				//ch.SendSysMessage(msg.c_str());
				//pPlayer->ADD_GOSSIP_ITEM(0, msg.str(), GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 1);
			}
			//player->ADD_GOSSIP_ITEM(GOSSIP_ICON_DOT, pItem->GetTemplate()->Name1, pItem->GetTemplate()->ItemId, GOSSIP_ACTION_INFO_DEF + i);
			//TC_LOG_DEBUG("lasyan", "Item=%s, Sender=%d, Action=%d", pItem->GetTemplate()->Name1.c_str(), pItem->GetTemplate()->ItemId, GOSSIP_ACTION_INFO_DEF + i);
		}
	}
};

void AddSC_item_lootedQuestItems()
{
	new item_lootedQuestItems();
}
