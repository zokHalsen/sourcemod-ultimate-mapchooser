//Header file for Ultimate Mapchooser v3.0 by Steell.

#if defined _umc_core_included
    #endinput
#endif
#define _umc_core_included

#pragma semicolon 1

#define PL_VERSION "3.1.1"

#define UMC_DEBUG 0

#define INVALID_GROUP ""
#define MAP_LENGTH 64

#define MAP_TRIE_MAP_KEY   "map"
#define MAP_TRIE_GROUP_KEY "group"

enum UMC_VoteType
{
    VoteType_Map = 0,
    VoteType_Group,
    VoteType_Tier
}

enum UMC_ChangeMapTime
{
    ChangeMapTime_Now = 0,
    ChangeMapTime_RoundEnd,
    ChangeMapTime_MapEnd
}

enum UMC_VoteFailAction
{
    VoteFailAction_Nothing = 0,
    VoteFailAction_Runoff
}

enum UMC_RunoffFailAction
{
    RunoffFailAction_Nothing = 0,
    RunoffFailAction_Accept
}


/**
 * Called when a nomination was removed from UMC.
 *
 * @param map  The name of the map
 * @param client    The client that the nomination belonged to.
 *
 * @noreturn
 */
forward OnNominationRemoved(const String:map[], client);
 


/**
 * Called when a map's weight is being reweighted. Allows plugin to modify the
 * weight by calling UMC_AddWeightModifier.
 *
 * @param kv    The mapcycle being used.
 * @param map   The name of the map we're reweighting.
 * @param group The group that the map belongs to.
 * @noreturn
 */
forward UMC_OnReweightMap(Handle:kv, const String:map[], const String:group[]);


/**
 * Called when a group's weight is being reweighted.
 *
 * @param kv    The mapcycle being used
 * @param group The group being reweighted.
 *
 * @noreturn
 */
forward UMC_OnReweightGroup(Handle:kv, const String:group[]);


/**
 * Adds a MapWeightModifier to UMC's internal modifier list.
 *
 * @param amt  Amount to modify the currently weighted map's weight with.
 *
 * @noreturn
 */
native UMC_AddWeightModifier(Float:amt);


/**
 * Called when a map is being considered for exclusion.
 *
 * @param kv    The mapcycle being used.
 * @param map   The name of the map being considered.
 * @param group The group that the map belongs to.
 * @param isNomination  Specifies whether this is a nominated map or not.
 * @param forMapChange  Whether or not this map will be used this game frame for a mapchange.
 * 
 * @return  Plugin_Continue to keep the map, Plugin_Stop to exclude the map.
 */
forward Action:UMC_OnDetermineMapExclude(Handle:kv, const String:map[], const String:group[],
    bool:isNomination, bool:forMapChange);
 
 
/**
 * Starts a UMC map vote.
 *
 * @param kv    The mapcycle to use for the vote.
 * @param type  The type of vote.
 * @param time  How long the vote should remain acive (in seconds).
 * @param excludedMaps  adt_array of maps to be excluded from the vote.
 * @param excludedGroups    adt_array of groups corresponding to maps excluded from the vote.
 * @param numExcludedGroups Amount of groups in the excluded groups array to be excluded from the vote.
 * @param scramble  Determines if the vote menu is scrambled.
 * @param numBlockSlots Amount of slots in the vote to be blocked.
 * @param startSound    Sound to be played when the vote starts (must be precached already).
 * @param endSound      Sound to be played when the vote ends (must be precached already).
 * @param extendOption  Determines if an "Extend Map" option is displayed.
 * @param timestep       How long to extend the time limit.
 * @param roundstep      How long to extend the round limit.
 * @param fragstep       How long to extend the frag limit.
 * @param dontChangeOption  Determines if a "Don't Change" option is displayed.
 * @param threshold Percentage winning vote must exceed in order for the vote to change the map.
 * @param successAction Action to be taken if the vote is successful (threshold was exceeded).
 * @param failAction    Action to be taken if the vote is unsuccessful (threshold wasn't exceeded).
 * @param maxRunoffs    Maximum amount of runoffs that will be performed. 0 = no max
 * @param maxRunoffMaps Maximum amount of maps that appear in runoffs. 0 = no max
 * @param runoffFailAction  Action to be taken should the maximum amount of runoffs is reached and there is still no winning map.
 * @param runoffSound   Sound to be played when the runoff vote starts (must be precached already).
 * @param nominationStrictness  Specifies how nominations should be handled.
 * @param allowDuplicates   Determines if the same map (in different groups) can appear in the vote.
 * @param adminFlags    String of flags required for players to see the vote.
 * @param runExclusion  Determines if UMC will filter the mapcycle through the Map Exclusion system.
 * 
 * @return True on success, false on failure.
 */
