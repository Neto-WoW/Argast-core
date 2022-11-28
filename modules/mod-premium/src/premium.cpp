#include "Define.h"
#include "GossipDef.h"
#include "Item.h"
#include "Player.h"
#include "ScriptedGossip.h"
#include "ScriptMgr.h"
#include "Spell.h"
#include "Config.h"

enum Vendors
{
    NPC_VENDOR_A    = 54,
    NPC_VENDOR_H    = 3163,
    NPC_AUCTION_H   = 9856,
    NPC_AUCTION_A   = 8670
};

enum Trainers
{
    // Alliance
    DRUID_A     = 5504,
    HUNTER_A    = 5515,
    MAGE_A      = 5497,
    PALADIN_A   = 928,
    PRIEST_A    = 376,
    ROGUE_A     = 918,
    SHAMAN_A    = 20407,
    WARLOCK_A   = 461,
    WARRIOR_A   = 5479,

    // Horde
    DRUID_H     = 3033,
    HUNTER_H    = 3406,
    MAGE_H      = 5883,
    PALADIN_H   = 23128,
    PRIEST_H    = 3045,
    ROGUE_H     = 3401,
    SHAMAN_H    = 3344,
    WARLOCK_H   = 3324,
    WARRIOR_H   = 3354,

    DEATHKNIGHT_AH  = 28472
};

enum Mounts
{
    HUMAN_MOUNT     = 22717,
    ORC_MOUNT       = 22724,
    GNOME_MOUNT     = 22719,
    NIGHTELF_MOUNT  = 22723,
    DWARF_MOUNT     = 22720,
    UNEAD_MOUNT     = 22722,
    TAUREN_MOUNT    = 22718,
    TROLL_MOUNT     = 22721,
    BLOODELF_MOUNT  = 35028,
    DRAENEI_MOUNT   = 48027
};

class premium_account : public ItemScript
{
public:
    premium_account() : ItemScript("premium_account") { }

