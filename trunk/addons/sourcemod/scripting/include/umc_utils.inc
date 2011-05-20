#if defined _umc_utils_included
    #endinput
#endif
#define _umc_utils_included

#pragma semicolon 1

#include <sourcemod>

#include <umc-core>

#include <sdktools_functions>
#include <sdktools_entinput>
#include <sdktools_stringtables>

//************************************************************************************************//
//                                        GENERAL UTILITIES                                       //
//************************************************************************************************//

//Utility function to build a map trie.
stock Handle:CreateMapTrie(const String:map[], const String:group[])
{
    new Handle:trie = CreateTrie();
    SetTrieString(trie, MAP_TRIE_MAP_KEY, map);
    SetTrieString(trie, MAP_TRIE_GROUP_KEY, group);
    return trie;
}


//Utility function to get a KeyValues Handle from a filename, with the specified root key.
stock Handle:GetKvFromFile(const String:filename[], const String:rootKey[])
{
    new Handle:kv = CreateKeyValues(rootKey);
    
    //Log an error and return empty handle if...
    //    ...the kv file fails to parse.
    if (!FileToKeyValues(kv, filename))
    {
        LogError("KV ERROR: Unable to load KV file: %s", filename);
        return INVALID_HANDLE;
    }
    
    //Log success message and return handle.
    //LogMessage("SETUP: KV file \"%s\" successfully loaded.", filename);
    return kv;
}


//Utility function to jump to a specific map in a mapcycle.
//  kv: Mapcycle Keyvalues that must be at the root value.
stock bool:KvJumpToMap(Handle:kv, const String:map[])
{
    if (!KvGotoFirstSubKey(kv))
        return false;
        
    decl String:mapName[MAP_LENGTH];
    
    do
    {
        if (!KvGotoFirstSubKey(kv))
            continue;
        do
        {
            KvGetSectionName(kv, mapName, sizeof(mapName));
            if (StrEqual(mapName, map))
                return true;
        } while (KvGotoNextKey(kv));
        
        KvGoBack(kv);
    } while (KvGotoNextKey(kv));
    
    KvGoBack(kv);
    return false;
}


//Utility function to search for a group that contains the given map.
//  kv: Mapcycle
//  map: Map whose group we're looking for.
//  buffer: Buffer to store the found group name.
//  maxlen: Maximum length of the buffer.
stock bool:KvFindGroupOfMap(Handle:kv, const String:map[], String:buffer[], maxlen)
{
    if (!KvGotoFirstSubKey(kv))
        return false;
    
    decl String:mapName[MAP_LENGTH], String:groupName[MAP_LENGTH];
    do
    {
        KvGetSectionName(kv, groupName, sizeof(groupName));
        
        if (!KvGotoFirstSubKey(kv))
            continue;
        
        do
        {
            KvGetSectionName(kv, mapName, sizeof(mapName));
            if (StrEqual(mapName, map, false))
            {
                KvGoBack(kv);
                KvGoBack(kv);
                strcopy(buffer, maxlen, groupName);
                return true;
            }
        }
        while (KvGotoNextKey(kv));
        
        KvGoBack(kv);
    }
    while (KvGotoNextKey(kv));
    
    KvGoBack(kv);
    
    return false;
}


//Color Arrays for colors in warning messages.
static g_iSColors[7]             = {1, 3, 3, 4, 4, 5, 6};
static String:g_sSColors[7][13]  = {"{DEFAULT}", "{LIGHTGREEN}", "{TEAM}", "{GREEN}", "{RED}",
                                    "{DARKGREEN}", "{YELLOW}"};
static g_iTColors[13][3]         = {{255, 255, 255}, {255,   0,   0}, {  0, 255,   0}, 
                                    {  0,   0, 255}, {255, 255,   0}, {255,   0, 255},
                                    {  0, 255, 255}, {255, 128,   0}, {255,   0, 128},
                                    {128, 255,   0}, {  0, 255, 128}, {128,   0, 255}, 
                                    {  0, 128, 255}};
