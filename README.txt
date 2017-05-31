# TungstenCut

Code to run an Arduino-based system which uses hot tungsten filiment wire to cut rigging lines etc. 

Functionality includes GPS (Adafruit GPS Breakout V3), XBee communications (see below for commands), and SD data logging.

Usage Description:
When payload is powered (i.e. batteries plugged in and switch in "on" position), it will be in flight mode.
//     
//     Flight Mode:
//                 Payload will count up in milliseconds from zero until a specified time. One blink equals 5 minutes remaining. Once specified time has passed, payload will then 
//                 fire the burner continuously until the burner breaks. Payload will then enter Recovery Mode.
//    Recovery Mode:
//                 Payload will continuously flash LED's to indicate its status until it is recovered or powered off. If it decides that 
//                 the burn was unsuccessful, it will attempt to re-try burning until it decides it worked.


==============================================================================================================================================
__________________________________________________________________________________
 __   ______               _____                                          _     _ 
 \ \ / /  _ \             / ____|                                        | |   | |
  \ V /| |_) | ___  ___  | |     ___  _ __ ___  _ __ ___   __ _ _ __   __| |___| |
   > < |  _ < / _ \/ _ \ | |    / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` / __| |
  / . \| |_) |  __/  __/ | |___| (_) | | | | | | | | | | | (_| | | | | (_| \__ \_|
 /_/ \_\____/ \___|\___|  \_____\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|___(_)
----------------------------------------------------------------------------------

xBee Command Protocol: xBeeID + "?" + command + "!"

List of xBee Commands:

Command:        Example:        Function:
WA__            W1?WA15!        Adds specified amount of minutes to failsafe timer
                                Then returns remaining timer
WR__            W1?WR13!        Removes """"""""""""""""""""""""""""

WB              W1?WB!          Blinks the "Action LED". Does nothing else.

WX              W1?WX!          Initiates Cutdown!

WT              W1?WT!          Polls for remaining failsafe time

GPS             W1?GPS!         Polls most recent GPS data
==============================================================================================================================================