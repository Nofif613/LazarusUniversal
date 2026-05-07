# LazarusUniversal
## Problem:

 **CPU and Graphics card can't work with large process cause their work with many process.**

## Solution:

**Lazarus solve this problem, it freeze trash process and give all resources to main process.**

## Under the hood:

**Lazarus use Windows API to control threads:**


```cpp   
    HANDLE hThreat = OpenThread(THREAD_SUSPEND_RESUME, false, te32.th32ThreadID);
    if (hThreat != NULL) {
     if (freeze == true) {
      SuspendThread(hThreat);
```
**And force memory:**


```cpp
OpenProcess(PROCESS_SET_QUOTA | PROCESS_SET_INFORMATION, false, targetPID);
if (hProcess != NULL)
{SetProcessWorkingSetSize(hProcess, (SIZE_T)-1, (SIZE_T)-1);
}
```
# instruction:
## Important:
**1. Put txt files to folder where will be LazarusUniversal.exe!!!!**

**2. The txt files must be save as ANSI or UTF - 16 LE**
## Put it in file that names FileLaz.txt:

**1. Name of GreenList.txt**

**2. Name of WhiteList.txt**

## For example:

**GreenList.txt**

**WhiteList.txt**
## Put names of bad programs to GreenList.txt for example:
**browser.exe**

**msEdge.exe**
## Put names of bad programs to WhiteList.txt for example:


**Cyberpunk2077.exe**


**Metro2033.exe**


## Using:
**To start press F8 Until appear a text "Program had started"**


**To stop press F12 Until appear a text "program had stoped"**


**To stop/resume GreenList press F9**
https://github.com/Nofif613/Lazarus-