static String:g_sTColors[13][12] = {"{WHITE}", "{RED}", "{GREEN}", "{BLUE}", "{YELLOW}", "{PURPLE}",
                                    "{CYAN}", "{ORANGE}", "{PINK}", "{OLIVE}", "{LIME}", "{VIOLET}",    
                                    "{LIGHTBLUE}"};

//Handle to the Center Message timer. For Vote Warnings.
static Handle:center_message_timer = INVALID_HANDLE;
static bool:center_warning_active = false;

//Handle to the TF2 Game Message entity timer. For Vote Warnings.
static Handle:game_message_timer = INVALID_HANDLE;
static bool:game_message_active = false;

//Displays a message to the server
stock DisplayServerMessage(const String:msg[], const String:type[])
{
    //Kill timers for previous messages.
    if (game_message_active)
    {
        TriggerTimer(game_message_timer);
    }
    if (center_warning_active)
    {   
        center_warning_active = false;
        TriggerTimer(center_message_timer);
    }
    
    decl String:message[255];
    strcopy(message, sizeof(message), msg);
    
    //Display a chat message ("S") if...
    //    ...the user specifies.
    if (StrContains(type, "S") != -1)
    {
        new String:sColor[4];
        
        Format(message, sizeof(message), "%c%s", 1, message);
        
        for (new c = 0; c < sizeof(g_iSColors); c++)
        {
            if (StrContains(message, g_sSColors[c]))
            {
                Format(sColor, sizeof(sColor), "%c", g_iSColors[c]);
                ReplaceString(message, sizeof(message), g_sSColors[c], sColor);
            }
        }
        
        PrintToChatAll(message);
    }
    
    //Buffer to hold message in order to manipulate it.
    decl String:sTextTmp[255];
    
    //Display a top message ("T") if...
    //    ...the user specifies.
    if (StrContains(type, "T") != -1)
    {
        strcopy(sTextTmp, sizeof(sTextTmp), message);
        decl String:sColor[16];
        new iColor = -1, iPos = BreakString(sTextTmp, sColor, sizeof(sColor));
        
        for (new i = 0; i < sizeof(g_sTColors); i++)
        {
            if (StrEqual(sColor, g_sTColors[i]))
                iColor = i;
        }
        
        if (iColor == -1)
        {
            iPos   = 0;
            iColor = 0;
        }
        
        new Handle:hKv = CreateKeyValues("Stuff", "title", sTextTmp[iPos]);
        KvSetColor(hKv, "color", g_iTColors[iColor][0], g_iTColors[iColor][1], 
                   g_iTColors[iColor][2], 255);
        KvSetNum(hKv, "level", 1);
        KvSetNum(hKv, "time",  10);
        
        for (new i = 1; i <= MaxClients; i++)
        {
            if (IsClientInGame(i) && !IsFakeClient(i))
                CreateDialog(i, hKv, DialogType_Msg);
        }
        
        CloseHandle(hKv);
    }
    
    // Remove colors from advertisement, because
    // C,H,M methods do not support colors.
    
    //Remove a color from the message string for...
    //    ...each color in the Say color array.
    for (new c = 0; c < sizeof(g_iSColors); c++)
    {
        if (StrContains(message, g_sSColors[c]) != -1)
            ReplaceString(message, sizeof(message), g_sSColors[c], "");
    }
    
    //Remove a color from the message string for...
    //    ...each color in the Top color array.
    for (new c = 0; c < sizeof(g_iTColors); c++)
    {
        if (StrContains(message, g_sTColors[c]) != -1)
            ReplaceString(message, sizeof(message), g_sTColors[c], "");
    }
    
    //Display a center message ("C") if...
    //    ...the user specifies.
    if (StrContains(type, "C") != -1)
    {
        PrintCenterTextAll(message);
        
        //Setup timer to keep the center message visible.
        new Handle:hCenterAd;
        center_message_timer = CreateDataTimer(1.0, Timer_CenterAd, hCenterAd, 
                                               TIMER_FLAG_NO_MAPCHANGE|TIMER_REPEAT);
        WritePackString(hCenterAd, message);
        
        center_warning_active = true;
    }
    
    //Display a hint message ("H") if...
    //    ...the user specifies.
    if (StrContains(type, "H") != -1)
        PrintHintTextToAll(message);
        
    //Display a TF2 Game Event message ("G") if...
    //    ...the user specifies.
    if (StrContains(type, "G") != -1)
    {
        /*
        decl String:sIcon[64],  String:sBackground[6],  String:sTeam[6];
        
        KvGetString(g_hAdvertisements, "icon",  sIcon,  sizeof(sIcon),
                    "leaderboard_dominated");
        KvGetString(g_hAdvertisements, "background",  sBackground,

        sizeof(sBackground), "0");
        KvGetString(g_hAdvertisements, "team", sTeam, sizeof(sTeam), "0");
        new Float:fTime = KvGetFloat(g_hAdvertisements, "time");
        */
        
        TFGameText(message);
    }
}


