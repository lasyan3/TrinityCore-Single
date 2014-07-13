class item_lootedQuestItems : public ItemScript
{
public:
	item_lootedQuestItems() : ItemScript("item_lootedQuestItems") {}

	bool OnUse(Player* player, Item* item, SpellCastTargets const& targets) // Any hook here
	{
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
		player->PlayerTalkClass->SendGossipMenu(DEFAULT_GOSSIP_MESSAGE, item->GetGUID());
		return false;
	}

	void OnGossipSelect(Player* player, Item* item, uint32 sender, uint32 action)
	{
		return;
		TC_LOG_DEBUG("lasyan", "Action=%d, Sender=%d", action, sender);
		player->PlayerTalkClass->ClearMenus();
		//int32 id;
		std::string msg;
		if (sender == GOSSIP_SENDER_MAIN)
		{
			//MakeFirstPage(player);
			player->PlayerTalkClass->SendGossipMenu(DEFAULT_GOSSIP_MESSAGE, item->GetGUID());
			return;
		}
		else if (sender == GOSSIP_SENDER_INFO)
		{
		} 
		else
		{
			//Item* pItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i);
			uint32 itemid = sender;// pItem->GetTemplate()->ItemId;
			ObjectMgr::QuestMap _allQuests = player->GetAvailableQuestsForItem(itemid);
			ObjectMgr::QuestMap::const_iterator iter = _allQuests.begin();
			if (iter != _allQuests.end())
			{
				uint32 questid = iter->first;
				Quest const* qInfo = iter->second;
				std::string giver_name, area_name, zone_name;
				player->GetQuestItemInformations(qInfo, giver_name, area_name, zone_name);
				ItemTemplate const * it = sObjectMgr->GetItemTemplate(itemid);
				std::ostringstream msg;
				msg << "Item: " << it->Name1;
				if (giver_name.size() > 0)
				{
					msg << "\r\nGiver: " << giver_name;
					if (area_name.size() > 0) msg << "\r\nArea: " << area_name;
					if (zone_name.size() > 0) msg << " (" << zone_name << ")";
				}
				//ChatHandler ch(player->GetSession());
				//ch.SendSysMessage(msg.c_str());
				player->ADD_GOSSIP_ITEM(0, msg.str(), GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 1);
			}
				/*}
			}
			for (uint8 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; ++i)
			{
				Bag* pBag = player->GetBagByPos(i);
				if (pBag == NULL) continue;
				for (uint32 j = 0; j < pBag->GetBagSize(); ++j)
				{
					if (action == GOSSIP_ACTION_INFO_DEF * 10 + i * 100 + j)
					{
						Item* pItem = player->GetItemByPos(i, j);
						uint32 questid = GetQuestForItem(player, pItem->GetTemplate()->ItemId);
						//player->GetItemQuestgiverInformation(pItem->GetTemplate()->ItemId, sObjectMgr->GetQuestTemplate(questid), id, msg);
						//ChatHandler ch(player->GetSession());
						//ch.SendSysMessage(msg.c_str());
						player->ADD_GOSSIP_ITEM(0, msg, GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 1);
						break;
					}
				}
			}*/
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "BACK", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 666);
			player->PlayerTalkClass->SendGossipMenu(DEFAULT_GOSSIP_MESSAGE, item->GetGUID());
		}
	}
private:

	void ProcessItem(Player * pPlayer, Item *pItem)
	{
		if (pItem->GetTemplate()->Class == 12 && !pPlayer->HasQuestForItem(pItem->GetTemplate()->ItemId))
		{
			ObjectMgr::QuestMap _allQuests = pPlayer->GetAvailableQuestsForItem(pItem->GetTemplate()->ItemId);
			if (_allQuests.size() > 0)
			{
				ObjectMgr::QuestMap::const_iterator iter = _allQuests.begin();
				uint32 questid = iter->first;
				Quest const* qInfo = iter->second;

				// Get first available quest of chain
				while (qInfo->prevQuests.size() > 0)
				{
					Quest::PrevQuests::const_iterator iter = qInfo->prevQuests.begin();
					Quest const * qTemp = sObjectMgr->GetQuestTemplate(*iter);
					if (qTemp == NULL)
						break;
					if (pPlayer->GetQuestStatus(*iter) == QUEST_STATUS_REWARDED)
						break;
					qInfo = qTemp;
				}

				std::string giver_name, area_name, zone_name;
				pPlayer->GetQuestItemInformations(qInfo, giver_name, area_name, zone_name);
				std::ostringstream msg;
				msg << "Item: " << pItem->GetTemplate()->Name1;
				msg << "\r\nQuest: " << qInfo->GetTitle();
				if (giver_name.size() > 0)
				{
					msg << "\r\nGiver: " << giver_name;
					if (area_name.size() > 0) msg << "\r\nArea: " << area_name;
					if (zone_name.size() > 0) msg << " (" << zone_name << ")";
				}
				//ChatHandler ch(player->GetSession());
				//ch.SendSysMessage(msg.c_str());
				pPlayer->ADD_GOSSIP_ITEM(0, msg.str(), GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 1);
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
