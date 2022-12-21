/*
 * Copyright © 2020 Z C O R E
 * This content is a private project
 *
 * The core is based on AzerotCore 2019
 * Huge gratitude for their work
 * Lead kernel developer: F42 and zaqqaz
 *
 */
#include "Guild.h"
#include "GuildMgr.h"
#include "Player.h"
#include "DatabaseEnv.h"
#include "ScriptedGossip.h"
#include "ScriptMgr.h"
#include "World.h"
 //---------Coefficient-----------//
#define HONOR_POINT  1


class guild_npc : public CreatureScript
{
public:
	guild_npc() : CreatureScript("guild_npc") { }

	bool OnGossipHello(Player* player, Creature* creature)
	{
		player->PlayerTalkClass->ClearMenus();
		if (uint32 m_id = player->GetGuildId())
		{
			QueryResult result = CharacterDatabase.Query("SELECT level, xp FROM guild WHERE guildid = %u", m_id);
			if (result)
			{
				Field* fields = result->Fetch();
				uint32 level = fields[0].Get<uint32>();



				std::string msg = "|TInterface/ICONS/achievement_level_80:20:20|tУровень вашей гильдии: [|cffE80000" + std::to_string(level) + "|r]";
                AddGossipItemFor(player, 0, msg.c_str(), GOSSIP_SENDER_MAIN, 1);


				if (level >= 9) {
				}
				else {
                    AddGossipItemFor(player, 0, "|TInterface/ICONS/inv_misc_coin_16:20:20|tПовысить уровень гильдии", GOSSIP_SENDER_MAIN, 2);
				}

                AddGossipItemFor(player, 0, "|TInterface/ICONS/inv_misc_book_04:20:20|tСостояние моей гильдии", GOSSIP_SENDER_MAIN, 4);
				//AddGossipItemFor(player,0,"|TInterface/ICONS/inv_helmet_44:20:20|tЧто дают бонусы?", GOSSIP_SENDER_MAIN, 5);
                AddGossipItemFor(player, 0, "|TInterface/ICONS/spell_holy_holyguidance:20:20|tДействующие бонусы", GOSSIP_SENDER_MAIN, 6);
                AddGossipItemFor(player, 0, "|TInterface/ICONS/Ability_Spy:20:20|tВыход", GOSSIP_SENDER_MAIN, 10);

			}
			else {
				ChatHandler(player->GetSession()).PSendSysMessage("|c001CE6B9[Гильд-мастер]:|r |c00FFFFFFУ вас нет гильдии!|r");
				player->PlayerTalkClass->SendCloseGossip();
				return true;
			}
            SendGossipMenuFor(player, 1, creature->GetGUID());
			return true;

		}
		else
			return true;

	}