//Called with each tick of the timer for center messages. Used to keep the message visible for an
//extended period.
public Action:Timer_CenterAd(Handle:timer, Handle:pack)
{
    decl String:sText[256];
    static iCount = 0;
    
    ResetPack(pack);
    ReadPackString(pack, sText, sizeof(sText));
    
    if (center_warning_active && ++iCount < 5)
    {
        PrintCenterTextAll(sText);
        return Plugin_Continue;
    }
    else
    {
        iCount = 0;
        center_message_timer = INVALID_HANDLE;
        center_warning_active = false;
        return Plugin_Stop;
    }
}


//Displays a game text message for TF2.
stock static TFGameText(const String:message[], Float:time=10.0, const String:icon[]="leaderboard_dominated",
           const String:background[]="0", const String:team[]="0")
{        
    if (!IsEntLimitReached(.message="unable to create game_text_tf"))
    {
        new Text_Ent = CreateEntityByName("game_text_tf");
        DispatchKeyValue(Text_Ent,"message",message);
        DispatchKeyValue(Text_Ent,"display_to_team",team);
        DispatchKeyValue(Text_Ent,"icon",icon);
        DispatchKeyValue(Text_Ent,"targetname","game_text1");
        DispatchKeyValue(Text_Ent,"background",background);
        DispatchSpawn(Text_Ent);

        AcceptEntityInput(Text_Ent, "Display", Text_Ent, Text_Ent);
        
        game_message_timer = CreateTimer(time, Kill_ent, 
                                         EntIndexToEntRef(Text_Ent));
        game_message_active = true;
    }
}


//Kills the game text message when the time limit is up.
public Action:Kill_ent(Handle:timer, any:ref)
{
    new ent = EntRefToEntIndex(ref);
    if (ent > 0 && IsValidEntity(ent))
    {
        decl String:classname[50];
        if (GetEdictClassname(ent, classname, sizeof(classname)) &&
            StrEqual(classname, "game_text_tf", false))
        {
            AcceptEntityInput(ent, "kill");
        }
    }
    game_message_active = false;
}


//Sets all elements of an array of booleans to false.
stock ResetArray(bool:arr[], size)
{
    for (new i = 0; i < size; i++)
        arr[i] = false;
}


