# Don't use this code
Use https://github.com/debauchee/barrier instead


# clip
Cross platform clipboard sharing between 2 computers

I am using [Synergy](https://symless.com/synergy) for keyboard and mouse sharing between my Linux and Windows computers. This also provided me with clipboard sharing. However, after upgrading to Kubuntu 17.04 I could no longer get clipboard sharing to work with Synergy. And a newer version of synergy on windows didn't seem to share the keystrokes correctly,

As a temporary fix, [portaclip](https://github.com/arnholm/portaclip) was created. It kind of worked, but it had issues that became very annoying. After 3 years it was decided to make something better. Ideally it should be completely invisible and automatically share the clipboard between 2 computers, without side effects. clip is close to that goal even if it only shares text at the moment. 


When starting up for the first time, enter the local LAN IP address of the machine you are sending to and press OK to save configuration. Do the same on the other computer, so that they are "pointing at each other". Finally, on one of the computers, check the "Monitor Mouse" checkbox. By experience this works best on a Windows machine. It does not matter if the mouse/keyboard is connected to the other machine. When you move the mouse from one screen to another, the clipboard is automatically shared using UDP messaging.

Linux | Windows
------------ | -------------
![clip_linux](/images/linux.png)| ![clip_windows](/images/windows.png)

