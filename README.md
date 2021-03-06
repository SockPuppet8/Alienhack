## FAO Windows 10 users

There is a problem whereby the application window doesn't resize itself correctly at startup. 

I don't have a Windows 10 system to work on so this is unlikely to be fixed very soon. 

You can work around it easily by right-clicking in the top left of the window, selecting "Properties", and then manually setting the window to have a dimension of 80x40 characters. Once you've done that, the game should be playable. While you're there you might want to alter the console font as well to suit your tastes.

---

ALIENHACK by Sock Puppet
========================

This is the source code for the Alienhack game. Alienhack is a "roguelike" game loosely based on "AliensRL". More information can be found in "ah-readme.txt", which can also be viewed in-game.

If you're here because you want to play it, just download the binary (go to the Releases page at https://github.com/SockPuppet8/Alienhack/releases, the file you want is "Alienhack.0.9.1.beta.zip"), unzip it, and run "Alienhack.exe". At the moment, Windows is the only supported platform.

Note that the game currently does not have any sound effects. So if you can't hear anything, it's not a bug. If the game can't make you jumpy by gameplay alone then it has failed.

## Building the game

See "build.txt" for instructions on how to build the game.

Some of the source code - all the code which is not specific to Alienhack, and could be re-used in a different game - is in a separate repository called "RL-Shared". See "build.txt" for more details. 

Instructions for the game itself are in "ah_readme.txt". Controls are configured in "keys.txt". Both of these files are intended to be shipped in the same folder as the executable, so if you make any changes to them, don't forget to copy them to the executable folder. 

Currently, Windows is the only supported platform. Only the following files contain Windows-specific code: 
* Include/assert.hpp in the "RL-Shared" repository
* Console/Console.cpp in the "RL-Shared" repository

## License

See "license.txt".