//Utility function to cache a sound.
stock CacheSound(const String:sound[])
{
    //Handle the sound if...
    //  ...it is defined.
    if (strlen(sound) > 0)
    {
        //Filepath buffer
        decl String:filePath[PLATFORM_MAX_PATH];
    
        //Format sound to the correct directory.
        Format(filePath, sizeof(filePath), "sound/%s", sound);
        
        //Log an error and don't cache the sound if...
        //    ...the sound file does not exist
        if (!FileExists(filePath))
            LogError("SOUND ERROR: Sound file '%s' does not exist.", filePath);
        //Otherwise, cache the sound.
        else
        {
            //Make sure clients download the sound if they don't have it.
            AddFileToDownloadsTable(filePath);
            
            //Cache it.
            PrecacheSound(sound, true);
            
            //Log an error if...
            //    ...the sound failed to be cached.
            if (!IsSoundPrecached(filePath))
                LogError("SOUND ERROR: Failed to precache sound file '%s'", sound);
        }
    }
}


//Fetch the next index of the menu.
//    size: the size of the menu
//    scramble: whether or not a random index should be picked.
stock GetNextMenuIndex(size, bool:scramble)
{
    return scramble ? GetRandomInt(0, size) : size;
}


//Inserts given string into given array at given index.
stock InsertArrayString(Handle:arr, index, const String:value[])
{
    if (GetArraySize(arr) > index)
    {
        ShiftArrayUp(arr, index);
        SetArrayString(arr, index, value);
    }
    else
        PushArrayString(arr, value);
}


//Inserts given cell into given adt_array at given index,
stock InsertArrayCell(Handle:arr, index, any:cell)
{
    if (GetArraySize(arr) > index)
    {
        ShiftArrayUp(arr, index);
        SetArrayCell(arr, index, cell);
    }
    else
        PushArrayCell(arr, cell);
}


//Deletes values off the beginning of an array until it is down to the given size.
stock TrimArray(Handle:arr, size)
{
    //Remove elements from the start of an array while...
    //    ...the size of the array is greater than the required size.
    new asize = GetArraySize(arr);
    while (asize > size)
        RemoveFromArray(arr, --asize);
}


//Adds the given map to the given memory array.
//    mapName: the name of the map
//    arr:     the memory array we're adding to
//    size:    the maximum size of the memory array
stock AddToMemoryArray(const String:mapName[], Handle:arr, size)
{
    //Add the new map to the beginning of the array.
    InsertArrayString(arr, 0, mapName);
    
    //Trim the array down to size.
    TrimArray(arr, size);
}


//Adds entire array to the given menu.
stock AddArrayToMenu(Handle:menu, Handle:arr, Handle:dispArr=INVALID_HANDLE)
{
    decl String:map[MAP_LENGTH], String:disp[MAP_LENGTH];
    new arrSize = GetArraySize(arr);
    new dispSize = (dispArr != INVALID_HANDLE) ? GetArraySize(dispArr) : 0;
    for (new i = 0; i < arrSize; i++)
    {
        GetArrayString(arr, i, map, sizeof(map));
        if (i >= dispSize)
            disp = map;
        else
            GetArrayString(dispArr, i, disp, sizeof(disp));
        AddMenuItem(menu, map, disp);
    }
}


//Function to check the entity limit. Used before spawning an entity.
#tryinclude <entlimit>
#if !defined _entlimit_included
stock IsEntLimitReached(warn=20, critical=16, client=0, const String:message[]="")
{
    new max = GetMaxEntities();
    new count = GetEntityCount();
    new remaining = max - count;
    if (remaining <= warn)
    {
        if (count <= critical)
        {
            PrintToServer("Warning: Entity limit is nearly reached! Please switch or reload the map!");
            LogError("VOTEWARNINGS: Entity limit is nearly reached: %d/%d (%d):%s", 
                count, max, remaining, message);
    
            if (client > 0)
            {
                PrintToConsole(client, "Entity limit is nearly reached: %d/%d (%d):%s",
                       count, max, remaining, message);
            }
        }
        else
        {
            PrintToServer("Caution: Entity count is getting high!");
            LogMessage("VOTEWARNINGS: Entity count is getting high: %d/%d (%d):%s",
                count, max, remaining, message);
    
            if (client > 0)
            {
                PrintToConsole(client, "Entity count is getting high: %d/%d (%d):%s",
                       count, max, remaining, message);
            }
        }
        return count;
    }
    else
        return 0;
}
#endif


