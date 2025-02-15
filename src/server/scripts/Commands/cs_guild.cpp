/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/* ScriptData
Name: guild_commandscript
%Complete: 100
Comment: All guild related commands
Category: commandscripts
EndScriptData */

#include "Chat.h"
#include "Guild.h"
#include "GuildMgr.h"
#include "Language.h"
#include "ScriptMgr.h"

#if AC_COMPILER == AC_COMPILER_GNU
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

using namespace Acore::ChatCommands;

class guild_commandscript : public CommandScript
{
public:
    guild_commandscript() : CommandScript("guild_commandscript") { }

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable guildCommandTable =
        {
            { "create",         SEC_GAMEMASTER,     true,  &HandleGuildCreateCommand,           "" },
            { "delete",         SEC_GAMEMASTER,     true,  &HandleGuildDeleteCommand,           "" },
            { "invite",         SEC_GAMEMASTER,     true,  &HandleGuildInviteCommand,           "" },
            { "uninvite",       SEC_GAMEMASTER,     true,  &HandleGuildUninviteCommand,         "" },
            { "rank",           SEC_GAMEMASTER,     true,  &HandleGuildRankCommand,             "" },
            { "rename",         SEC_GAMEMASTER,     true,  &HandleGuildRenameCommand,           "" },
            { "info",           SEC_GAMEMASTER,     true,  &HandleGuildInfoCommand,             "" },
            { "linfo",          SEC_GAMEMASTER,     true,  &HandleGuildLInfoCommand,              "" },
            { "setlevel",       SEC_GAMEMASTER,     true,  &HandleGuildSetLevelCommand,         "" },
            { "givexp",         SEC_GAMEMASTER,     true,  &HandleGuildGiveXpCommand,           "" }
        };
        static ChatCommandTable commandTable =
        {
            { "guild",          SEC_GAMEMASTER,  true, nullptr,                                 "", guildCommandTable }
        };
        return commandTable;
    }

    /** \brief GM command level 3 - Create a guild.
     *
     * This command allows a GM (level 3) to create a guild.
     *
     * The "args" parameter contains the name of the guild leader
     * and then the name of the guild.
     *
     */
    static bool HandleGuildCreateCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        // if not guild name only (in "") then player name
        Player* target;
        if (!handler->extractPlayerTarget(*args != '"' ? (char*)args : nullptr, &target))
            return false;

        char* tailStr = *args != '"' ? strtok(nullptr, "") : (char*)args;
        if (!tailStr)
            return false;

        char* guildStr = handler->extractQuotedArg(tailStr);
        if (!guildStr)
            return false;

        std::string guildName = guildStr;

        if (target->GetGuildId())
        {
            handler->SendSysMessage(LANG_PLAYER_IN_GUILD);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (sGuildMgr->GetGuildByName(guildName))
        {
            handler->SendSysMessage(LANG_GUILD_RENAME_ALREADY_EXISTS);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (sObjectMgr->IsReservedName(guildName) || !sObjectMgr->IsValidCharterName(guildName))
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Guild* guild = new Guild;
        if (!guild->Create(target, guildName))
        {
            delete guild;
            handler->SendSysMessage(LANG_GUILD_NOT_CREATED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        sGuildMgr->AddGuild(guild);

        return true;
    }

    static bool HandleGuildDeleteCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        char* guildStr = handler->extractQuotedArg((char*)args);
        if (!guildStr)
            return false;

        std::string guildName = guildStr;

        Guild* targetGuild = sGuildMgr->GetGuildByName(guildName);
        if (!targetGuild)
            return false;

        targetGuild->Disband();
        delete targetGuild;

        return true;
    }

    static bool HandleGuildInviteCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        // if not guild name only (in "") then player name
        ObjectGuid targetGuid;
        if (!handler->extractPlayerTarget(*args != '"' ? (char*)args : nullptr, nullptr, &targetGuid))
            return false;

        char* tailStr = *args != '"' ? strtok(nullptr, "") : (char*)args;
        if (!tailStr)
            return false;

        char* guildStr = handler->extractQuotedArg(tailStr);
        if (!guildStr)
            return false;

        std::string guildName = guildStr;
        Guild* targetGuild = sGuildMgr->GetGuildByName(guildName);
        if (!targetGuild)
            return false;

        // player's guild membership checked in AddMember before add
        return targetGuild->AddMember(targetGuid);
    }

    static bool HandleGuildUninviteCommand(ChatHandler* handler, char const* args)
    {
        Player* target;
        ObjectGuid targetGuid;
        if (!handler->extractPlayerTarget((char*)args, &target, &targetGuid))
            return false;

        uint32 guildId = target ? target->GetGuildId() : sCharacterCache->GetCharacterGuildIdByGuid(targetGuid);
        if (!guildId)
            return false;

        Guild* targetGuild = sGuildMgr->GetGuildById(guildId);
        if (!targetGuild)
            return false;

        targetGuild->DeleteMember(targetGuid, false, true, true);
        return true;
    }

    static bool HandleGuildRankCommand(ChatHandler* handler, Optional<PlayerIdentifier> player, uint8 rank)
    {
        if (!player)
            player = PlayerIdentifier::FromTargetOrSelf(handler);

        if (!player)
            return false;

        uint32 guildId = player->IsConnected() ? player->GetConnectedPlayer()->GetGuildId() : sCharacterCache->GetCharacterGuildIdByGuid(player->GetGUID());
        if (!guildId)
            return false;

        Guild* targetGuild = sGuildMgr->GetGuildById(guildId);
        if (!targetGuild)
            return false;

        return targetGuild->ChangeMemberRank(player->GetGUID(), rank);
    }

    static bool HandleGuildRenameCommand(ChatHandler* handler, char const* _args)
    {
        if (!*_args)
            return false;

        char *args = (char *)_args;

        char const* oldGuildStr = handler->extractQuotedArg(args);
        if (!oldGuildStr)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        char const* newGuildStr = handler->extractQuotedArg(strtok(nullptr, ""));
        if (!newGuildStr)
        {
            handler->SendSysMessage(LANG_INSERT_GUILD_NAME);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Guild* guild = sGuildMgr->GetGuildByName(oldGuildStr);
        if (!guild)
        {
            handler->PSendSysMessage(LANG_COMMAND_COULDNOTFIND, oldGuildStr);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (sGuildMgr->GetGuildByName(newGuildStr))
        {
            handler->PSendSysMessage(LANG_GUILD_RENAME_ALREADY_EXISTS, newGuildStr);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!guild->SetName(newGuildStr))
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        handler->PSendSysMessage(LANG_GUILD_RENAME_DONE, oldGuildStr, newGuildStr);
        return true;
    }

    static bool HandleGuildInfoCommand(ChatHandler* handler, char const* args)
    {
        Guild* guild = nullptr;

        if (args && args[0] != '\0')
        {
            if (isNumeric(args))
                guild = sGuildMgr->GetGuildById(strtoull(args, nullptr, 10));
            else
                guild = sGuildMgr->GetGuildByName(args);
        }
        else if (Player* target = handler->getSelectedPlayerOrSelf())
            guild = target->GetGuild();

        if (!guild)
            return false;

        // Display Guild Information
        handler->PSendSysMessage(LANG_GUILD_INFO_NAME, guild->GetName().c_str(), guild->GetId()); // Guild Id + Name
        std::string guildMasterName;
        if (sCharacterCache->GetCharacterNameByGuid(guild->GetLeaderGUID(), guildMasterName))
        {
            handler->PSendSysMessage(LANG_GUILD_INFO_GUILD_MASTER, guildMasterName.c_str(), guild->GetLeaderGUID().GetCounter()); // Guild Master
        }

        handler->PSendSysMessage(LANG_GUILD_INFO_CREATION_DATE, Acore::Time::TimeToHumanReadable(Seconds(guild->GetCreatedDate())).c_str()); // Creation Date
        handler->PSendSysMessage(LANG_GUILD_INFO_MEMBER_COUNT, guild->GetMemberCount()); // Number of Members
        handler->PSendSysMessage(LANG_GUILD_INFO_BANK_GOLD, guild->GetTotalBankMoney() / 100 / 100); // Bank Gold (in gold coins)
        handler->PSendSysMessage(LANG_GUILD_INFO_MOTD, guild->GetMOTD().c_str()); // Message of the day
        handler->PSendSysMessage(LANG_GUILD_INFO_EXTRA_INFO, guild->GetInfo().c_str()); // Extra Information
        return true;
    }

    //Guild-Level-System
    static bool HandleGuildLInfoCommand(ChatHandler* handler, char const* /*args*/)
    {
        Guild* guild = handler->GetSession()->GetPlayer()->GetGuild();

        if (guild)
        {
            handler->PSendSysMessage(LANG_GUILDINFO_LEVEL, guild->GetLevel());

            if (guild->GetLevel() >= GUILD_MAX_LEVEL)
                handler->PSendSysMessage(LANG_GUILDINFO_XP_INFO, 0, 0);
            else
                handler->PSendSysMessage(LANG_GUILDINFO_XP_INFO, guild->GetCurrentXP(), guild->GetXpForNextLevel());

            handler->PSendSysMessage("Active Bonus:");

            if (guild->GetLevel() > 0)
            {
                if (guild->HasLevelForBonus(GUILD_BONUS_GOLD_1))
                    handler->PSendSysMessage("Gold bonus [Rank 1]");
                if (guild->HasLevelForBonus(GUILD_BONUS_XP_1))
                    handler->PSendSysMessage("Bonus Experience [Rank 1]");
                if (guild->HasLevelForBonus(GUILD_BONUS_SCHNELLER_GEIST))
                    handler->PSendSysMessage("Faster Ghost");
                if (guild->HasLevelForBonus(GUILD_BONUS_REPERATUR_1))
                    handler->PSendSysMessage("Cheaper Repairs [Rank 1]");
                if (guild->HasLevelForBonus(GUILD_BONUS_GOLD_2))
                    handler->PSendSysMessage("Gold bonus [Rank 2]");
                if (guild->HasLevelForBonus(GUILD_BONUS_REITTEMPO_1))
                    handler->PSendSysMessage("Mount Speed [Rank 1]");
                if (guild->HasLevelForBonus(GUILD_BONUS_RUF_1))
                    handler->PSendSysMessage("Reputation [Rank 1]");
                if (guild->HasLevelForBonus(GUILD_BONUS_XP_2))
                    handler->PSendSysMessage("Bonus Experience [Rank 2]");
                if (guild->HasLevelForBonus(GUILD_BONUS_REPERATUR_2))
                    handler->PSendSysMessage("Cheaper Repairs [Rank 2]");
                if (guild->HasLevelForBonus(GUILD_BONUS_REITTEMPO_2))
                    handler->PSendSysMessage("Mount Speed [Rank 2]");
                if (guild->HasLevelForBonus(GUILD_BONUS_REPERATUR_2))
                    handler->PSendSysMessage("Reputation [Rank 2]");
                if (guild->HasLevelForBonus(GUILD_BONUS_EHRE_1))
                    handler->PSendSysMessage("Bonus Honor [Rank 1]");
                if (guild->HasLevelForBonus(GUILD_BONUS_EHRE_2))
                    handler->PSendSysMessage("Bonus Honor [Rank 2]");
            }
            else
                handler->PSendSysMessage("None");

            return true;
        }
        else
        {
            handler->PSendSysMessage("You are not in a guild");
            return false;
        }
    }

    static bool HandleGuildSetLevelCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        char* guildNameStr;
        char* levelStr;
        handler->extractOptFirstArg((char*)args, &guildNameStr, &levelStr);

        if (!levelStr)
            return false;

        uint8 newLevel = uint8(atoi(levelStr));
        Guild* guild = sGuildMgr->GetGuildByName(guildNameStr);

        if (guild)
        {
            if (newLevel > GUILD_MAX_LEVEL)
            {
                handler->PSendSysMessage("Your guild is max level");
                return false;
            }
            else
                guild->SetLevel(newLevel, true);
        }
        else
        {
            handler->PSendSysMessage("There is no guild named [%s] found.", guildNameStr);
            return false;
        }

        return true;
    }

    static bool HandleGuildGiveXpCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        char* guildNameStr;
        char* xpStr;
        handler->extractOptFirstArg((char*)args, &guildNameStr, &xpStr);

        if (!xpStr)
            return false;

        uint32 value = uint32(atoi(xpStr));

        Guild* guild = sGuildMgr->GetGuildByName(guildNameStr);

        if (guild)
            guild->GiveXp(value);
        else
        {
            handler->PSendSysMessage("There was no guild with the name [%s] found.", guildNameStr);
            return false;
        }

        return true;
    }
};

void AddSC_guild_commandscript()
{
    new guild_commandscript();
}