    bool OnUse(Player* player, Item* item, SpellCastTargets const& /*targets*/) override
    {
        if (!sConfigMgr->GetOption<bool>("PremiumAccount", true))
            return false;

        if (!player->IsPremium())
            return false;

        if (player->IsInCombat())
            return false;

        float rangeCheck = 10.0f;
        if (player->FindNearestCreature(NPC_AUCTION_A, rangeCheck) ||
            player->FindNearestCreature(NPC_AUCTION_H, rangeCheck) ||
            player->FindNearestCreature(NPC_VENDOR_A, rangeCheck) ||
            player->FindNearestCreature(NPC_VENDOR_H, rangeCheck) ||
            player->FindNearestCreature(ROGUE_A, rangeCheck) ||
            player->FindNearestCreature(WARRIOR_A, rangeCheck) ||
            player->FindNearestCreature(HUNTER_A, rangeCheck) ||
            player->FindNearestCreature(PRIEST_A, rangeCheck) ||
            player->FindNearestCreature(PALADIN_A, rangeCheck) ||
            player->FindNearestCreature(DRUID_A, rangeCheck) ||
            player->FindNearestCreature(SHAMAN_A, rangeCheck) ||
            player->FindNearestCreature(MAGE_A, rangeCheck) ||
            player->FindNearestCreature(WARLOCK_A, rangeCheck) ||
            player->FindNearestCreature(HUNTER_H, rangeCheck) ||
            player->FindNearestCreature(WARRIOR_H, rangeCheck) ||
            player->FindNearestCreature(SHAMAN_H, rangeCheck) ||
            player->FindNearestCreature(PALADIN_H, rangeCheck) ||
            player->FindNearestCreature(ROGUE_H, rangeCheck) ||
            player->FindNearestCreature(WARLOCK_H, rangeCheck) ||
            player->FindNearestCreature(MAGE_H, rangeCheck) ||
            player->FindNearestCreature(PRIEST_H, rangeCheck) ||
            player->FindNearestCreature(DRUID_H, rangeCheck) ||
            player->FindNearestCreature(DEATHKNIGHT_AH, rangeCheck))
            return false;

        ClearGossipMenuFor(player);

        if (sConfigMgr->GetOption<bool>("Morph", true))
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface/ICONS/Ability_Hunter_Resourcefulness:30:30:-25:0|tMorph me!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        if (sConfigMgr->GetOption<bool>("Mount", true))
            AddGossipItemFor(player, GOSSIP_ICON_CHAT_16, "|TInterface/ICONS/Ability_Mount_WarHippogryph:30:30:-25:0|tSummon a Mount.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);

        if (sConfigMgr->GetOption<bool>("PlayerInteraction", true))
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface/ICONS/Ability_Repair:30:30:-25:0|tRepair Equipment.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 21);

        // Argast Additions
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface/ICONS/Spell_Arcane_TeleportTheramore:30:30:-25:0|tTeleport to a capital", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10); // cities & dalaran

            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface/ICONS/Spell_Arcane_TeleportDalaran:30:30:-25:0|tTeleport to Dalaran", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 17);

            if (sConfigMgr->GetOption<bool>("Trainers", true))
                AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "|TInterface/ICONS/Spell_Holy_Penance:30:30:-25:0|tLearn class skills", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);

            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface/ICONS/INV_Letter_07:30:30:-25:0|tMailbox", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);

            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface/ICONS/INV_Misc_Bag_15:30:30:-25:0|tMy Bank", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);


        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, item->GetGUID());

        return false; // Cast the spell on use normally
    }

    void OnGossipSelect(Player* player, Item* item, uint32 /*sender*/, uint32 action) override
    {
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF + 1: /*Morph*/
            {
                CloseGossipMenuFor(player);
                ApplyRandomMorph(player);
                break;
            }
            case GOSSIP_ACTION_INFO_DEF + 2: /*Demorph*/
            {
                player->DeMorph();
                CloseGossipMenuFor(player);
                break;
            }
            case GOSSIP_ACTION_INFO_DEF + 3: /*Show Bank*/
            {
                player->GetSession()->SendShowBank(player->GetGUID());
                break;
            }
            case GOSSIP_ACTION_INFO_DEF + 4: /*Mail Box*/
            {
                player->GetSession()->SendShowMailBox(player->GetGUID());
                break;
            }
            case GOSSIP_ACTION_INFO_DEF + 5: /*Vendor*/
            {
                uint32 vendorId = 0;
                std::string salute;
                if (player->GetTeamId() == TEAM_ALLIANCE)
                {
                    vendorId = NPC_VENDOR_A;
                    salute = "Greetings";
                } else {
                    vendorId = NPC_VENDOR_H;
                    salute = "Zug zug";
                }

                SummonTempNPC(player, vendorId, salute.c_str());
                CloseGossipMenuFor(player);
                break;
            }
            case GOSSIP_ACTION_INFO_DEF + 6: /*Mount*/
            {
                CloseGossipMenuFor(player);
                switch (player->getRace())
                {
                    case RACE_HUMAN:         player->CastSpell(player, HUMAN_MOUNT); break;
                    case RACE_ORC:           player->CastSpell(player, ORC_MOUNT); break;
                    case RACE_GNOME:         player->CastSpell(player, GNOME_MOUNT); break;
                    case RACE_NIGHTELF:      player->CastSpell(player, NIGHTELF_MOUNT); break;
                    case RACE_DWARF:         player->CastSpell(player, DWARF_MOUNT); break;
                    case RACE_DRAENEI:       player->CastSpell(player, DRAENEI_MOUNT); break;
                    case RACE_UNDEAD_PLAYER: player->CastSpell(player, UNEAD_MOUNT); break;
                    case RACE_TAUREN:        player->CastSpell(player, TAUREN_MOUNT); break;
                    case RACE_TROLL:         player->CastSpell(player, TROLL_MOUNT); break;
                    case RACE_BLOODELF:      player->CastSpell(player, BLOODELF_MOUNT); break;
                }
                break;
            }
            case GOSSIP_ACTION_INFO_DEF + 7: /*Auction House*/
            {
                uint32 auctionId = 0;
                std::string salute;
                if (player->GetTeamId() == TEAM_HORDE)
                {
                    auctionId = NPC_AUCTION_H;
                    salute = "I will go shortly, I need to get back to Orgrimmar";
                }
                else
                {
                    auctionId = NPC_AUCTION_A;
                    salute = "I will go shortly, I need to get back to Stormwind City";
                }

                SummonTempNPC(player, auctionId, salute.c_str());
                CloseGossipMenuFor(player);
                break;
            }
            case GOSSIP_ACTION_INFO_DEF + 8: /* Class Trainers*/
            {
                uint32 trainerId = 0;
                switch (player->getClass())
                {
                    case CLASS_ROGUE:
                        trainerId = player->GetTeamId() == TEAM_ALLIANCE ? ROGUE_A : ROGUE_H;
                        break;
                    case CLASS_WARRIOR:
                        trainerId = player->GetTeamId() == TEAM_ALLIANCE ? WARRIOR_A : WARRIOR_H;
                        break;
                    case CLASS_PRIEST:
                        trainerId = player->GetTeamId() == TEAM_ALLIANCE ? PRIEST_A : PRIEST_H;
                        break;
                    case CLASS_MAGE:
                        trainerId = player->GetTeamId() == TEAM_ALLIANCE ? MAGE_A : MAGE_H;
                        break;
                    case CLASS_PALADIN:
                        trainerId = player->GetTeamId() == TEAM_ALLIANCE ? PALADIN_A : PALADIN_H;
                        break;
                    case CLASS_HUNTER:
                        trainerId = player->GetTeamId() == TEAM_ALLIANCE ? HUNTER_A : HUNTER_H;
                        break;
                    case CLASS_DRUID:
                        trainerId = player->GetTeamId() == TEAM_ALLIANCE ? DRUID_A : DRUID_H;
                        break;
                    case CLASS_SHAMAN:
                        trainerId = player->GetTeamId() == TEAM_ALLIANCE ? SHAMAN_A : SHAMAN_H;
                        break;
                    case CLASS_WARLOCK:
                        trainerId = player->GetTeamId() == TEAM_ALLIANCE ? WARLOCK_A : WARLOCK_H;
                        break;
                    case CLASS_DEATH_KNIGHT:
                        trainerId = DEATHKNIGHT_AH;
                        break;
                }

                SummonTempNPC(player, trainerId);
                CloseGossipMenuFor(player);
                break;
            }
            case GOSSIP_ACTION_INFO_DEF + 9: /*Player Interactions*/
            {
                ClearGossipMenuFor(player);

                if (sConfigMgr->GetOption<bool>("Vendor", true))
                    AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "|TInterface/ICONS/INV_Hammer_20:30:30:-25:0|tVendor", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);

                if (sConfigMgr->GetOption<bool>("MailBox", true))
                    AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "|TInterface/ICONS/INV_Letter_03:30:30:-25:0|tMailman", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);

                if (sConfigMgr->GetOption<bool>("Bank", true))
                    AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "|TInterface/ICONS/INV_Ore_Gold_01:30:30:-25:0|tShow me my Bank", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);

                if (sConfigMgr->GetOption<bool>("Auction", true))
                    AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "|TInterface/ICONS/Spell_Shaman_SpectralTransformation:30:30:-25:0|tAuctioneer", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);

                SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, item->GetGUID());
                break;
            }

            case GOSSIP_ACTION_INFO_DEF + 10: /*Teleport*/
            {
                ClearGossipMenuFor(player);

                if (player->GetTeamId() == TEAM_ALLIANCE)
                {
                    AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "|TInterface/ICONS/Spell_Arcane_TeleportDarnassus:30:30:-25:0|tDarnassus", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
                    AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "|TInterface/ICONS/Spell_Arcane_TeleportStormWind:30:30:-25:0|tStormwind", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 12);
                    AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "|TInterface/ICONS/Spell_Arcane_TeleportIronForge:30:30:-25:0|tIronforge", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 13);
                    AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "|TInterface/ICONS/Spell_Arcane_TeleportExodar:30:30:-25:0|tExodar", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 18);
                }
                else
                {
                    AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "|TInterface/ICONS/Spell_Arcane_TeleportThunderBluff:30:30:-25:0|tThunderbluff", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 14);
                    AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "|TInterface/ICONS/Spell_Arcane_TeleportUnderCity:30:30:-25:0|tUndercity", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 15);
                    AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "|TInterface/ICONS/Spell_Arcane_TeleportOrgrimmar:30:30:-25:0|tOrgrimmar", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 16);
                    AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "|TInterface/ICONS/Spell_Arcane_TeleportSilvermoon:30:30:-25:0|tSilvermoon", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 20);
                }

                AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "|TInterface/ICONS/Spell_Arcane_TeleportDalaran:30:30:-25:0|tDalaran", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 17);
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "|TInterface/ICONS/Spell_Arcane_TeleportShattrath:30:30:-25:0|tShattrath", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 19);

                SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, item->GetGUID());
                break;
            }

            case GOSSIP_ACTION_INFO_DEF + 11: /*Darnassus*/
            {
                player->TeleportTo(WorldLocation(1, 10150.45f, 2602.12f, 1330.82f, 5.03f));
                break;
            }

            case GOSSIP_ACTION_INFO_DEF + 12: /*Stormwind*/
            {
                player->TeleportTo(WorldLocation(0, -8748.27f, 1074.27f, 90.52f, 4.17f));
                break;
            }

            case GOSSIP_ACTION_INFO_DEF + 13: /*Ironforge*/
            {
                player->TeleportTo(WorldLocation(0, -4663.39f, -956.23f, 500.37f, 5.73f));
                break;
            }

            case GOSSIP_ACTION_INFO_DEF + 14: /*Thunderbluff*/
            {
                player->TeleportTo(WorldLocation(1, -1031.73f, -230.42f, 160.18f, 3.12f));
                break;
            }

            case GOSSIP_ACTION_INFO_DEF + 15: /*Undercity*/
            {
                player->TeleportTo(WorldLocation(0, 1642.41f, 239.9f, 62.59f, 3.01f));
                break;
            }

            case GOSSIP_ACTION_INFO_DEF + 16: /*Orgrimmar*/
            {
                player->TeleportTo(WorldLocation(1, 1971.18f, -4259.45f, 32.21f, 4.0f));
                break;
            }

            case GOSSIP_ACTION_INFO_DEF + 17: /*Dalaran*/
            {
                player->TeleportTo(WorldLocation(571, 5804.14f, 624.77f, 647.76f, 1.64f));
                break;
            }

            case GOSSIP_ACTION_INFO_DEF + 18: /*Exodar*/
            {
                player->TeleportTo(WorldLocation(530, -3965.69f, -11653.59f, -138.84f, 1.64f));
                break;
            }

            case GOSSIP_ACTION_INFO_DEF + 19: /*Shattrath*/
            {
                player->TeleportTo(WorldLocation(530, -1838.16f, 5301.79f, -12.42f, 5.95f));
                break;
            }

            case GOSSIP_ACTION_INFO_DEF + 20: /*Silvermoon*/
            {
                player->TeleportTo(WorldLocation(530, 9487.69f, -7279.20f, 14.28f, 6.16f));
                break;
            }

            case GOSSIP_ACTION_INFO_DEF + 21: /*Repair*/
            {
                player->DurabilityRepairAll(false, 0, false);
                break;
            }

            CloseGossipMenuFor(player);
            break;
        }
    }

    void ApplyRandomMorph(Player* player)
    {
        uint32 random = (urand(1, 26)); // Change this line when adding more morphs
        {
            switch (random)
            {
                case 1: player->SetDisplayId(10134); break;     // Troll Female                 'Orb of Deception'
                case 2: player->SetDisplayId(10135); break;     // Troll Male                   'Orb of Deception'
                case 3: player->SetDisplayId(10136); break;     // Tauren Male                  'Orb of Deception'
                case 4: player->SetDisplayId(10137); break;     // Human Male                   'Orb of Deception'
                case 5: player->SetDisplayId(10138); break;     // Human Female                 'Orb of Deception'
                case 6: player->SetDisplayId(10139); break;     // Orc Male                     'Orb of Deception'
                case 7: player->SetDisplayId(10140); break;     // Orc Female                   'Orb of Deception' 
                case 8: player->SetDisplayId(10141); break;     // Dwarf Male                   'Orb of Deception'
                case 9: player->SetDisplayId(10142); break;     // Dwarf Female                 'Orb of Deception' 
                case 10: player->SetDisplayId(10143); break;    // NightElf Male                'Orb of Deception'
                case 11: player->SetDisplayId(10144); break;    // NightElf Female              'Orb of Deception'
                case 12: player->SetDisplayId(10145); break;    // Undead Female                'Orb of Deception'
                case 13: player->SetDisplayId(10146); break;    // Undead Male                  'Orb of Deception'
                case 14: player->SetDisplayId(10147); break;    // Tauren Female                'Orb of Deception'
                case 15: player->SetDisplayId(10148); break;    // Gnome Male                   'Orb of Deception'
                case 16: player->SetDisplayId(10149); break;    // Gnome Female                 'Orb of Deception'
                case 17: player->SetDisplayId(4527); break;     // Thrall                       'Orgrimmar Boss'
                case 18: player->SetDisplayId(11657); break;    // Lady Sylvanas                'Undercity Boss'
                case 19: player->SetDisplayId(4307); break;     // Cairne Bloodhoof             'Thunderbluff Boss'
                case 20: player->SetDisplayId(17122); break;    // Lor'themar Theron            'Silvermoon City Boss'
                case 21: player->SetDisplayId(3597); break;     // King Magni Bronzebeard       'Ironforge Boss'
                case 22: player->SetDisplayId(5566); break;     // Highlord Bolvar Fordragon    'Stormwind Boss'
                case 23: player->SetDisplayId(7006); break;     // High Tinker Mekkatorque      'Gnomer Boss'
                case 24: player->SetDisplayId(7274); break;     // Tyrande Whisperwind          'Darnassus Boss'
                case 25: player->SetDisplayId(21976); break;    // Arthus Small                 'Arthus'
                case 26: player->SetDisplayId(24641); break;    // Arthus Ghost                 'Arthus Ghost'

                default:
                    break;
            }
        }
    }

    void SummonTempNPC(Player* player, uint32 entry, const char* salute = "")
    {
        if (!player || entry == 0)
            return;

        int npcDuration = sConfigMgr->GetOption<int32>("Premium.NpcDuration", 60) * IN_MILLISECONDS;
        if (npcDuration <= 0) // Safeguard
            npcDuration = 60;

        Creature* npc = player->SummonCreature(entry, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, npcDuration);
        npc->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        npc->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, player->GetFollowAngle());
        npc->SetFaction(player->GetFaction());

        if (salute && !(salute[0] == '\0'))
            npc->Whisper(salute, LANG_UNIVERSAL, player, false);
    }
};

void AddPremiumAccount()
{
    new premium_account();
}
