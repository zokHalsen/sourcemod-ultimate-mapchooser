/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                Ultimate Mapchooser - Map Weight                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
 
#pragma semicolon 1
#define PL_VERSION "3.0"

#include <sourcemod>
#include <umc-core>

public Plugin:myinfo =
{
    name = "[UMC] Map Weight",
    author = "Steell",
    description = "Allows users to specify weights for maps and groups, making them more or less likely to be picked randomly.",
    version = PL_VERSION,
    url = "http://forums.alliedmods.net/showthread.php?t=134190"
}

#define WEIGHT_KEY_MAP   "weight"
#define WEIGHT_KEY_GROUP "group_weight"


//Reweights a map when UMC requests.
public UMC_OnReweightMap(Handle:kv, const String:map[], const String:group[])
{
    KvRewind(kv);
    if (KvJumpToKey(kv, group))
    {
        if (KvJumpToKey(kv, map))
        {
            UMC_AddWeightModifier(KvGetFloat(kv, WEIGHT_KEY_MAP, 1.0));
            KvGoBack(kv);
        }
        KvGoBack(kv);
    }
}


//Reweights a group when UMC requests.
public UMC_OnReweightGroup(Handle:kv, const String:group[])
{
    KvRewind(kv);
    if (KvJumpToKey(kv, group))
    {
        UMC_AddWeightModifier(KvGetFloat(kv, WEIGHT_KEY_GROUP, 1.0));
        KvGoBack(kv);
    }
}