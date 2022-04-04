// corePCH.hpp - Precompiled core engine headers

#ifndef __CORE_PCH__
#define __CORE_PCH__

// Interface Headers
#include "INTFcommon.hpp"
#include "INTFcvarSystem.hpp"
#include "INTFfileSystem.hpp"
#include "INTFsys.hpp"

// libBud
#include "libBudPCH.hpp"

// You need to include the filesystem right after including libBud, 
// precompiled headers don't get preprocessed in the same way cpp files do

// Core Framework
#include "coreBuildVer.hpp"
#include "coreLicense.hpp"
#include "coreCommon.hpp"
#include "coreUnzip.hpp"
#include "coreFile.hpp"
#include "coreFileManifest.hpp"
#include "coreFileSave.hpp"
#include "coreFileResource.hpp"
#include "coreFileSystem.hpp"
#include "coreUserCmdGen.hpp"
#include "coreSerializer.hpp"
#include "coreProfilePlayer.hpp"

// Session & Networking
#include "coreServerCompression.hpp"
#include "coreSnapshot.hpp"
#include "coreServerPacketProcessor.hpp"
#include "coreSnapshotProcessor.hpp"

#include "coreSave.hpp"
#include "coreSessionSave.hpp"
#include "coreProfile.hpp"
#include "coreLocalUser.hpp"
#include "coreSignin.hpp"
#include "coreMiscStats.hpp"
#include "coreStats.hpp"
#include "coreSession.hpp"

// More no preprocessor bullshit
// This is part of the core framework btw
#include "coreCompressor.hpp"
#include "coreEventLoop.hpp"
#include "coreInputEvent.hpp"
#include "coreEditField.hpp"
#include "coreConsole.hpp"
#include "coreFileDemo.hpp"


#undef min
#undef max
#include <algorithm>	// for min / max / swap

#endif /* !__CORE_PCH__ */