native bool:UMC_StartVote(Handle:kv, UMC_VoteType:type, time, Handle:excludedMaps,
    Handle:excludedGroups, numExcludedGroups, bool:scramble, numBlockSlots,
    const String:startSound[], const String:endSound[], bool:extendOption, Float:timestep, roundstep,
    fragstep, bool:dontChangeOption, Float:threshold, UMC_ChangeMapTime:successAction,
    UMC_VoteFailAction:failAction, maxRunoffs, maxRunoffMaps, UMC_RunoffFailAction:runoffFailAction,
    const String:runoffSound[], bool:nominationStrictness, bool:allowDuplicates,
    const String:adminFlags[], bool:runExclusion=true);


/**
 * Fetches a random map from a given map group in a given mapcycle.
 *
 * @param kv    Mapcycle to look for a map in.
 * @param group Group to look for a map in. If INVALID_GROUP then it will look in all groups.
 * @param buffer    Buffer to store the fetched map in.
 * @param size  Max size of the buffer.
 * @param groupBuffer   Buffer to store the group of the map in.
 * @param gBufferSize   Max size of the group buffer.
 * @param excludedMaps  adt_array of maps to be excluded from the vote.
 * @param excludedGroups adt_attay of groups corresponding to maps excluded from the vote.
 * @param numExGroups   Amount of groups in the excludedGroups adt_array to be excluded.
 * @param isNomination Whether or not this map should be treated as a nomination.
 * @param forMapChange Whether or not this map will be used this game frame for a mapchange.
 *
 * @return True on success, false on failure.
 */
native bool:UMC_GetRandomMap(Handle:kv, const String:group[], String:buffer[], size,
                             String:groupBuffer[], gBufferSize, Handle:excludedMaps,
                             Handle:excludedGroups, numExGroups, bool:isNomination,
                             bool:forMapChange);
                             
                             
/**
 * Sets the next map and map group for UMC.
 *
 * @param kv    Mapcycle containing the map and map group.
 * @param map   The name of the map
 * @param group The name of the map group containing the map.
 * @param when  When the map should be changed to the set next map.
 * 
 * @noreturn
 */
native UMC_SetNextMap(Handle:kv, const String:map[], const String:group[], UMC_ChangeMapTime:when);


/**
 * Determines if the given map from the given group is nominated.
 *
 * @param map   The name of the map.
 * @param group The name of the map group containing the map.
 *
 * @return True if the map is nominated, false otherwise.
 */
native bool:UMC_IsMapNominated(const String:map[], const String:group[]);


/**
 * Nominates a map for UMC.
 *
 * @param kv    Mapcycle containing the nomination
 * @param map   The name of the map.
 * @param group The name of the map group containing the map.
 * @param client    The client who is nominating this map.
 * @param nominationGroup   The group which this nomination should be associated with. If INVALID_GROUP, it will be associated with the given group.
 *
 * @return True on success, false otherwise.
 */
native bool:UMC_NominateMap(Handle:kv, const String:map[], const String:group[], client, 
                            const String:nominationGroup[]=INVALID_HANDLE);
 

/**
 * Determines if the given group is valid (has at least 1 valid map in it) for the given mapcycle.
 *
 * @param kv    Mapcycle containing the group.
 * @param group The group to be checked.
 * 
 * @return True if the group is valid, false otherwise.
 */
//native bool:UMC_IsGroupValid(Handle:kv, const String:group[]);


/**
 * Determines if the given map is valid for the given mapcycle.
 *
 * @param kv    Mapcycle containing the map.
 * @param map   The name of the map.
 * @param group The name of the group containing the map.
 *
 * @return True if the map is valid, false otherwise.
 */
native bool:UMC_IsMapValid(Handle:kv, const String:map[], const String:group[], Handle:exMaps,
                           Handle:exGroups, bool:isNom, bool:forMapChange);


/**
 * Returns a copy of the given mapcycle with all invalid maps and groups filtered out.
 *
 * @param kv    Mapcycle to filter.
 * @param isNomination  Are we filtering for nomination?
 * @param forMapChange  Are we filtering for maps we will be using this frame for a map change?
 *
 * @return Handle to the new mapcycle KV.
 */
