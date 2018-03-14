s# TungstenCut

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
XBee ID: "W" + the letter identifier of the stack. For example, stack A is "WA", B is "WB", etc.

xBee Command Protocol: xBeeID + "?" + command + "!"

List of xBee Commands:

Command:        Example:        Function:
WA__            WA?WA15!        adds time in minutes to failsafe timer, returns new timer

WR__            WA?WR13!        removes time from failsafe in minutes, returns remaining timer

WB              WA?WB!          Blinks the "Action LED". Does nothing else.

WX              WA?WX!          Initiates Cutdown

WT              WA?WT!          Polls for remaining failsafe time

GPS             WA?GPS!         Polls most recent GPS data

WD              WA?WD!          disables altitude cutdown

WU__            WA?WU50000!	sets new altitude for cutdown

WE		WA?WE!		enables altitude cutdown cutdown

WC   		WA?WC! 		enables timed burn. resets timer to 60 minutes

WS 		WA?WS!		disables timed burn.

WF		WA?WF! 		polls cutdown altitude

WQ		WA?WQ!  	enables beacon

WP		WA?WP!		disables bacon

TP              WA?TE!          polls for temperature

FA              WA?FA3!         add 3 minutes to floatTimer 

FS              WA?FS5!         subtract 5 minutes from foattimer

PF              WA?PF!          poll for float time
==============================================================================================================================================