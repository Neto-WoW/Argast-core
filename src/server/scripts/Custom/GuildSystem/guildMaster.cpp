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
            CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_GUILD_MASTER_1);
            stmt->SetData(0, m_id);

            PreparedQueryResult result = CharacterDatabase.Query(stmt);

            if (result)
            {
                Field* fields = result->Fetch();
                uint32 level = fields[0].Get<uint32>();

                std::string msg = "|TInterface/ICONS/achievement_level_80:20:20|tYour Guild level: [|cffE80000" + std::to_string(level) + "|r]";
                AddGossipItemFor(player, 0, msg.c_str(), GOSSIP_SENDER_MAIN, 1);


                if (level >= 9) {
                }
                else {
                    AddGossipItemFor(player, 0, "|TInterface/ICONS/inv_misc_coin_16:20:20|tRaise the level of the Guild", GOSSIP_SENDER_MAIN, 2);
                }

                AddGossipItemFor(player, 0, "|TInterface/ICONS/inv_misc_book_04:20:20|tStatus of my Guild", GOSSIP_SENDER_MAIN, 4);
                //AddGossipItemFor(player,0,"|TInterface/ICONS/inv_helmet_44:20:20|tWhat are effects of bonuses?", GOSSIP_SENDER_MAIN, 5);
                AddGossipItemFor(player, 0, "|TInterface/ICONS/spell_holy_holyguidance:20:20|tActive bonuses", GOSSIP_SENDER_MAIN, 6);
                AddGossipItemFor(player, 0, "|TInterface/ICONS/Ability_Spy:20:20|tExit", GOSSIP_SENDER_MAIN, 10);

            }
            else {
                ChatHandler(player->GetSession()).PSendSysMessage("|c001CE6B9[Guild-master]:|r |c00FFFFFFУ You don't have a Guild!|r");
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
        case 2: /* Something like than (Not sure if interactive part works) */
        {
            player->PlayerTalkClass->ClearMenus();
            AddGossipItemFor(player, 1, "|TInterface/ICONS/inv_misc_frostemblem_01:20:20|tEmblem of Frost", GOSSIP_SENDER_MAIN, 100, "|c001CE6B9[Information]|r |n Exchange rate|c00540081[Emblem of Frost]|r х1 for |c007EBFF1[Guild progress]|r х10. |n Attention if you press |c004E2A04[Enter]|r nothing will happen", 0, true);
            AddGossipItemFor(player, 1, "|TInterface/ICONS/spell_holy_summonchampion:20:20|tEmblem of Triumph", GOSSIP_SENDER_MAIN, 101, "|c001CE6B9[Information]|r |n Exchange rate|c00540081[Emblem of Triumph]|r х5 for |c007EBFF1[Guild progress]|r х1. |n Attention if you press |c004E2A04[Enter]|r nothing will happen", 0, true);
            AddGossipItemFor(player, 1, "|TInterface/ICONS/spell_holy_championsgrace:20:20|tEmblem of Conquest", GOSSIP_SENDER_MAIN, 102, "|c001CE6B9[Information]|r |n Exchange rate|c00540081[Emblem of Conquest]|r х1 for |c007EBFF1[Guild progress]|r х5. |n Attention if you press |c004E2A04[Enter]|r nothing will happen", 0, true);
            AddGossipItemFor(player, 0, "|TInterface/ICONS/achievement_featsofstrength_gladiator_08:20:20|tHonor Points", GOSSIP_SENDER_MAIN, 11);
            AddGossipItemFor(player, 0, "|TInterface/ICONS/Ability_Spy:20:20|tBack", GOSSIP_SENDER_MAIN, 3);
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
            AddGossipItemFor(player, 0, "|TInterface/ICONS/Ability_Spy:20:20|tBack", GOSSIP_SENDER_MAIN, 2);
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
            AddGossipItemFor(player, 0, "1000 Honor Points", GOSSIP_SENDER_MAIN, 12);
            AddGossipItemFor(player, 0, "5000 Honor Points", GOSSIP_SENDER_MAIN, 13);
            AddGossipItemFor(player, 0, "10000 Honor Points", GOSSIP_SENDER_MAIN, 14);
            AddGossipItemFor(player, 0, "25000 Honor Points", GOSSIP_SENDER_MAIN, 15);
            AddGossipItemFor(player, 0, "50000 Honor Points", GOSSIP_SENDER_MAIN, 16);
            AddGossipItemFor(player, 0, "|TInterface/ICONS/Ability_Spy:20:20|tBack", GOSSIP_SENDER_MAIN, 2);
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

        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_GUILD_MASTER_2);
        stmt->SetData(0, GuildID);

        PreparedQueryResult resultXP = CharacterDatabase.Query(stmt);

        if (resultXP)
        {
            Field* fields = resultXP->Fetch();
            uint32 xp = fields[0].Get<uint32>();
            uint32 result = xp + expReward;

            if (player->GetHonorPoints() >= countHonor)
            {
                player->ModifyHonorPoints(-countHonor);
                CharacterDatabase.Query("UPDATE guild SET xp =  %u WHERE guildid = %u", result, GuildID);
                ChatHandler(player->GetSession()).PSendSysMessage("|c001CE6B9[Guild-master]:|r |c00FFFFFFYou successfully invested|r %u |c00540081[Honor Points]|r|c00FFFFFF into your Guild.|r", countHonor);
                guildInfo(player);
                CloseGossipMenuFor(player);

            }
            else {
                ChatHandler(player->GetSession()).PSendSysMessage("|c001CE6B9[Guild-master]:|r |c00FFFFFFYou dont have enough |r|c00540081[Honor Points]|r");
                CloseGossipMenuFor(player);
            }

        }


    }

    void bonusInfo(Player* player)
    {

        uint32 m_id = player->GetGuildId();

        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_GUILD_MASTER_1);
        stmt->SetData(0, m_id);

        PreparedQueryResult result = CharacterDatabase.Query(stmt);
        if (result)
        {
            Field* fields = result->Fetch();
            uint32 level = fields[0].Get<uint32>();

            if (level <= 0)
            {
                ChatHandler(player->GetSession()).PSendSysMessage("Your Guild does not have active bonuses");
            }
            else {
                if (level > 0) {
                    ChatHandler(player->GetSession()).PSendSysMessage("|cff4CFF00[Ранг 1]|r Bonus 5% gold share to Guild bank");
                }
                if (level > 1) {
                    ChatHandler(player->GetSession()).PSendSysMessage("|cff4CFF00[Ранг 1]|r Bonus +5% experience");
                }
                if (level > 2) {
                    ChatHandler(player->GetSession()).PSendSysMessage("|cff4CFF00[Ранг 1]|r Bonus -25% repair cost");
                }
                if (level > 3) {
                    ChatHandler(player->GetSession()).PSendSysMessage("|cff4CFF00[Ранг 1]|r Bonus +20% ghost form speed");
                }
                if (level > 4) {
                    ChatHandler(player->GetSession()).PSendSysMessage("|cff4CFF00[Ранг 1]|r Bonus +5% mounted speed");
                }
                if (level > 5) {
                    ChatHandler(player->GetSession()).PSendSysMessage("|cffFF0000[Ранг 2]|r Bonus 10% gold share to Guild bank");
                }
                if (level > 6) {
                    ChatHandler(player->GetSession()).PSendSysMessage("|cffFF0000[Ранг 2]|r Bonus +10% mounted speed");
                }
                if (level > 7) {
                    ChatHandler(player->GetSession()).PSendSysMessage("|cffFF0000[Ранг 2]|r Bonus -50% repair cost");
                }
                if (level > 8) {
                    ChatHandler(player->GetSession()).PSendSysMessage("|cffFF0000[Ранг 2]|r Bonus +10% experience");
                }
            }



        }
    }

    void CheckPointsAce(Player* player, char const* code)
    {
        uint32 points = atoi(code);
        uint32 pointscoef = points * 10;
        uint32 GuildID = player->GetGuildId();

        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_GUILD_MASTER_2);
        stmt->SetData(0, GuildID);

        PreparedQueryResult resultXP = CharacterDatabase.Query(stmt);

        if (resultXP)
        {
            Field* fields = resultXP->Fetch();
            uint32 xp = fields[0].Get<uint32>();
            uint32 istok = xp + pointscoef;

            if (player->HasItemCount(49426, points))
            {
                player->DestroyItemCount(49426, points, true, false);
                CharacterDatabase.Query("UPDATE guild SET xp = %u WHERE guildid = %u", istok, GuildID);
                ChatHandler(player->GetSession()).PSendSysMessage("|c001CE6B9[Guild-master]:|r |c00FFFFFFYou successfully invested|r %u |c00540081[Emblems of Frost]|r|c00FFFFFF into your Guild.|r", points);
                guildInfo(player);
                player->PlayerTalkClass->ClearMenus();
            }
            else
            {
                ChatHandler(player->GetSession()).PSendSysMessage("|c001CE6B9[Guild-master]:|r |c00FFFFFFYou dont have enough |r|c00540081[Emblems of Frost]|r");
                player->PlayerTalkClass->ClearMenus();
            }
        }
        else
            return;
    }

    void guildInfo(Player* player)
    {
        uint32 GuildID = player->GetGuildId();

        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_GUILD_MASTER_1);
        stmt->SetData(0, GuildID);

        PreparedQueryResult result = CharacterDatabase.Query(stmt);

        if (result)
        {
            Field* fields = result->Fetch();
            uint32 level = fields[0].Get<uint32>();
            uint32 xp = fields[1].Get<uint32>();

            CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_GUILD_MASTER_3);
            stmt->SetData(0, level);

            PreparedQueryResult result1 = CharacterDatabase.Query(stmt);

            if (result1)
            {
                Field* fields = result1->Fetch();
                uint32 exe = fields[0].Get<uint32>();
                ChatHandler(player->GetSession()).PSendSysMessage("Your current guild level [%u]", level);
                ChatHandler(player->GetSession()).PSendSysMessage("Current progress [%u/%u]", xp, exe);
                player->PlayerTalkClass->ClearMenus();
            }
        }
    }
    void CheckPointsTtriumph(Player* player, char const* code)
    {
        uint32 points = atoi(code);
        uint32 pointscoef = points * 5;
        uint32 GuildID = player->GetGuildId();

        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_GUILD_MASTER_2);
        stmt->SetData(0, GuildID);

        PreparedQueryResult resultXP = CharacterDatabase.Query(stmt);
        if (resultXP)
        {
            Field* fields = resultXP->Fetch();
            uint32 xp = fields[0].Get<uint32>();
            uint32 istok = xp + pointscoef;

            if (player->HasItemCount(47241, points))
            {
                player->DestroyItemCount(47241, points, true, false);
                CharacterDatabase.Query("UPDATE guild SET xp =  %u WHERE guildid = %u", istok, GuildID);
                ChatHandler(player->GetSession()).PSendSysMessage("|c001CE6B9[Guild-master]:|r |c00FFFFFFYou successfully invested|r %u |c00540081[Emblems of Triumph]|r|c00FFFFFF into your Guild.|r", points);
                player->PlayerTalkClass->ClearMenus();
                guildInfo(player);
            }
            else
            {
                ChatHandler(player->GetSession()).PSendSysMessage("|c001CE6B9[Guild-master]:|r |c00FFFFFFYou dont have enough |r|c00540081[Emblems of Triumph]|r");
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
                ChatHandler(player->GetSession()).PSendSysMessage("|c001CE6B9[Guild-master]:|r |c00FFFFFFYou successfully invested|r %u |c00540081[Emblems of Conquest]|r|c00FFFFFF into your Guild.|r", points);
                player->PlayerTalkClass->ClearMenus();
                guildInfo(player);
            }
            else
            {
                ChatHandler(player->GetSession()).PSendSysMessage("|c001CE6B9[Guild-master]:|r |c00FFFFFFYou dont have enough |r|c00540081[Emblems of Conquest]|r");
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
            ss << "|TInterface/ICONS/achievement_general:10:10:-23:0|tGuild <" << name << "> gains [ " << level << " ] level of progression! Congratulations";
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