//Changes the map in 5 seconds.
stock ForceChangeInFive(const String:map[], const String:reason[]="")
{
    //Notify the server.
    PrintToChatAll("\x03[UMC]\x01 %t", "Map Change in 5");
    
    //Setup the change.
    ForceChangeMap(map, 5.0, reason);
}


//Changes the map after the specified time period.
stock ForceChangeMap(const String:map[], Float:time, const String:reason[]="")
{
    LogMessage("%s: Changing map to '%s' in %.f seconds.", reason, map, time);

    //Setup the timer.
    new Handle:pack;
    CreateDataTimer(
        time,
        Handle_MapChangeTimer,
        pack,
        TIMER_FLAG_NO_MAPCHANGE
    );
    WritePackString(pack, map);
    WritePackString(pack, reason);
}


//Called after the mapchange timer is completed.
public Action:Handle_MapChangeTimer(Handle:timer, Handle:pack)
{
    //Get map from the timer's pack.
    decl String:map[MAP_LENGTH], String:reason[255];
    ResetPack(pack);
    ReadPackString(pack, map, sizeof(map));
    ReadPackString(pack, reason, sizeof(reason));
    
    //Change the map.
    ForceChangeLevel(map, reason);
}


//Changes the map based off of the given action.
stock SetupMapChange(UMC_ChangeMapTime:action, const String:map[], const String:reason[]="")
{
    switch (action)
    {
        case ChangeMapTime_Now: //We change the map in 5 seconds.
            ForceChangeInFive(map, reason);
        case ChangeMapTime_RoundEnd: //We change the map at the end of the round.
        {
            LogMessage("%s: Map will change to '%s' at the end of the round.", reason, map);
            
            new Handle:cvarMaxRounds = FindConVar("mp_maxrounds");
            
            //Limit the remaining rounds to 1.
            SetConVarInt(cvarMaxRounds, 1);
            
            //Get the next map.
            decl String:nmap[MAP_LENGTH];
            GetNextMap(nmap, sizeof(nmap));
            
            //Set the map to be the next map played.
            if (!StrEqual(map, nmap))
                SetTheNextMap(map);
            
            //Print a message.
            PrintToChatAll("\x03[UMC]\x01 %t", "Map Change at Round End");
        }
        case ChangeMapTime_MapEnd: //We set the map as the next map.
        {
            //Get the currently set next map.
            decl String:curMap[MAP_LENGTH];
            GetNextMap(curMap, sizeof(curMap));
            
            //Set the voted map as the next map if...
            //    ...that map isn't already set as the next map.
            //if (!StrEqual(curMap, map))
            SetTheNextMap(map);
        }
    }
}


//Determines if the current server time is between the given min and max.
stock bool:IsTimeBetween(min, max)
{
    //Get the current server time.
    decl String:time[5];
    FormatTime(time, sizeof(time), "%H%M");
    new theTime = StringToInt(time);
    
    //Handle wrap-around case if...
    //  ...max time is less than min time.
    if (max <= min)
    {
        max += 2400;
        if (theTime <= min)
        {
            theTime += 2400;
        }
    }
    return min <= theTime && theTime <= max;
    
}


//Determines if the current server player count is between the given min and max.
stock bool:IsPlayerCountBetween(min, max)
{
    //Get the current number of players.
    new numplayers = GetRealClientCount();
    
    return min <= numplayers && numplayers <= max;
}


//Converts an adt_array to a standard array.
stock ConvertArray(Handle:arr, any:newArr[], size)
{
    new arraySize = GetArraySize(arr);
    for (new i = 0; i < size && i < arraySize; i++)
        newArr[i] = GetArrayCell(arr, i);
}


