# spotify-ad-mute
Console application that mutes Spotify during an ad and unmutes as soon as the ad is finished.

It only works on Windows with the Spotify desktop app (not Spotify web).

How it currently works:
The program scans the active processes to find the Spotify process, gets the handle to it and checks every 2 seconds for the title of the associated window(s). Since the main Spotify window is either called the title of the song that is playing or something else when an advertisement is playing, from that it can be determined if an advertisement is playing. The interval can be changed in the options, though options are not yet saved to any file.

The process is then muted/unmuted using the Win32 API COM interface for audio.

It can rarely happen that the program doesn't detect an ad, because the window is called something different from the standard titles. I plan to add a functionality that allows "blocking" of user-specified window titles.

