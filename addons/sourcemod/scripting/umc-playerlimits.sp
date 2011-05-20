/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                              Ultimate Mapchooser - Player Limits                              *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
 
#pragma semicolon 1

#include <sourcemod>
#include <umc-core>
#include <umc_utils>
#include <umc-playerlimits>

public Plugin:myinfo =
{
    name = "[UMC] Player Limits",
    author = "Steell",
    description = "Allows users to specify player limits for maps.",
    version = PL_VERSION,
    url = "http://forums.alliedmods.net/showthread.php?t=134190"
}

new Handle:cvar_nom_ignore = INVALID_HANDLE;
new Handle:cvar_display_ignore = INVALID_HANDLE;

public OnPluginStart()
{
    cvar_nom_ignore = CreateConVar(
        "sm_umc_playerlimits_nominations",
        "0",
        "Determines if nominations are exempt from being excluded due to Player Limits.",
        0, true, 0.0, true, 1.0
    );
    
    cvar_display_ignore = CreateConVar(
        "sm_umc_playerlimits_display",
        "0",
        "Determines if maps being displayed are exempt from being excluded due to Player Limits.",
        0, true, 0.0, true, 1.0
    );
    
    AutoExecConfig(true, "umc-playerlimits");
}


//Called when UMC wants to know if this map is excluded
public Action:UMC_OnDetermineMapExclude(Handle:kv, const String:map[], const String:group[],
                                        bool:isNomination, bool:forMapChange)
{
    if (isNomination && GetConVarBool(cvar_nom_ignore))
        return Plugin_Continue;
        
    if (!forMapChange && GetConVarBool(cvar_display_ignore))
        return Plugin_Continue;
    
    new defaultMin, defaultMax;
    new min, max;
    
    KvRewind(kv);
    if (KvJumpToKey(kv, group))
    {
        defaultMin = KvGetNum(kv, PLAYERLIMIT_KEY_GROUP_MIN, 0);
        defaultMax = KvGetNum(kv, PLAYERLIMIT_KEY_GROUP_MAX, MaxClients);
    
        if (KvJumpToKey(kv, map))
        {    
            min = KvGetNum(kv, PLAYERLIMIT_KEY_MAP_MIN, defaultMin);
            max = KvGetNum(kv, PLAYERLIMIT_KEY_MAP_MAX, defaultMax);
            KvGoBack(kv);
        }
        KvGoBack(kv);
    }
    
    if (IsPlayerCountBetween(min, max))
        return Plugin_Continue;
    
    return Plugin_Stop;
}