//Selects one random name from the given name array, using the weights in the supplies weight array.
//Stores the result in buffer.
stock bool:GetWeightedRandomSubKey(String:buffer[], size, Handle:weightArr, Handle:nameArr, &index=0)
{
    //Calc total number of maps we're choosing.
    new total = GetArraySize(weightArr);
    
    DebugMessage("Getting number of maps in the pool - %i", total);
    
    //Return an answer immediately if...
    //    ...there's only one map to choose from.
    if (total == 1)
    {   
        DebugMessage("Only 1 map in pool, setting it as the winner.");
        //WE HAVE A WINNER!
        GetArrayString(nameArr, 0, buffer, size);
        return true;
    }
    //Otherwise, we immediately do nothing and return, if...
    //    ...there are no maps to choose from.
    else if (total == 0)
    {
        DebugMessage("No maps in the pool. Returning false.");
        return false;
    }

    DebugMessage("Setting up array of weights.");
    //Convert the adt_array of weights to a normal array.
    new Float:weights[total];
    ConvertArray(weightArr, weights, total);

    DebugMessage("Picking a random number.");
    //We select a random number here by getting a random Float in the
    //range [0, 1), and then multiply it by the sum of the weights, to
    //make the effective range [0, totalweight).
    new Float:rand = GetURandomFloat() * ArraySum(weights, total);
    new Float:runningTotal = 0.0; //keeps track of total so far
    
    DebugMessage("Find the winner in the pool.");
    //Determine if a map is the winner for...
    //    ...each map in the arrays.
    for (new i = 0; i < total; i++)
    {
        DebugMessage("Update running total of weights.");
        //add weight onto the total
        runningTotal += weights[i];
        
        DebugMessage("Check if we're at the right map.");
        //We have found an answer if...
        //    ...the running total has reached the random number.
        if (runningTotal > rand)
        {
            DebugMessage("Map found.");
            GetArrayString(nameArr, i, buffer, size);
            index = i;
            return true;
        }
    }
    
    DebugMessage("ERROR WITH THE RANDOMIZATION ALGORITHM!");
    //This shouldn't ever happen, but alas the compiler complains.
    index = -1;
    return false;
}


//Utility function to sum up an array of floats.
stock Float:ArraySum(const Float:floats[], size)
{
    new Float:result = 0.0;
    for (new i = 0; i < size; i++)
        result += floats[i];
    return result;
}


//Utility function to set the next map.
stock SetTheNextMap(const String:mapName[])
{
    LogMessage("Setting nextmap to: %s", mapName);
    SetNextMap(mapName);
}


//Utility function to clear an array of Handles and close each Handle.
stock ClearHandleArray(Handle:arr)
{
    new arraySize = GetArraySize(arr);
    for (new i = 0; i < arraySize; i++)
        CloseHandle(GetArrayCell(arr, i));
    ClearArray(arr);
}


//Utility function to get the true count of active clients on the server.
stock GetRealClientCount(bool:inGameOnly=true)
{
    new clients = 0;
    for (new i = 1; i <= MaxClients; i++)
    {
        if ((inGameOnly ? IsClientInGame(i) : IsClientConnected(i)) && !IsFakeClient(i))
            clients++;
    }
    return clients;
}


//Utiliy function to append arrays
stock ArrayAppend(Handle:arr1, Handle:arr2)
{
    new arraySize = GetArraySize(arr2);
    for (new i = 0; i < arraySize; i++)
        PushArrayCell(arr1, GetArrayCell(arr2, i));
}


//Builds an adt_array of numbers from 0 to max-1.
stock Handle:BuildNumArray(max)
{
    new size = 2 + max / 10;
    new Handle:result = CreateArray(ByteCountToCells(size));
    decl String:buffer[size];
    for (new i = 0; i < max; i++)
    {
        //IntToString(i, buffer, size);
        Format(buffer, size, "%i", i);
        PushArrayString(result, buffer);
    }
    return result;
}


