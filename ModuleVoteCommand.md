# Purpose #
> The Vote Command module defines a SourceMod admin command, _"sm\_umc\_mapvote"_, that will start a map vote on the server.

> _**sm\_umc\_mapvote** `<0|1|2>`_ -- Starts a UMC Map Vote. The one argument specifies when the map will be changed to the winner of the vote.
    * 0 = Change Immediately
    * 1 = Change at the end of the current round
    * 2 = Change at the end of the map

# List of CVars #
> The cvar list for the Vote Command module is located in **cfg/sourcemod/umc-votecommand.cfg**.
```
// Allows a map to appear in the vote more than once. This should be enabled if you want the same map in different categories to be distinct.
// -
// Default: "1"
// Minimum: "0.000000"
// Maximum: "0.500000"
sm_umc_vc_allowduplicates "1"

// Specifies how many slots in a vote are disabled to prevent accidental voting.
// -
// Default: "0"
// Minimum: "0.000000"
// Maximum: "5.000000"
sm_umc_vc_blockslots "0"

// File to use for Ultimate Mapchooser's map rotation.
// -
// Default: "umc_mapcycle.txt"
sm_umc_vc_cyclefile "umc_mapcycle.txt"

// Adds a "Don't Change" option to votes.
// -
// Default: "1"
// Minimum: "0.000000"
// Maximum: "1.000000"
sm_umc_vc_dontchange "1"

// Specifies how long a vote should be available for.
// -
// Default: "20"
// Minimum: "10.000000"
sm_umc_vc_duration "20"

// Sound file (relative to sound folder) to play at the completion of a vote.
// -
// Default: ""
sm_umc_vc_endsound ""

// Adds an "Extend" option to votes.
// -
// Default: "0"
// Minimum: "0.000000"
// Maximum: "1.000000"
sm_umc_vc_extend "0"

// Specifies how many more frags each extension adds to the frag limit.
// -
// Default: "10"
// Minimum: "1.000000"
sm_umc_vc_extend_fragstep "10"

// Specifies how many more rounds each extension adds to the round limit.
// -
// Default: "5"
// Minimum: "1.000000"
sm_umc_vc_extend_roundstep "5"

// Specifies how many more minutes each extension adds to the time limit.
// -
// Default: "15"
// Minimum: "1.000000"
sm_umc_vc_extend_timestep "15"

// Specifies what action to take if the vote doesn't reach the set theshold.
//  0 - Do Nothing,
//  1 - Perform Runoff Vote
// -
// Default: "0"
// Minimum: "0.000000"
// Maximum: "1.000000"
sm_umc_vc_failaction "0"

// Specifies how many past map groups to exclude from votes.
// -
// Default: "0"
// Minimum: "0.000000"
sm_umc_vc_groupexclude "0"

// Specifies how many past maps to exclude from votes.
// -
// Default: "3"
// Minimum: "0.000000"
sm_umc_vc_mapexclude "3"

// Specifies whether vote menu items are displayed in a random order.
// -
// Default: "0"
// Minimum: "0.000000"
// Maximum: "1.000000"
sm_umc_vc_menuscrambled "0"

// Specifies whether the number of nominated maps appearing in the vote for a map group should be limited by the group's "maps_invote" setting.
// -
// Default: "0"
// Minimum: "0.000000"
// Maximum: "1.000000"
sm_umc_vc_nominate_strict "0"

// Specifies what action to take if the runoff vote reaches the maximum amount of runoffs and the set threshold has not been reached.
//  0 - Do Nothing,
//  1 - Change Map to Winner
// -
// Default: "0"
// Minimum: "0.000000"
// Maximum: "1.000000"
sm_umc_vc_runoff_failaction "0"

// Specifies the maximum number of maps to appear in a runoff vote.
//  1 or 0 sets no maximum.
// -
// Default: "0"
// Minimum: "0.000000"
sm_umc_vc_runoff_max "0"

// If specified, this sound file (relative to sound folder) will be played at the beginning of a runoff vote. If not specified, it will use the normal vote start sound.
// -
// Default: ""
sm_umc_vc_runoff_sound ""

// Specifies a maximum number of runoff votes to run for a vote.
//  0 = unlimited.
// -
// Default: "0"
// Minimum: "0.000000"
sm_umc_vc_runoffs "0"

// Sound file (relative to sound folder) to play at the start of a vote.
// -
// Default: ""
sm_umc_vc_startsound ""

// If the winning option has less than this percentage of total votes, a vote will fail and the action specified in "sm_umc_vc_failaction" cvar will be performed.
// -
// Default: ".50"
// Minimum: "0.000000"
// Maximum: "1.000000"
sm_umc_vc_threshold ".50"

// Controls vote type:
//  0 - Maps,
//  1 - Groups,
//  2 - Tiered Vote (vote for a group, then vote for a map from the group).
// -
// Default: "0"
// Minimum: "0.000000"
// Maximum: "2.000000"
sm_umc_vc_type "0"



```