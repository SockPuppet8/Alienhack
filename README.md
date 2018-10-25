## FAO Windows 10 users

There is a problem whereby the application window doesn't resize itself correctly at startup. 

I don't have a Windows 10 system to work on so this is unlikely to be fixed very soon. 

You can work around it easily by right-clicking in the top left of the window, selecting "Properties", and then manually setting the window to have a dimension of 80x40 characters. Once you've done that, the game should be playable. While you're there you might want to alter the console font as well to suit your tastes.

---

ALIENHACK by Sock Puppet
========================

This is the source code for the Alienhack game. If you're here because you want to play it, just download the binary (assuming that I have uploaded one yet!) and run "Alienhack.exe". At the moment, Windows is the only supported platform.

See "build.txt" for instructions on how to build the game.

Some of the source code - all the code which is not specific to Alienhack, and could be re-used in a different game - is in a separate repository called "Shared". See "build.txt" for more details. 

Instructions for the game itself are in "ah_readme.txt". Controls are configured in "keys.txt". Both of these files are intended to be shipped in the same folder as the executable, so if you make any changes to them, don't forget to copy them to the executable folder. 

Currently, Windows is the only supported platform. Only the following files contain Windows-specific code: 
* Shared/Include/assert.hpp in the "Shared" repository
* Shared/Console/Console.cpp in the "Shared" repository

## License

See "license.txt".