	bool OnGossipSelect(Player* player, Creature* _Creature, uint32 sender, uint32 action)
	{
		if (sender != GOSSIP_SENDER_MAIN)
		{
			return false;
		}
		switch (action)
		{
		case 1:
			OnGossipHello(player, _Creature);
			break;
		case 2: /* Что-то вроде того (Не фак что интерактив сроботает) */
		{
			player->PlayerTalkClass->ClearMenus();
            AddGossipItemFor(player, 1, "|TInterface/ICONS/inv_misc_frostemblem_01:20:20|tЭмблемы льда", GOSSIP_SENDER_MAIN, 100, "|c001CE6B9[Информация]|r |n Курс обмена|c00540081[Эмблема льда]|r х1 на |c007EBFF1[Опыт гильдии]|r х10. |n Учьтите если вы нажмёте |c004E2A04[Enter]|r ничего не произойдёт", 0, true);
            AddGossipItemFor(player, 1, "|TInterface/ICONS/spell_holy_summonchampion:20:20|tЭмблемы триумфа", GOSSIP_SENDER_MAIN, 101, "|c001CE6B9[Информация]|r |n Курс обмена|c00540081[Эмблемы триумфа]|r х5 на |c007EBFF1[Опыт гильдии]|r х1. |n Учьтите если вы нажмёте |c004E2A04[Enter]|r ничего не произойдёт", 0, true);
            AddGossipItemFor(player, 1, "|TInterface/ICONS/spell_holy_championsgrace:20:20|tЭмблемы завоевания", GOSSIP_SENDER_MAIN, 102, "|c001CE6B9[Информация]|r |n Курс обмена|c00540081[Эмблема завоевания]|r х1 на |c007EBFF1[Опыт гильдии]|r х5. |n Учьтите если вы нажмёте |c004E2A04[Enter]|r ничего не произойдёт", 0, true);
            AddGossipItemFor(player, 0, "|TInterface/ICONS/achievement_featsofstrength_gladiator_08:20:20|tОчки чести", GOSSIP_SENDER_MAIN, 11);
            AddGossipItemFor(player, 0, "|TInterface/ICONS/Ability_Spy:20:20|tНазад", GOSSIP_SENDER_MAIN, 3);
            SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, _Creature->GetGUID());
		}
		break;
		case 3: //main menu
			OnGossipHello(player, _Creature);
			break;
		case 4: //guild info
			guildInfo(player);
            CloseGossipMenuFor(player);
			break;
		case 5: //About bonus
			player->PlayerTalkClass->ClearMenus();
			SendGossipMenuFor(player, 70010, _Creature->GetGUID());
			AddGossipItemFor(player,0, "|TInterface/ICONS/Ability_Spy:20:20|tНазад", GOSSIP_SENDER_MAIN, 2);
			break;
		case 6: //Active bonus
			bonusInfo(player);
            CloseGossipMenuFor(player);
			break;
		case 10: //exit
            CloseGossipMenuFor(player);
			break;
		case 11: //honor point
			player->PlayerTalkClass->ClearMenus();
            AddGossipItemFor(player, 0, "1000 очков чести", GOSSIP_SENDER_MAIN, 12);
			AddGossipItemFor(player,0, "5000 очков чести", GOSSIP_SENDER_MAIN, 13);
			AddGossipItemFor(player,0, "10000 очков чести", GOSSIP_SENDER_MAIN, 14);
			AddGossipItemFor(player,0, "25000 очков чести", GOSSIP_SENDER_MAIN, 15);
			AddGossipItemFor(player,0, "50000 очков чести", GOSSIP_SENDER_MAIN, 16);
			AddGossipItemFor(player,0, "|TInterface/ICONS/Ability_Spy:20:20|tНазад", GOSSIP_SENDER_MAIN, 2);
			SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, _Creature->GetGUID());
			break;