//Determines the correct time to paginate a menu. Menu passed to this argument should have
//pagination enabled.
stock SetCorrectMenuPagination(Handle:menu, numSlots)
{
    if (GetMenuStyleHandle(MenuStyle_Valve) != GetMenuStyleHandle(MenuStyle_Radio) && numSlots < 10)
        SetMenuPagination(menu, MENU_NO_PAGINATION);
}


//Finds a string in an array starting at a specific index.
stock FindStringInArrayEx(Handle:arr, const String:value[], start=0)
{
    new size = GetArraySize(arr);
    decl String:buffer[255];
    for (new i = start; i < size; i++)
    {
        GetArrayString(arr, i, buffer, sizeof(buffer));
        if (StrEqual(value, buffer))
            return i;
    }
    return -1;
}


//Closes a handle and sets the variable pointer to INVALID_HANDLE
stock CloseHandleEx(&Handle:handle)
{
    CloseHandle(handle);
    handle = INVALID_HANDLE;
}


//Prints a debug message if the DEBUG flag is on.
stock DebugMessage(const String:message[], any:...)
{
#if UMC_DEBUG
    
    new size = strlen(message) + 255;
    decl String:fMessage[size];
    VFormat(fMessage, size, message, 2);
    
    LogMessage("DEBUG: %s", fMessage);
    
#endif
}


//Creates a copy of an adt_array.
stock Handle:CopyStringArray(Handle:arr, blocksize=1)
{
    new size = GetArraySize(arr);
    new Handle:result = CreateArray(blocksize);
    new len = 4 * blocksize;
    decl String:buffer[len];
    for (new i = 0; i < size; i++)
    {
        GetArrayString(arr, i, buffer, len);
        PushArrayString(result, buffer);
    }
    return result;
}


//Makes the timer to retry running a vote every second.
stock MakeRetryVoteTimer(Function:callback)
{
    CreateTimer(1.0, Handle_RetryVoteTimer, callback, TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
}


//Handles the retry timer for votes that were attempted to be started.
public Action:Handle_RetryVoteTimer(Handle:timer, any:data)
{
    if (IsVoteInProgress())
        return Plugin_Continue;

    new Function:callback = Function:data;
    
    Call_StartFunction(INVALID_HANDLE, callback);
    Call_Finish();
    
    return Plugin_Stop;
}


//Comparison function for map tries <Map, MapGroup>. Used for sorting.
public CompareMapTries(index1, index2, Handle:array, Handle:hndl)
{
    decl String:map1[MAP_LENGTH], String:map2[MAP_LENGTH],
         String:group1[MAP_LENGTH], String:group2[MAP_LENGTH];
    new Handle:map = INVALID_HANDLE;
    map = GetArrayCell(array, index1);
    GetTrieString(map, MAP_TRIE_MAP_KEY, map1, sizeof(map1));
    GetTrieString(map, MAP_TRIE_GROUP_KEY, group1, sizeof(group1));
    map = GetArrayCell(array, index2);
    GetTrieString(map, MAP_TRIE_MAP_KEY, map2, sizeof(map2));
    GetTrieString(map, MAP_TRIE_GROUP_KEY, group2, sizeof(group2));
    
    new result = strcmp(map1, map2);
    if (result == 0)
        result = strcmp(group1, group2);
    return result;
}


//Sorts an array of map tries
stock SortMapTrieArray(Handle:array)
{
    SortADTArrayCustom(array, CompareMapTries);
}


//Prints the sections of a kv to the log.
stock PrintKv(Handle:kv, depth=0)
{
    decl String:section[256];
    KvGetSectionName(kv, section, sizeof(section));
    LogMessage("%i: \"%s\"", depth, section);
            
    if (!KvGotoFirstSubKey(kv))
        return;
    
    do
    {
        PrintKv(kv, depth + 1);
    } while (KvGotoNextKey(kv));
    
    KvGoBack(kv);
}


