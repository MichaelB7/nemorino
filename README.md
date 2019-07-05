# README #

Nemorino is a UCI chess engine.

### Features ###

* Syzygy Tablebase support
* Pondering
* Standard Chess and Chess960
* Multi Core support (Shared Hash Table)
* Multi PV analysis mode
* CECP support (experimental - all testing is done using UCI)
##### UCI parameters: #####
- **UCI_Chess960:**     If set to true engine will play according to Chess960 rules (default false)
- **Hash:**             Size in MBytes used for the transposition table (default is 32. Nemorino will use additional memory for other fixed-size hash tables)
- **Clear Hash:**       Will clear the transposition table
- **MultiPV:**          Number of principal variations shown (default 1)
- **Threads:**          Maximum number of threads (cores) used (default determined by number of available hardware threads)
- **Ponder:**           When set to true, engine will continue analysis while waiting for opponent's move (default false)
- **Contempt:**         Score (in centipawns) for accepting draws. If Contempt > 0 then the engine will avoid draws (default 0)
- **BookFile:**         Polyglot book file the engine shall use (default "book.bin"). Nemorino doesn't have an own book so far
- **OwnBook:**          Use Polyglot book, specified by book file (default false)
- **SyzygyPath:**       Path(es) to directories Syzygy tablebase files can be found. Seperate multiple directories by ';' on windows and by ':' on linux
- **SyzygyProbeDepth:** Minimum search depth for probing Syzygy Bases. The lower the number, more tablebase lookups will be done, but search speed will decrease (default 8)
- **MoveOverhead:**     Safety Time(in ms) needed to avoid time-losses (default 100)
- **UCI_Opponent:**     With this command the GUI can send the name, title, elo and if the engine is playing a human or computer to the engine (see [UCI Specification](http://wbec-ridderkerk.nl/html/UCIProtocol.html))


### Installation ###
Released executables can be downloaded in the **[Download section](https://bitbucket.org/christian_g_nther/nemorino/downloads)**

The current development versions are also available:

* [Main Nemorino dev version](https://s3.eu-central-1.amazonaws.com/nemorinotest/appveyor/nemorino_Release.zip)
* [Nemorino dev version for old PC without POPCNT support](https://s3.eu-central-1.amazonaws.com/nemorinotest/appveyor/nemorino_ReleaseNoPopcount.zip)

Nemorino doesn't bring it's own UI. So for running it, you need a GUI supporting UCI (like [Arena](http://www.playwitharena.com/)).  
The engine requires 64-bit Windows.

### Internals ###

* Bitboard based (using magic bitboards for sliding move generation)
* Principal Variation Search (with Aspiration Windows, Null-Move Pruning, Killermoves, Futility Pruning, Razoring, ...)
* Tapered Eval based on Material, Mobility, Threats, King Safety, and Pawn Structure
* Special evaluation functions for some endgames
* Copy/Make

### Version History ###

**1.01:**

*          Bugfix: rare crashes caused by hash collisions in transposition table
*          Code changes for non-popcount compiles

**1.02:**

*          New Parameters: SyzygyProbeDepth and MoveOverhead
*          Bugfix: Handling of EP-Squares in TB probing code

**2.00:**

*          Several Improvements in Search: Singular Extensions, Cut Node Extensions, improved IID,..
*          Improved SEE (consider promotions and pinned pieces)
*          Improved King Safety Evaluation
*          Scale down endgame values if there are opposite colored bishops
*          Several minor improvements and clean-ups

**2.00a (only bugfixes relevant when running with more than one thread):**

*          Fix for known issue with tablebase access (see https://github.com/basil00/Fathom/issues/15
*          Fix for other (rare) crashes

**3.00:**

*          Refactoring of SMP implementation => Improved stability
*          Several minor improvements in search and move ordering

**4.00:**

*          Improved King Safety evaluation
*          Some Improvements when running with many threads
*          Some Evaluation Improvements (added some material imbalance terms, knight center mobility, ... and removed some useless terms)

**5.00:**

*          Several minor Improvements especially for King Safety 
*          Several Bugfixes
*          Replaced Syzygy probing code. Now instead of Fathom library, probing code from Stockfish is used

### License ###

[GNU GENERAL PUBLIC LICENSE Version 3](https://www.gnu.org/licenses/gpl-3.0.en.html)

### Strength ###

Nemorino is listed in several rating lists:

Nemorino 5.00

* [CCRL 40/40 (4 CPU) **Elo 3161 #17**](http://www.computerchess.org.uk/ccrl/4040/cgi/engine_details.cgi?print=Details&each_game=1&eng=Nemorino%205.00%2064-bit%204CPU#Nemorino_5_00_64-bit_4CPU)
* [CCRL 40/40 **Elo 3084 #21**](http://www.computerchess.org.uk/ccrl/4040/cgi/engine_details.cgi?match_length=30&each_game=1&print=Details&each_game=1&eng=Nemorino%205.00%2064-bit#Nemorino_5_00_64-bit)
* [CCRL 40/4 (4 CPU) **Elo 3178 #25**](http://www.computerchess.org.uk/ccrl/404/cgi/engine_details.cgi?match_length=30&each_game=1&print=Details&each_game=1&eng=Nemorino%204.00%2064-bit%204CPU#Nemorino_4_00_64-bit_4CPU)
* [CCRL 40/4 **Elo 3123 #21**](http://www.computerchess.org.uk/ccrl/404/cgi/engine_details.cgi?print=Details&each_game=1&eng=Nemorino%205.00%2064-bit#Nemorino_5_00_64-bit)
* [CCRL 40/4 (FRC) **Elo 3180 #9**](http://www.computerchess.org.uk/ccrl/404FRC/cgi/engine_details.cgi?print=Details&each_game=1&eng=Nemorino%205.0%2064-bit#Nemorino_5_0_64-bit)
* [FastGM Bullet **Elo 2921 #24**](http://www.fastgm.de/60-0.60.html)
* [FastGM 10min **Elo 2971 #18**](http://www.fastgm.de/10min.html)
* [CEGT 40/4 **Elo 2974 #23**](http://www.cegt.net/40_4_Ratinglist/40_4_single/rangliste.html)
* [CEGT 40/20 **Elo 2992 #21**](http://www.cegt.net/40_40%20Rating%20List/40_40%20SingleVersion/rangliste.html)
* [Owl Chess **Elo 3097 #21**](http://chessowl.blogspot.de/p/rating-list_27.html)

Nemorino 4.00

* [CEGT 40/4 (2 CPU) **Elo 2984 #24**](http://www.cegt.net/40_4_Ratinglist/40_4_mp/rangliste.html)
* [CEGT 5+3 PB=ON **Elo 2903 #27**](http://www.cegt.net/5Plus3Rating/Purelist/rangliste.html)


### Remarks ###

I wrote this engine because I wanted to understand, how a chess engine is working. And the best way to learn is to write an engine from scratch by yourself. I first started with C#, but after some time I got the ambition to learn C++.
Therefore don't expect a lot of new ideas within my code, and neither expect clean and well-structured code. Instead you will find a unique combination of all those ideas explained in the [Chess Programming Wiki](https://chessprogramming.wikispaces.com).

### Acknowledgements ###

These are my sources of information I used for my engine:

* My main source of information: [Chess Programming Wiki](https://chessprogramming.wikispaces.com)
* [Computer Chess Club Forums](http://talkchess.com/forum/index.php)
* The code of several Open Source Chess Engines, mainly:
    * [Stockfish](http://stockfishchess.org/) - I copied board and move representation and some lines of code from Stockfish (marked by source code comments)
    * [Hakkapeliitta](https://github.com/mAarnos/Hakkapeliitta) 
    * [Lozza](http://op12no2.me/toys/lozza/)
    * [Senpai](http://www.chessprogramming.net/senpai/)
* The blogs of [Steve Maughan](http://www.chessprogramming.net/), [Thomas Petzke](http://macechess.blogspot.de/) and [Jonatan Pettersson](http://mediocrechess.blogspot.de/)