//native Handle:UMC_FilterMapcycle(Handle:kv, bool:isNomination, bool:forMapChange);


/**
 * Returns an array consisting of all the valid maps in the given mapcycle.
 * 
 * @param kv    Mapcycle to look for maps in.
 * @param group Group to select maps from. If INVALID_GROUP, all groups are looked in.
 * @param excludedMaps  adt_array of maps to be excluded.
 * @param excludedGroups    adt_array of groups corresponding to the maps in excludedMaps.
 * @param numExcludedGroups Amount of groups in the excludedGroups array to be excluded.
 * @param isNom Are these maps to be considered nominations?
 * @param forMapChange  Are any of these maps going to be used to change the map this game frame?
 *
 * @return Handle to the new array containing all of the valid map tries.
 */
native Handle:UMC_CreateValidMapArray(Handle:kv, const String:group[], Handle:excludedMaps,
                                      Handle:excludedGroups, numExcludedGroups, bool:isNom, 
                                      bool:forMapChange);


/**
 * Returns an array consisting of all the valid groups in the given mapcycle. A valid group is a
 * group with 1 or more valid maps in it.
 *
 * @param kv    Mapcycle to look for maps in.
 * @param excludedMaps  adt_array of maps to be excluded
 * @param excludedGroups    adt_array of groups corresponding to the maps in excludedMaps.
 * @param numExcludedGroups Amound of groups in the excludedGroups array to be excluded.
 * @param isNom Are the maps in these groups to be considered nominations?
 * @param forMapChange  Are any of the maps in these groups going to be used to change the map this game frame?
 *
 * @return Handle to the new array containing all of the valid map groups.
 */
native Handle:UMC_CreateValidMapGroupArray(Handle:kv, Handle:excludedMaps, Handle:excludedGroups,
                                           numExcludedGroups, bool:isNom, bool:forMapChange);


/**
 * Called when has extended the map.
 * 
 * @noreturn
 */
forward UMC_OnMapExtended();


/**
 * Called when a vote has failed. This can be due to the Don't Change option winning the vote or the
 * winning map not winning by a high enough percentage.
 * 
 * @noreturn
 */
forward UMC_OnVoteFailed(); //UMC_FailReason:reason);


/**
 * Called immediately after UMC has set a next map.
 *
 * @param map   The name of the winning map.
 * @param group The group fo the winning map.
 * @param kv    The mapcycle used for the vote.
 * 
 * @noreturn
 */
forward UMC_OnNextmapSet(Handle:kv, const String:map[], const String:group[]);


/**
 * Called when the "sm_umc_reload" command is used. If your plugin has its own mapcycle, you should
 * reload it in this forward.
 *
 * @noreturn
 */
forward UMC_RequestReloadMapcycle();


/**
 * Retrieves the name of the current map group.
 *
 * @param buffer    Buffer to store the group name in.
 * @param size  Maximum length of the buffer.
 *
 * @noreturn
 */
native UMC_GetCurrentMapGroup(String:buffer[], size);


/**
 * Determines if UMC currently has a vote in progress.
 *
 * @return True if vote is in progress, false otherwise.
 */
native bool:UMC_IsVoteInProgress();


//SourceMod Malarky
public SharedPlugin:__pl_umccore =
{
    name = "umccore",
    file = "umc-core.smx",
#if defined REQUIRE_PLUGIN
    required = 1,
#else
    required = 0,
#endif
};

public __pl_umccore_SetNTVOptional()
{
    MarkNativeAsOptional("UMC_AddWeightModifier");
    MarkNativeAsOptional("UMC_StartVote");
    MarkNativeAsOptional("UMC_GetCurrentMapGroup");
    MarkNativeAsOptional("UMC_GetRandomMap");
    MarkNativeAsOptional("UMC_SetNextMap");
    MarkNativeAsOptional("UMC_IsMapNominated");
    MarkNativeAsOptional("UMC_NominateMap");
    MarkNativeAsOptional("UMC_CreateValidMapArray");
    MarkNativeAsOptional("UMC_CreateValidMapGroupArray");
    MarkNativeAsOptional("UMC_IsMapValid");
    //MarkNativeAsOptional("UMC_IsGroupValid");
    //MarkNativeAsOptional("UMC_FilterMapcycle");
    MarkNativeAsOptional("UMC_IsVoteInProgress");
}
