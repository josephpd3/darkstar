-----------------------------------
-- Area: Yhoator Jungle
--   NM: Kappa Bonze
-- Involved in Quest: True will
-----------------------------------
local ID = require("scripts/zones/Yhoator_Jungle/IDs")
mixins = {require("scripts/mixins/job_special")}
require("scripts/globals/quests")
-----------------------------------

function onMobInitialize(mob)
    mob:setMobMod(dsp.mobMod.IDLE_DESPAWN, 180)
end

function onMobDeath(mob, player, isKiller)
    if player:getQuestStatus(OUTLANDS, dsp.quest.id.outlands.TRUE_WILL) == QUEST_ACCEPTED then
        local lastNM = not (GetMobByID(ID.mob.KAPPA_AKUSO):isAlive() or GetMobByID(ID.mob.KAPPA_BIWA):isAlive())
        if lastNM then -- Only count the kill for the last alive/spawned NM dying
            player:addCharVar("trueWillKilledNM", 1)
        end
    end
end