//                                  ...........................'........''......''''''........'''''''........''''''......''''''.''.'..''''..'...'''''''..'..''''''''''''''''''''''''''''''.............    
//       ;llc. .cll,   .coxxdl;.  .....;oooooolc,.':dddddooc;'''cddddddl,'cdddddol:,.'cdddddooc,'''';oxxkkxdc,.''':oxkkxdl,'',lddddddc'',cdxkkxo:''''coxkkxdc,''',cdxkkxo:''':ddddddol;'''.'codxxoc'.....  
//      .kMMMx.;XMMx. cXWMNXWMWk,......dMMMMWWMW0:'xMMMMWWMWKl',kMMMMMMK:,OMMMWWWMNx,,OMMMWWMMWO:.,oXWMMNXWMNk;.,dXMMNXWMW0c.:0MMMMMMO,;OWMWXNMMNd',dNMMNXWMWk;.;kNMWXNMMNd,'dWMMMWWMWXo,';kNMN0XMW0:...   
//      .kMMMN:;KMMx.'0MMMd:KMMWd......dWMMXdxWMWx'xMMMXdkMMMO,,kMMMXdlc,,OMMM0o0MMXc,OMMM0o0MMMx.:KMMWWdcKMMWd.cXMMWd:0MMMk';0MMM0ol:'lNMMXcoWWWO,;KMMWocKWWKc.dNMMXldWMMXc'dWMMNdkWMM0;'dWMMO;dMMMO'..   
//      .kMMMMkc0MMx.,KMMMo,0MMMx......dWMMX:lNMMx,xMMMKlxMMMO,,kMMM0:',.,OMMMk;xMMNl,OMMMOcOMMMx.cXMMWWo:KMMMx'lXMMWo,OMMMO,;0MMMk,',.cXMMWkolll:';0MMW0olllc,'xWMMXcoWMMNo'dWMMNodWMM0:'cdddc'dMMM0, . ..
//      .kMMWWNOKMMx.;XMMMo,0MMMk......dWMMNO0WMWd,xMMMWXNWXOc',kMMMWK0d,,OMMMXOXMMXc,OMMMNXWWKx:.lXMMWWo:KMMMk'lNMMWo'cdddc';0MMMNK0o',oKWMMWKxc,''c0WMMWXkl,''xWMMXcoWMMNo,dWMMWXNMNOl;,,:::,'xMMM0, ....
//      .kMMK0WWWMMx.;XMMMo,0MMMk......dWMMWNXK0x;,kMMMNOKWNOl',kMMMWK0d,,OMMMWNXK0o,;OMMMXOXWXkc.lXMMWWo:KMMMk'lNMMWo',;;;,':0MMMNK0o''';lkXWMMNx,'',cxKWMMWO:'xWMMXcoWMMNo,dWMMW0KWN0o;'lXNNX0XMMMO'.....
//      .kMMOl0MMMMx.;KMMMo,0MMMx......dWMMXo;:;,,,xMMMKcdWMMO,,kMMM0:,,';OMMM0c;;,,';OMMMk;kMMMx'cXMMWWo:KMMMx'lXMMWo,xXXXx,;0MMMk,,,':xkkdcxWMMXc,okkxcoXMMWd'xWMMXcoWMMNo,xWMMXloWMM0:'oNMMX0KK0x;......
//      .kMM0;oWMMMx.,KMMMd:0MMMd......dWMMXl,;,,,,kMMMKcdWMMO;,kMMMKocc,;OMMMO;',,,';OMMMk;kMMMk':KMMWWdcKMMWx.cKMMWd:0MMMk,:0MMM0cc:'cXMMXccXMMNl,OMMWd;0MMWx'dWMMXldWMMXl,xWMMXloNMMKc':xkko,,,'........
//      .kMMK,,KMMMx..oNMMNXWMW0;......dWMMXl,,,,,;kMMMKcdWMM0;,OMMMWWNXl,OMMMO;',,,,;OMMMO;kMMMk,,dNMMMNXWMWO:',xNMMXKWMW0c':0MMMWNNK:,xNMWXXWMWO;'oXMMXKNMWKl':OWMWXXMMNx;,xWMMNloWMM0c'lKNNk;'''''......
//      .:xxo. :xxx:. .,lxkOkdc'.......:xkkd:,,;;;;lkkkd;ckkko;;okkkkkkx:,okkko,,,,,,,okkko,lkkkl,,,cdkkOOkxl;,,,,cdkOOkxl;,,;okkkkkkd;',cdkOOkxl;'',:oxOOkxo:',';lxkOOkdl;;;lkOOx:ckkkd;':xkko,,,'''''....
//                   ....................',,,,;;;;;,,,,,,,;;;;;;;,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,',,,,,,,'''''',,,,''',''','',''''''''',,,,,',,,,;;;;;;;;;;;;;;,,,,,,,,,,,,'''''....
//                 .....................',,,,;;;;;;,,,;;;::::::;;;;;,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,',,,,,,,,,,,,,,,,,,,;;:::::;;::::;;;;;;;,,,,,,,,''''''...
//               .......................',,,;;;;;;;,,;;::::::::;;;;;;;,,,,,,,,,,,,,,,,,,,,;;;;,,,;;;,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,;;:::::::::::;;;;;;;;;;;;;;,''''''...
//             .........................',,;;;;;;;;;;:::::::::;;;;;;;;;;;;;,,,,;;;;;;;;;,,,;;;;;;;;;;;;;;;;;;,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,;;;;:::::::::;;;;;;;;;;;;;;;,'''''''..
//           ...........................,,;;;;;;;;;;::cccc::::;;;;;;;;;;;;;;;;;;;;;;;;;;;;,;;;;;;;;;;;;;;;;;;;;;;;;;;;;;,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,;;;;;;;;:::::;;;;;;;;;;;;;;;;;;;,'''''''..
//           ..........................',;;;;;;;;;::cccccc::::;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,;;;;::;;;;::::;;;;;;;;;;;;;;;;;;,''''''''..
//                  ...................',;;;;;;;::cccccc::::;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,;;;::::::;;::::;;;;;;;;;;;;;;;;;,''''''''..
//                  ...................';;;;;;::cccccccc::;;;;;,,;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;,,,,,,,,,,,,,,,,,,,,,,,,,,;;;;::cc:::;;:::;;;;;;;;;;;;;;::;,'''''''''.
//                   ..... ...       ..,;;;;;::cccccccc::;;;;;,,,;;;;;;;;::;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;,,,,,,,,,,,,,,,,,,,,,;;;;;;;::::::;;:::;;;;;;;;;;;;;:::;''''''''''.
//                    ....           ..,;;;::ccclllcc:::;;;;;;,,,;;;;;;;;:::::::::;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;,,,,,,,,,,,,,,,,,,;;;;;;;;;::::::::::;;;;;;;;;;;:::::,'''''''''..
//                      ..           .';;;::cccllccc::;;;;;;;,,,,,;;;;;;;::::::::::;;;;:::::;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;,,,,,,,,,,,;;;;;;;;;;::::::::::;;;;;;;;;::::::;''''''''''..
//     ...                    ...   ..,;;:cccllccc::;;;;;;;;;,,,,,,;;;;;;:::::::::::::::::::::;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;,,,,,,,;;;;;;;:::::::::::::::;;;;;;;:;::::::,''''''''''..
//   .....                ............,;:ccccccc::;;;;;;;;;;;,,,,,,,;;;;;::::::::::::::::::::::::::;;;;;;:::::::::::::::::;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;::::::::::::::::;;;;;;:::::::c;''''''''''...
//  .......               ...........';:cccc::::;;;;;;;;;;;;,,,,,,,,,;;;;;:::::::::::::::::::::::::::::::::::::::::::::::::::::::;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;::::::::::::::::;;;:::::::::c:,'.''''''.....
// .........              ..........';::::::::;;;;;;;;;;;;;;,,,,,,,,,;;;;;;::::::::::::::::::::::::::::::::::::::::::::cccccc:::::::::;;;;;;;;;;;;;;;;;;;;;;;;;:::::::::::::::::;;::::::::cc:'.............
// ..........             ..........;::::;;;;;;;;;;;;;;;;;;,,,,,,,,,,,,;;;;:::::::::::::::::::::::::::;;;;;;:::::::::::cccccccccccc:::::::;;;;;;;;;;;;;;;;;;;:::::::c:::c::::::::::::::::cc:,..............
// ...........           ..........,;;;;;;;;;;;;;;;;;;;;;,,,,,,,,,,,,,,,;;;;::::::::::::::::::::::::::;;;;;;;;;;;::::::::cccccccccccccc:::::::;;;;;;;;;;::::::::::::::::::::::::::::::::ccc;'..............
// ............        ...........,;;;;;;;;;;;;;;;;;;;;,;,,,,,'''.'',,,,,;;,;:::::::::::::::::::::::;;;;;;;;;;;;;;;:::::::cccccccccccccccc::::::::::::::::::::::::ccc:::::::::::::::::::cc:'...............
// .............    ..............;;;;;;;;;;;;;;;;;;,,,,,,'''.......',,,,;;,,,::::::::::::::::::;;;;;;;;;;;;;;;;;;;;::::::::cccccccccccccccc::::::::::::::::::::ccccccc:::::::::::::::cccc,................
// ..............................';;;;;;;;;;;;;;,,,,,''.............',,,,;;;'.';:::::::::::::::;;;;;;;;;;;;;;;;;;;;;;;:::::::::ccccccccccccccc::::::::::c:::::cccccccccc::::::::::::::ccc;.................
// ..............................',,,,,,,,,,,,'''''................'',,,,,;;,...,;:::::::::::::;;;;;;;;;;;;;;;;;;;;;;;;;;::::::::cccccccccccccc:::::ccccccccccccccccccc:::::::::::::cccc;'.................
// ..................................................        .....',,,,,,,;;;'....,;;;::::::;::;;;;;;;;;;;;;;;;;;;;;;::;;:::::::::::cccccccccccccccccccccccccccccccccc::::::::::::::ccc:'..................
// ...............................................         .....'',,,;,,,,;;;,'.....',;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;:::::::::::::ccccccccccccccccccccccccccccccc:::::::::::::ccc:'...................
// ..............................   ....                ......'',,,,;;;;,,,;;;,'.......',;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;:::;:;;;;;:::::::ccccccccccccccccccccccccccc:::::::::::::ccc:'................... 
// ......................   ....                    .......'',,,,,,;;;;;,,,;;;;,,'........'',,,;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;::::::ccccccccccccllcccccccccc::::::;;;::::ccc:'.................   .
// .......................   ....            ..........''',,,,,,;;;;;;;;;,,,;;;;;;;,... .......'''',,,,,,,,,,,,,,,,,,,,,,,,;;;;;;;;;;;;;::::ccccccccclllllllccccccc:::;;;;;;;:::ccc;'...............   ....
// ........................  .....       .......''''',,,,,,,,,;;;;;;;;;;;;,,;;;;;;;;;,...  ................................''',,,;;;;;;;;;:::cccccccllllllllccccc::::;;;;;;::::ccc;'.............    ......
// .................................    ...''',,,,,,,,,,,,,,,;;;;;;;;;;;;;;,;;;,,,;;;;;,...   ...........           ............'',,,;;;;;;:::cccllllllllllllccc::::;;;;;;;::::cc,.............   .........
// ..................................   ..',,,,,,,,,,,,,,,,,,,,,,,;;;;;;;;;;;;;;,,,,;;;;,,'...                        .............'',,,;;;;:::cclllllllllllccc:::;;;;;;;;:::cc:,...........    ...........
// ........................................',,,,,,,,,,,,,,,,,,,,,,,,;;;;;;;;;;;;;,,,,,;;;;;,,'....                          .........',,,;;;;::cccllllllllllcc:::;;;;;;;;;::c:;'..........   ..............
// .........................................',;;,,,,,,,,,,,,,,,,,,,,;;;;;;;;;;;;;,,,,,,;;;;;;;,,''.......                      ........',,;;;::ccclllllllllcc:::;;;;;;;;;::::,.........    ................
// ...........................................',;,,,,,,,,,,,,,,,,,,,,,,,,;;;;;;;;;,,,,,,,;;;;;;;,,,,,''''......                 ........',,;;:::cclllllllllcc::;;;;;;;;;:::;'........    ..................
// .............................................',,,,,,,,,,,,,,,,,,,,,,,,,,;;;;;;;,,,,,,,,,,;;;,,,,,,,,,,,,,'''.......           ........',,;;::ccllllllllcc::;;;,,;;;;::;'........    ....................
// ................................................',,,,,,,,,,,,,,,,,,,,,,,,;;;;;;,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,'''...           .....',,;::cclllllllcc::;;,,,;;;;:;,.......     ......................
// ..................................................',,,,,,''''''''''',,,,,;;;;;;,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,''...        .....',;;:cclllllllc::;;,,,,;;;;'.......    .........................
// ....................................................',,'..........''''',,,;;;;;,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,''..............,;;:ccllllllc::;;,,,,,,,'.......    ...........................
// ............................................... ..  ....         ....'''',;;;;;,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,'''.........',;:cclllllc::;;,,,,'........      ............................
// ..........................................              .,;,'..      ...',;;;;;,,,',,,,,,,,,,'''''''''''''''''',,,,,,,,,,,,,,,,,,''......',;:ccllllcc:;;,,,'.    .        ..............................
// ..................................                     'lolc,...       ..',;;;;,,,',,,'''''''''''''''''''''''''''',,,,,,,,,,,,,,,,,''....',;:cclllcc:;;,,'.             ................................
// ..................................                    'odol:'....       ..,;;;,,,'',,''''''''''.................''''',,,,,,,,,,,,,,,,''..',;:cllllc::;;,.    ..'''.   ..................................
// ...............................                      .lxdoloc.      ...  .';;;,,,'''''''''''......         ........'''',,,,,,,,,,,,,,,'''',;:clllcc::::,....',::::'. ...................................
// .............................                        'dxdoc;'.     .,::.  .;;;,,,'''''''''.....                  .....''',,,,,,,,,,,,,,'',;:cclllc:::;,'.'',;;;,'... ...................................
// ...........................                          ,dxdoc'.      ....   ';;;,,,''''''''.......',;,,,..            ....'''',,,,,,,,,',,,,;:clllcc:,'....',,'''...     .................................
// ...........................                         .,lddo:...            ':;;,,''''''''......;:cc:;,'...              ...'''',,,,,,'',,,;:cllllc;'....'''......        ................................
// ......................                             ...':lo:'.....        .,:;,,,''',,''.....';ccooc'.....               ...''''',,'''',,;::clll:;'...',,'......         ................................
// ...................                               ';,'..';::,....        .;:;,,,',,,,''.....;:cldoc'..    .....          ...'''''''''',,;:clol:,'.''',,'....              ..............................
//    .........                                     .;:;,''....'............;::;,,,',,,,''....':clol:'...   ..,;,.     ....   ..',,,'''',,;:clol:;'...'''.....                 ............................
//                                                  .;:;;,''..............';::;;,,,,,,,'''....,:clol;..     ..,,'.     .''..  .',,;;;;;;;;:clol:,...........                    ...........................
//                                                  .;:;;,,'............',;:c:;;,,,,,,''''....,:cllc;.                ..;:;....,;;::::ccccllolc;...'''....                       ..........................
//                               .                  .,::;;;,,''.....''',;:cc::;,,,',,,''''.....:clll:'.               .,cc:,..';:ccclllllloooc:;''',,'..                      ..   ........................
//                           ..                      ,:::;;;;,,,,,,;;;:::ccc:;,,,''''''''......':cllc;'...............,:cc;'.';ccllloooooodol:;;,,''..                           ..    ....................
//                        ....                       .;::;;;;;;;;;::::ccccc:;,,,''''''''.........,;clc:;,............,;;,'..':cllloooooooool:;;'...                                ...   ..................
//                     .. ..'...                     .;:;;;;;;;;::::ccccc::;,,''''''''''............',;;;,..........''....',;:cllooooooooool:'.                                       ...  ................
//                  ...     ...........     ..........,;;;;;;;;;:::ccccc:;;,,''''''',,'''...........................'''',,;::cllloooooooool;.                                           .... ..............
//               ..           .........................;;;;;;;;;:cccccc:;,,,''''''',,,,,'''.........................''',,;::cclloooooooooc'                                                ....  ..........
//            ..                ........ ..............,;;,,;;::ccccc::;,,,,'''''',,,,,,,,''''''...................'',,;::cccllllooooool;.                                                   ....     .....
//         ..                        ..................';;,,;:cccccc::;,,,,'''''',,,,,,,,,,,,'''''''''.......''''',,,;;:::ccccllllllllc,.                                                       ....       
//       ..                            .......        .';;;;::ccccc:;;,,,,,,'''',,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,;;;;;;:::::ccccccccc,. ...                                                       ...     
//                                         ...         .,:::cccccc::;,;,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,;;;;;;;;:::::::c:,..........                                                     ...   
//                                         ..          .;:::ccccc::;;;;;;;;;;;;,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,;;;;;;;:::,.......''....                                                      .. 
//                                       ..............;::cccccc::;;;;;;;;;;;;;;;;;;,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,;;;;:::;'....  .'.......                                                       
//                                        ............,:ccccccc:::;:::::::::::::::;;;;;,,,,,,,,,,,,,,,,,,,,,,,'''''',,,,,,,;;;;::::,..    .'..........           ...'..                                    
//                                        ...........,:ccccccc::::::ccccccccccccc:::;;;;;,,,,,,,,,,,,,,,,,'''''''''',,,,,,;;;;::c:,,;,.  .'...............     .....co,                                    
//                                        ..........':ccccccc::::cccclllllllllllccc::;;;;;;;,,''',,,,,,,,,,'''''',,,,,,,,;;;;::c:'.,cc;'.........................  .oo.                                    
//                                        ..........;:ccccccc::ccclllloooooooollccc::::::::;,'''''',,,,,,,,,,'',,,,,,,,;;;;;:cc:'...;:;;,'......................  .;c'.                                    
//                                         ........':cccccccccccllloooooddddoolllccccccc:::;,'''''''',,,,,,,,,,,,,,,,,;;;;::ccc'   .'''''''''.............       .';,.                                     
//                                         .......';cccccccccclllooodddddddddoollccccccc:::;'''''''''',,,,,,,,,,,,,,,;;;;::ccc,.   .','.....'...............     .'...                                     
//                                          .....';cclllllllllooooddddxxxxddoollllcccccc::;,'''''''''',,,,,,,,,,,,,,;;;:::ccc,.      .............................. ...                                    
//                                            ..':cclllllllloooodddddxxxddddoolllllcccc:;,'''''''''''',,,,,,,,,,,,;;;:::cccc,.        ..  ...............................                                  
//                                           ..,:clllloooooooddddddddxxdddooolllcccc::;,'''''''''''''',,,,,,,,,,;;;::::cccc;.               ................................                               
//                                          .':ccllllooooooddddddxxddddddooollcc:::;;,,,'''''''''''''',,,,,,,,;;;:::cccccc;.                   ...............................                             
//                                         .':cccclllloooooddddddddddddooollc:;;;;;;;;,,,,,,,,'''''''',,,,,,,;;;:::cccclc;.                        ..............................                          
//                                         ..;:;::cclllooooddddddddoooollc::;;;;;::::::::;;;,,,''''''''',,,,;;;::ccccclc;.                                      ...................                        
//                                           .,;;;;::cllloooooooooolllcc:;;;:::cccccccccc:::;;,,,'''''',,,,;;;:::cccccc,.                                         ...................   .                  
//                                           .',,,,;;::ccllllllllccc:::;;::cccllllllllllllcc::;;,,''''',,,,;;:::cccccc,.                                              ................    ..               
//                                     .    ....',,,,,;;:::ccc::::;;;;:::cclllooooooooooolllcc:;;,,'''',,,;;;::ccccc:,.                                                  ..............      .             
//                                    ..    ......'''',,,;;;;;,,,,,,;;::ccclllllloooooooooollcc:;;,''',,,,;;:::cccc:'.                                            ..        ............       ..          
//                                    .     ..   .......',,,,''''..'''',,;;::::::::::cclllllllc::;,,'',,,,;:::cccc;'.                                                            .  ...                    
//                                   ..              ....,,,''...........'',,;;;;;;;;,,',;:cllcc:;,,'',,,;;:::cc:;..                                                                                       
//                                   ..                 .,,,'...........'',,;;::cclllcc;,''';ccc::;,'',,,;;:::c:,.                                                                                         
//                                 ..                  ..,,,'..  .......''',,;;::cclllllcc;,'';::;,''',,;;::::;,..                                                                                         
//        .....   .......         ...                  ..,,,'..     .....''''',,,;;;;:::ccc:;,,,;;,''',,;;:::;'...   ....                                                                                  
//  ............ .........                            ...,,'...       ....''''''''',,,,;;::::;,'',,'',,;;;:;,'...........                                                                                  
//       .              ..                           ....,,'...      ......''''''''',,,,,;;:::;,''''',;;;;;,'............                                                                                  