		case 12:
			ChekHonorCoin(player, 1000, 5);
			break;
		case 13:
			ChekHonorCoin(player, 5000, 25);
			break;
		case 14:
			ChekHonorCoin(player, 10000, 50);
			break;
		case 15:
			ChekHonorCoin(player, 25000, 125);
			break;
		case 16:
			ChekHonorCoin(player, 50000, 250);
			break;
		}
		return true;
	}

	void ChekHonorCoin(Player* player, uint32 countHonor, uint32 expReward)
	{
		uint32 GuildID = player->GetGuildId();


		QueryResult resultXP = CharacterDatabase.Query("SELECT xp FROM guild WHERE guildid = %u", GuildID);
		if (resultXP)
		{
			Field* fields = resultXP->Fetch();
			uint32 xp = fields[0].Get<uint32>();
			uint32 result = xp + expReward;

			if (player->GetHonorPoints() >= countHonor)
			{
				player->ModifyHonorPoints(-countHonor);
				CharacterDatabase.Query("UPDATE guild SET xp =  %u WHERE guildid = %u", result, GuildID);
				ChatHandler(player->GetSession()).PSendSysMessage("|c001CE6B9[Гильд-мастер]:|r |c00FFFFFFВы успешно сделали свой вклад|r %u |c00540081[Очков чести]|r|c00FFFFFF в вашу гильдию.|r", countHonor);
				guildInfo(player);
                CloseGossipMenuFor(player);

			}
			else {
				ChatHandler(player->GetSession()).PSendSysMessage("|c001CE6B9[Гильд-мастер]:|r |c00FFFFFFУ вас не достаточно |r|c00540081[Очков чести]|r");
                CloseGossipMenuFor(player);
			}

		}


	}

	void bonusInfo(Player* player)
	{

		uint32 m_id = player->GetGuildId();

		QueryResult result = CharacterDatabase.Query("SELECT level, xp FROM guild WHERE guildid = %u", m_id);
		if (result)
		{
			Field* fields = result->Fetch();
			uint32 level = fields[0].Get<uint32>();

			if (level <= 0)
			{
				ChatHandler(player->GetSession()).PSendSysMessage("У вашей гильдии нет активных бонусов");
			}
			else {
				if (level > 0) {
					ChatHandler(player->GetSession()).PSendSysMessage("|cff4CFF00[Ранг 1]|r Бонус 5% с добычи в банк");
				}
				if (level > 1) {
					ChatHandler(player->GetSession()).PSendSysMessage("|cff4CFF00[Ранг 1]|r Бонус +5% к опыту");
				}
				if (level > 2) {
					ChatHandler(player->GetSession()).PSendSysMessage("|cff4CFF00[Ранг 1]|r Бонус -25% к стоимости починки");
				}
				if (level > 3) {
					ChatHandler(player->GetSession()).PSendSysMessage("|cff4CFF00[Ранг 1]|r Бонус +20% к скорости духа");
				}
				if (level > 4) {
					ChatHandler(player->GetSession()).PSendSysMessage("|cff4CFF00[Ранг 1]|r Бонус +5% к скорости передвижения (верхом, на маунте)");
				}
				if (level > 5) {
					ChatHandler(player->GetSession()).PSendSysMessage("|cffFF0000[Ранг 2]|r Бонус 10% с добычи в банк");
				}
				if (level > 6) {
					ChatHandler(player->GetSession()).PSendSysMessage("|cffFF0000[Ранг 2]|r Бонус +10% к скорости передвижения");
				}
				if (level > 7) {
					ChatHandler(player->GetSession()).PSendSysMessage("|cffFF0000[Ранг 2]|r Бонус -50% к стоимости починки");
				}
				if (level > 8) {
					ChatHandler(player->GetSession()).PSendSysMessage("|cffFF0000[Ранг 2]|r Бонус +10% к опыту");
				}
			}



		}
	}

	void CheckPointsAce(Player* player, char const* code)
	{
		uint32 points = atoi(code);
		uint32 pointscoef = points * 10;
		uint32 GuildID = player->GetGuildId();

		QueryResult resultXP = CharacterDatabase.Query("SELECT xp FROM guild WHERE guildid = %u", GuildID);
		if (resultXP)
		{
			Field* fields = resultXP->Fetch();
			uint32 xp = fields[0].Get<uint32>();
			uint32 istok = xp + pointscoef;

			if (player->HasItemCount(49426, points))
			{
				player->DestroyItemCount(49426, points, true, false);
				CharacterDatabase.Query("UPDATE guild SET xp = %u WHERE guildid = %u", istok, GuildID);
				ChatHandler(player->GetSession()).PSendSysMessage("|c001CE6B9[Гильд-мастер]:|r |c00FFFFFFВы успешно сделали свой вклад|r %u |c00540081[Эмблем льда]|r|c00FFFFFF в вашу гильдию.|r", points);
				guildInfo(player);
				player->PlayerTalkClass->ClearMenus();
			}
			else
			{
				ChatHandler(player->GetSession()).PSendSysMessage("|c001CE6B9[Гильд-мастер]:|r |c00FFFFFFУ вас не достаточно |r|c00540081[Эмблем льда]|r");
				player->PlayerTalkClass->ClearMenus();
			}
		}
		else
			return;
	}

	void guildInfo(Player* player)
	{
		uint32 GuildID = player->GetGuildId();
		QueryResult result = CharacterDatabase.Query("SELECT level, xp FROM guild WHERE guildid = %u", GuildID);

		if (result)
		{
			Field* fields = result->Fetch();
			uint32 level = fields[0].Get<uint32>();
			uint32 xp = fields[1].Get<uint32>();

			QueryResult result1 = CharacterDatabase.Query("SELECT xp_for_next_level FROM guild_xp_for_next_level WHERE level = %u", level);

			if (result1)
			{
				Field* fields = result1->Fetch();
				uint32 exe = fields[0].Get<uint32>();
				ChatHandler(player->GetSession()).PSendSysMessage("Уровень вашей гильдии [%u]", level);
				ChatHandler(player->GetSession()).PSendSysMessage("Текущий опыт [%u/%u]", xp, exe);
				player->PlayerTalkClass->ClearMenus();
			}
		}
	}
	void CheckPointsTtriumph(Player* player, char const* code)
	{
		uint32 points = atoi(code);
		uint32 pointscoef = points * 5;
		uint32 GuildID = player->GetGuildId();

		QueryResult resultXP = CharacterDatabase.Query("SELECT xp FROM guild WHERE guildid = %u", GuildID);
		if (resultXP)
		{
			Field* fields = resultXP->Fetch();
			uint32 xp = fields[0].Get<uint32>();
			uint32 istok = xp + pointscoef;

			if (player->HasItemCount(47241, points))
			{
				player->DestroyItemCount(47241, points, true, false);
				CharacterDatabase.Query("UPDATE guild SET xp =  %u WHERE guildid = %u", istok, GuildID);
				ChatHandler(player->GetSession()).PSendSysMessage("|c001CE6B9[Гильд-мастер]:|r |c00FFFFFFВы успешно сделали свой вклад|r %u |c00540081[Эмблем триумфа]|r|c00FFFFFF в вашу гильдию.|r", points);
				player->PlayerTalkClass->ClearMenus();
				guildInfo(player);
			}
			else
			{
				ChatHandler(player->GetSession()).PSendSysMessage("|c001CE6B9[Гильд-мастер]:|r |c00FFFFFFУ вас не достаточно |r|c00540081[Эмблем триумфа]|r");
				player->PlayerTalkClass->ClearMenus();
			}
		}
		else
			return;
	}

	void CheckPointsСhampionsgrace(Player* player, char const* code)
	{
		uint32 points = atoi(code);
		uint32 pointscoef = points * 5;
		uint32 GuildID = player->GetGuildId();

		QueryResult resultXP = CharacterDatabase.Query("SELECT xp FROM guild WHERE guildid = %u", GuildID);
		if (resultXP)
		{
			Field* fields = resultXP->Fetch();
			uint32 xp = fields[0].Get<uint32>();
			uint32 istok = xp + pointscoef;

			if (player->HasItemCount(45624, points))
			{
				player->DestroyItemCount(45624, points, true, false);
				CharacterDatabase.Query("UPDATE guild SET xp =  %u WHERE guildid = %u", istok, GuildID);
				ChatHandler(player->GetSession()).PSendSysMessage("|c001CE6B9[Гильд-мастер]:|r |c00FFFFFFВы успешно сделали свой вклад|r %u |c00540081[Эмблем завоевания]|r|c00FFFFFF в вашу гильдию.|r", points);
				player->PlayerTalkClass->ClearMenus();
				guildInfo(player);
			}
			else
			{
				ChatHandler(player->GetSession()).PSendSysMessage("|c001CE6B9[Гильд-мастер]:|r |c00FFFFFFУ вас не достаточно |r|c00540081[Эмблем завоевания]|r");
				player->PlayerTalkClass->ClearMenus();
			}
		}
		else
			return;
	}

	void MessegeGuildLevelUp(Player* player)
	{
		uint32 GuildID = player->GetGuildId();
		QueryResult result = CharacterDatabase.Query("SELECT name, level FROM guild WHERE guildid = %u", GuildID);
		if (result)
		{
			Field* fields = result->Fetch();
            std::string name = fields[0].Get<std::string>();
			uint32 level = fields[1].Get<uint32>();

			std::ostringstream ss;
			ss << "|TInterface/ICONS/achievement_general:10:10:-23:0|tГильдия <" << name << "> получает [ " << level << " ] уровень для своей гильдии!Поздравляем";
			sWorld->SendServerMessage(SERVER_MSG_STRING, ss.str().c_str());
		}

	}
	//chek guild level updata
	void UpdateLevelGuild(Player* player)
	{
		uint32 GuildID = player->GetGuildId();
		QueryResult result = CharacterDatabase.Query("SELECT level, xp FROM guild WHERE guildid = %u", GuildID);
		if (result)
		{
			Field* fields = result->Fetch();
			uint32 level = fields[0].Get<uint32>();
			uint32 xp = fields[1].Get<uint32>();

			QueryResult result1 = CharacterDatabase.Query("SELECT xp_for_next_level FROM guild_xp_for_next_level WHERE level = %u", level);

			if (result1)
			{
				Field* fields = result1->Fetch();
				uint32 exe = fields[0].Get<uint32>();

				if (xp >= exe) {
					CharacterDatabase.Query("UPDATE guild SET level = %u + 1 WHERE guildid = %u", level, GuildID);
					UpdateLevelGuild(player);
					MessegeGuildLevelUp(player);
					player->PlayerTalkClass->ClearMenus();
				}
				else {
					return;
				}

			}

		}

	}


	bool OnGossipSelectCode(Player* player, Creature* creature, uint32 sender, uint32 action, char const* code)
	{

		if (sender != GOSSIP_SENDER_MAIN)
		{
			return false;
		}

		switch (action)
		{

		case 100:
			CheckPointsAce(player, code);
			UpdateLevelGuild(player);
			player->PlayerTalkClass->ClearMenus();
            CloseGossipMenuFor(player);
			break;

		case 101:
			CheckPointsTtriumph(player, code);
			UpdateLevelGuild(player);
			player->PlayerTalkClass->ClearMenus();
            CloseGossipMenuFor(player);
			break;
		case 102:
			CheckPointsСhampionsgrace(player, code);
			UpdateLevelGuild(player);
			player->PlayerTalkClass->ClearMenus();
            CloseGossipMenuFor(player);
			break;
		case 104:
			SendGossipMenuFor(player, 70010, creature->GetGUID());
			break;

		}
		return true;
	}
};

void AddSC_guildMaster_npc()
{
	new guild_npc();
}
