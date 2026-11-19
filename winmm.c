/*
 * P5R "keep running in background" - tiny winmm.dll proxy (v3).
 * Put the built winmm.dll next to P5R.exe. Delete it to undo.
 *
 * Inside the game's own process only:
 *  1) background  : swallows "lost focus / deactivated" messages so the game keeps running when alt-tabbed.
 *  2) spoof_focus : GetForegroundWindow / GetActiveWindow / GetFocus answer "the game window".
 *  3) block_*     : while ANOTHER app is in front, the game sees no keyboard / mouse / controller input.
 * Everything else in winmm is forwarded to the real Windows winmm.dll.
 *
 * Every feature can be switched off in p5r_bg.ini (created next to the DLL on first run).
 * Changes apply within about a second, no need to restart the game.
 * p5r_bg.log (same folder) shows what happened.
 */
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

/* ---------- forward every winmm export to the real DLL ---------- */
#pragma comment(linker, "/export:CloseDriver=C:\\Windows\\System32\\winmm.CloseDriver")
#pragma comment(linker, "/export:DefDriverProc=C:\\Windows\\System32\\winmm.DefDriverProc")
#pragma comment(linker, "/export:DriverCallback=C:\\Windows\\System32\\winmm.DriverCallback")
#pragma comment(linker, "/export:DrvGetModuleHandle=C:\\Windows\\System32\\winmm.DrvGetModuleHandle")
#pragma comment(linker, "/export:GetDriverModuleHandle=C:\\Windows\\System32\\winmm.GetDriverModuleHandle")
#pragma comment(linker, "/export:NotifyCallbackData=C:\\Windows\\System32\\winmm.NotifyCallbackData")
#pragma comment(linker, "/export:OpenDriver=C:\\Windows\\System32\\winmm.OpenDriver")
#pragma comment(linker, "/export:PlaySound=C:\\Windows\\System32\\winmm.PlaySound")
#pragma comment(linker, "/export:PlaySoundA=C:\\Windows\\System32\\winmm.PlaySoundA")
#pragma comment(linker, "/export:PlaySoundW=C:\\Windows\\System32\\winmm.PlaySoundW")
#pragma comment(linker, "/export:SendDriverMessage=C:\\Windows\\System32\\winmm.SendDriverMessage")
#pragma comment(linker, "/export:WOW32DriverCallback=C:\\Windows\\System32\\winmm.WOW32DriverCallback")
#pragma comment(linker, "/export:WOW32ResolveMultiMediaHandle=C:\\Windows\\System32\\winmm.WOW32ResolveMultiMediaHandle")
#pragma comment(linker, "/export:WOWAppExit=C:\\Windows\\System32\\winmm.WOWAppExit")
#pragma comment(linker, "/export:aux32Message=C:\\Windows\\System32\\winmm.aux32Message")
#pragma comment(linker, "/export:auxGetDevCapsA=C:\\Windows\\System32\\winmm.auxGetDevCapsA")
#pragma comment(linker, "/export:auxGetDevCapsW=C:\\Windows\\System32\\winmm.auxGetDevCapsW")
#pragma comment(linker, "/export:auxGetNumDevs=C:\\Windows\\System32\\winmm.auxGetNumDevs")
#pragma comment(linker, "/export:auxGetVolume=C:\\Windows\\System32\\winmm.auxGetVolume")
#pragma comment(linker, "/export:auxOutMessage=C:\\Windows\\System32\\winmm.auxOutMessage")
#pragma comment(linker, "/export:auxSetVolume=C:\\Windows\\System32\\winmm.auxSetVolume")
#pragma comment(linker, "/export:joy32Message=C:\\Windows\\System32\\winmm.joy32Message")
#pragma comment(linker, "/export:joyConfigChanged=C:\\Windows\\System32\\winmm.joyConfigChanged")
#pragma comment(linker, "/export:joyGetDevCapsA=C:\\Windows\\System32\\winmm.joyGetDevCapsA")
#pragma comment(linker, "/export:joyGetDevCapsW=C:\\Windows\\System32\\winmm.joyGetDevCapsW")
#pragma comment(linker, "/export:joyGetNumDevs=C:\\Windows\\System32\\winmm.joyGetNumDevs")
#pragma comment(linker, "/export:joyGetPos=C:\\Windows\\System32\\winmm.joyGetPos")
#pragma comment(linker, "/export:joyGetPosEx=C:\\Windows\\System32\\winmm.joyGetPosEx")
#pragma comment(linker, "/export:joyGetThreshold=C:\\Windows\\System32\\winmm.joyGetThreshold")
#pragma comment(linker, "/export:joyReleaseCapture=C:\\Windows\\System32\\winmm.joyReleaseCapture")
#pragma comment(linker, "/export:joySetCapture=C:\\Windows\\System32\\winmm.joySetCapture")
#pragma comment(linker, "/export:joySetThreshold=C:\\Windows\\System32\\winmm.joySetThreshold")
#pragma comment(linker, "/export:mci32Message=C:\\Windows\\System32\\winmm.mci32Message")
#pragma comment(linker, "/export:mciDriverNotify=C:\\Windows\\System32\\winmm.mciDriverNotify")
#pragma comment(linker, "/export:mciDriverYield=C:\\Windows\\System32\\winmm.mciDriverYield")
#pragma comment(linker, "/export:mciExecute=C:\\Windows\\System32\\winmm.mciExecute")
#pragma comment(linker, "/export:mciFreeCommandResource=C:\\Windows\\System32\\winmm.mciFreeCommandResource")
#pragma comment(linker, "/export:mciGetCreatorTask=C:\\Windows\\System32\\winmm.mciGetCreatorTask")
#pragma comment(linker, "/export:mciGetDeviceIDA=C:\\Windows\\System32\\winmm.mciGetDeviceIDA")
#pragma comment(linker, "/export:mciGetDeviceIDFromElementIDA=C:\\Windows\\System32\\winmm.mciGetDeviceIDFromElementIDA")
#pragma comment(linker, "/export:mciGetDeviceIDFromElementIDW=C:\\Windows\\System32\\winmm.mciGetDeviceIDFromElementIDW")
#pragma comment(linker, "/export:mciGetDeviceIDW=C:\\Windows\\System32\\winmm.mciGetDeviceIDW")
#pragma comment(linker, "/export:mciGetDriverData=C:\\Windows\\System32\\winmm.mciGetDriverData")
#pragma comment(linker, "/export:mciGetErrorStringA=C:\\Windows\\System32\\winmm.mciGetErrorStringA")
#pragma comment(linker, "/export:mciGetErrorStringW=C:\\Windows\\System32\\winmm.mciGetErrorStringW")
#pragma comment(linker, "/export:mciGetYieldProc=C:\\Windows\\System32\\winmm.mciGetYieldProc")
#pragma comment(linker, "/export:mciLoadCommandResource=C:\\Windows\\System32\\winmm.mciLoadCommandResource")
#pragma comment(linker, "/export:mciSendCommandA=C:\\Windows\\System32\\winmm.mciSendCommandA")
#pragma comment(linker, "/export:mciSendCommandW=C:\\Windows\\System32\\winmm.mciSendCommandW")
#pragma comment(linker, "/export:mciSendStringA=C:\\Windows\\System32\\winmm.mciSendStringA")
#pragma comment(linker, "/export:mciSendStringW=C:\\Windows\\System32\\winmm.mciSendStringW")
#pragma comment(linker, "/export:mciSetDriverData=C:\\Windows\\System32\\winmm.mciSetDriverData")
#pragma comment(linker, "/export:mciSetYieldProc=C:\\Windows\\System32\\winmm.mciSetYieldProc")
#pragma comment(linker, "/export:mid32Message=C:\\Windows\\System32\\winmm.mid32Message")
#pragma comment(linker, "/export:midiConnect=C:\\Windows\\System32\\winmm.midiConnect")
#pragma comment(linker, "/export:midiDisconnect=C:\\Windows\\System32\\winmm.midiDisconnect")
#pragma comment(linker, "/export:midiInAddBuffer=C:\\Windows\\System32\\winmm.midiInAddBuffer")
#pragma comment(linker, "/export:midiInClose=C:\\Windows\\System32\\winmm.midiInClose")
#pragma comment(linker, "/export:midiInGetDevCapsA=C:\\Windows\\System32\\winmm.midiInGetDevCapsA")
#pragma comment(linker, "/export:midiInGetDevCapsW=C:\\Windows\\System32\\winmm.midiInGetDevCapsW")
#pragma comment(linker, "/export:midiInGetErrorTextA=C:\\Windows\\System32\\winmm.midiInGetErrorTextA")
#pragma comment(linker, "/export:midiInGetErrorTextW=C:\\Windows\\System32\\winmm.midiInGetErrorTextW")
#pragma comment(linker, "/export:midiInGetID=C:\\Windows\\System32\\winmm.midiInGetID")
#pragma comment(linker, "/export:midiInGetNumDevs=C:\\Windows\\System32\\winmm.midiInGetNumDevs")
#pragma comment(linker, "/export:midiInMessage=C:\\Windows\\System32\\winmm.midiInMessage")
#pragma comment(linker, "/export:midiInOpen=C:\\Windows\\System32\\winmm.midiInOpen")
#pragma comment(linker, "/export:midiInPrepareHeader=C:\\Windows\\System32\\winmm.midiInPrepareHeader")
#pragma comment(linker, "/export:midiInReset=C:\\Windows\\System32\\winmm.midiInReset")
#pragma comment(linker, "/export:midiInStart=C:\\Windows\\System32\\winmm.midiInStart")
#pragma comment(linker, "/export:midiInStop=C:\\Windows\\System32\\winmm.midiInStop")
#pragma comment(linker, "/export:midiInUnprepareHeader=C:\\Windows\\System32\\winmm.midiInUnprepareHeader")
#pragma comment(linker, "/export:midiOutCacheDrumPatches=C:\\Windows\\System32\\winmm.midiOutCacheDrumPatches")
#pragma comment(linker, "/export:midiOutCachePatches=C:\\Windows\\System32\\winmm.midiOutCachePatches")
#pragma comment(linker, "/export:midiOutClose=C:\\Windows\\System32\\winmm.midiOutClose")
#pragma comment(linker, "/export:midiOutGetDevCapsA=C:\\Windows\\System32\\winmm.midiOutGetDevCapsA")
#pragma comment(linker, "/export:midiOutGetDevCapsW=C:\\Windows\\System32\\winmm.midiOutGetDevCapsW")
#pragma comment(linker, "/export:midiOutGetErrorTextA=C:\\Windows\\System32\\winmm.midiOutGetErrorTextA")
#pragma comment(linker, "/export:midiOutGetErrorTextW=C:\\Windows\\System32\\winmm.midiOutGetErrorTextW")
#pragma comment(linker, "/export:midiOutGetID=C:\\Windows\\System32\\winmm.midiOutGetID")
#pragma comment(linker, "/export:midiOutGetNumDevs=C:\\Windows\\System32\\winmm.midiOutGetNumDevs")
#pragma comment(linker, "/export:midiOutGetVolume=C:\\Windows\\System32\\winmm.midiOutGetVolume")
#pragma comment(linker, "/export:midiOutLongMsg=C:\\Windows\\System32\\winmm.midiOutLongMsg")
#pragma comment(linker, "/export:midiOutMessage=C:\\Windows\\System32\\winmm.midiOutMessage")
#pragma comment(linker, "/export:midiOutOpen=C:\\Windows\\System32\\winmm.midiOutOpen")
#pragma comment(linker, "/export:midiOutPrepareHeader=C:\\Windows\\System32\\winmm.midiOutPrepareHeader")
#pragma comment(linker, "/export:midiOutReset=C:\\Windows\\System32\\winmm.midiOutReset")
#pragma comment(linker, "/export:midiOutSetVolume=C:\\Windows\\System32\\winmm.midiOutSetVolume")
#pragma comment(linker, "/export:midiOutShortMsg=C:\\Windows\\System32\\winmm.midiOutShortMsg")
#pragma comment(linker, "/export:midiOutUnprepareHeader=C:\\Windows\\System32\\winmm.midiOutUnprepareHeader")
#pragma comment(linker, "/export:midiStreamClose=C:\\Windows\\System32\\winmm.midiStreamClose")
#pragma comment(linker, "/export:midiStreamOpen=C:\\Windows\\System32\\winmm.midiStreamOpen")
#pragma comment(linker, "/export:midiStreamOut=C:\\Windows\\System32\\winmm.midiStreamOut")
#pragma comment(linker, "/export:midiStreamPause=C:\\Windows\\System32\\winmm.midiStreamPause")
#pragma comment(linker, "/export:midiStreamPosition=C:\\Windows\\System32\\winmm.midiStreamPosition")
#pragma comment(linker, "/export:midiStreamProperty=C:\\Windows\\System32\\winmm.midiStreamProperty")
#pragma comment(linker, "/export:midiStreamRestart=C:\\Windows\\System32\\winmm.midiStreamRestart")
#pragma comment(linker, "/export:midiStreamStop=C:\\Windows\\System32\\winmm.midiStreamStop")
#pragma comment(linker, "/export:mixerClose=C:\\Windows\\System32\\winmm.mixerClose")
#pragma comment(linker, "/export:mixerGetControlDetailsA=C:\\Windows\\System32\\winmm.mixerGetControlDetailsA")
#pragma comment(linker, "/export:mixerGetControlDetailsW=C:\\Windows\\System32\\winmm.mixerGetControlDetailsW")
#pragma comment(linker, "/export:mixerGetDevCapsA=C:\\Windows\\System32\\winmm.mixerGetDevCapsA")
#pragma comment(linker, "/export:mixerGetDevCapsW=C:\\Windows\\System32\\winmm.mixerGetDevCapsW")
#pragma comment(linker, "/export:mixerGetID=C:\\Windows\\System32\\winmm.mixerGetID")
#pragma comment(linker, "/export:mixerGetLineControlsA=C:\\Windows\\System32\\winmm.mixerGetLineControlsA")
#pragma comment(linker, "/export:mixerGetLineControlsW=C:\\Windows\\System32\\winmm.mixerGetLineControlsW")
#pragma comment(linker, "/export:mixerGetLineInfoA=C:\\Windows\\System32\\winmm.mixerGetLineInfoA")
#pragma comment(linker, "/export:mixerGetLineInfoW=C:\\Windows\\System32\\winmm.mixerGetLineInfoW")
#pragma comment(linker, "/export:mixerGetNumDevs=C:\\Windows\\System32\\winmm.mixerGetNumDevs")
#pragma comment(linker, "/export:mixerMessage=C:\\Windows\\System32\\winmm.mixerMessage")
#pragma comment(linker, "/export:mixerOpen=C:\\Windows\\System32\\winmm.mixerOpen")
#pragma comment(linker, "/export:mixerSetControlDetails=C:\\Windows\\System32\\winmm.mixerSetControlDetails")
#pragma comment(linker, "/export:mmDrvInstall=C:\\Windows\\System32\\winmm.mmDrvInstall")
#pragma comment(linker, "/export:mmGetCurrentTask=C:\\Windows\\System32\\winmm.mmGetCurrentTask")
#pragma comment(linker, "/export:mmTaskBlock=C:\\Windows\\System32\\winmm.mmTaskBlock")
#pragma comment(linker, "/export:mmTaskCreate=C:\\Windows\\System32\\winmm.mmTaskCreate")
#pragma comment(linker, "/export:mmTaskSignal=C:\\Windows\\System32\\winmm.mmTaskSignal")
#pragma comment(linker, "/export:mmTaskYield=C:\\Windows\\System32\\winmm.mmTaskYield")
#pragma comment(linker, "/export:mmioAdvance=C:\\Windows\\System32\\winmm.mmioAdvance")
#pragma comment(linker, "/export:mmioAscend=C:\\Windows\\System32\\winmm.mmioAscend")
#pragma comment(linker, "/export:mmioClose=C:\\Windows\\System32\\winmm.mmioClose")
#pragma comment(linker, "/export:mmioCreateChunk=C:\\Windows\\System32\\winmm.mmioCreateChunk")
#pragma comment(linker, "/export:mmioDescend=C:\\Windows\\System32\\winmm.mmioDescend")
#pragma comment(linker, "/export:mmioFlush=C:\\Windows\\System32\\winmm.mmioFlush")
#pragma comment(linker, "/export:mmioGetInfo=C:\\Windows\\System32\\winmm.mmioGetInfo")
#pragma comment(linker, "/export:mmioInstallIOProcA=C:\\Windows\\System32\\winmm.mmioInstallIOProcA")
#pragma comment(linker, "/export:mmioInstallIOProcW=C:\\Windows\\System32\\winmm.mmioInstallIOProcW")
#pragma comment(linker, "/export:mmioOpenA=C:\\Windows\\System32\\winmm.mmioOpenA")
#pragma comment(linker, "/export:mmioOpenW=C:\\Windows\\System32\\winmm.mmioOpenW")
#pragma comment(linker, "/export:mmioRead=C:\\Windows\\System32\\winmm.mmioRead")
#pragma comment(linker, "/export:mmioRenameA=C:\\Windows\\System32\\winmm.mmioRenameA")
#pragma comment(linker, "/export:mmioRenameW=C:\\Windows\\System32\\winmm.mmioRenameW")
#pragma comment(linker, "/export:mmioSendMessage=C:\\Windows\\System32\\winmm.mmioSendMessage")
#pragma comment(linker, "/export:mmioSeek=C:\\Windows\\System32\\winmm.mmioSeek")
#pragma comment(linker, "/export:mmioSetBuffer=C:\\Windows\\System32\\winmm.mmioSetBuffer")
#pragma comment(linker, "/export:mmioSetInfo=C:\\Windows\\System32\\winmm.mmioSetInfo")
#pragma comment(linker, "/export:mmioStringToFOURCCA=C:\\Windows\\System32\\winmm.mmioStringToFOURCCA")
#pragma comment(linker, "/export:mmioStringToFOURCCW=C:\\Windows\\System32\\winmm.mmioStringToFOURCCW")
#pragma comment(linker, "/export:mmioWrite=C:\\Windows\\System32\\winmm.mmioWrite")
#pragma comment(linker, "/export:mmsystemGetVersion=C:\\Windows\\System32\\winmm.mmsystemGetVersion")
#pragma comment(linker, "/export:mod32Message=C:\\Windows\\System32\\winmm.mod32Message")
#pragma comment(linker, "/export:mxd32Message=C:\\Windows\\System32\\winmm.mxd32Message")
#pragma comment(linker, "/export:sndPlaySoundA=C:\\Windows\\System32\\winmm.sndPlaySoundA")
#pragma comment(linker, "/export:sndPlaySoundW=C:\\Windows\\System32\\winmm.sndPlaySoundW")
#pragma comment(linker, "/export:tid32Message=C:\\Windows\\System32\\winmm.tid32Message")
#pragma comment(linker, "/export:timeBeginPeriod=C:\\Windows\\System32\\winmm.timeBeginPeriod")
#pragma comment(linker, "/export:timeEndPeriod=C:\\Windows\\System32\\winmm.timeEndPeriod")
#pragma comment(linker, "/export:timeGetDevCaps=C:\\Windows\\System32\\winmm.timeGetDevCaps")
#pragma comment(linker, "/export:timeGetSystemTime=C:\\Windows\\System32\\winmm.timeGetSystemTime")
#pragma comment(linker, "/export:timeGetTime=C:\\Windows\\System32\\winmm.timeGetTime")
#pragma comment(linker, "/export:timeKillEvent=C:\\Windows\\System32\\winmm.timeKillEvent")
#pragma comment(linker, "/export:timeSetEvent=C:\\Windows\\System32\\winmm.timeSetEvent")
#pragma comment(linker, "/export:waveInAddBuffer=C:\\Windows\\System32\\winmm.waveInAddBuffer")
#pragma comment(linker, "/export:waveInClose=C:\\Windows\\System32\\winmm.waveInClose")
#pragma comment(linker, "/export:waveInGetDevCapsA=C:\\Windows\\System32\\winmm.waveInGetDevCapsA")
#pragma comment(linker, "/export:waveInGetDevCapsW=C:\\Windows\\System32\\winmm.waveInGetDevCapsW")
#pragma comment(linker, "/export:waveInGetErrorTextA=C:\\Windows\\System32\\winmm.waveInGetErrorTextA")
#pragma comment(linker, "/export:waveInGetErrorTextW=C:\\Windows\\System32\\winmm.waveInGetErrorTextW")
#pragma comment(linker, "/export:waveInGetID=C:\\Windows\\System32\\winmm.waveInGetID")
#pragma comment(linker, "/export:waveInGetNumDevs=C:\\Windows\\System32\\winmm.waveInGetNumDevs")
#pragma comment(linker, "/export:waveInGetPosition=C:\\Windows\\System32\\winmm.waveInGetPosition")
#pragma comment(linker, "/export:waveInMessage=C:\\Windows\\System32\\winmm.waveInMessage")
#pragma comment(linker, "/export:waveInOpen=C:\\Windows\\System32\\winmm.waveInOpen")
#pragma comment(linker, "/export:waveInPrepareHeader=C:\\Windows\\System32\\winmm.waveInPrepareHeader")
#pragma comment(linker, "/export:waveInReset=C:\\Windows\\System32\\winmm.waveInReset")
#pragma comment(linker, "/export:waveInStart=C:\\Windows\\System32\\winmm.waveInStart")
#pragma comment(linker, "/export:waveInStop=C:\\Windows\\System32\\winmm.waveInStop")
#pragma comment(linker, "/export:waveInUnprepareHeader=C:\\Windows\\System32\\winmm.waveInUnprepareHeader")
#pragma comment(linker, "/export:waveOutBreakLoop=C:\\Windows\\System32\\winmm.waveOutBreakLoop")
#pragma comment(linker, "/export:waveOutClose=C:\\Windows\\System32\\winmm.waveOutClose")
#pragma comment(linker, "/export:waveOutGetDevCapsA=C:\\Windows\\System32\\winmm.waveOutGetDevCapsA")
#pragma comment(linker, "/export:waveOutGetDevCapsW=C:\\Windows\\System32\\winmm.waveOutGetDevCapsW")
#pragma comment(linker, "/export:waveOutGetErrorTextA=C:\\Windows\\System32\\winmm.waveOutGetErrorTextA")
#pragma comment(linker, "/export:waveOutGetErrorTextW=C:\\Windows\\System32\\winmm.waveOutGetErrorTextW")
#pragma comment(linker, "/export:waveOutGetID=C:\\Windows\\System32\\winmm.waveOutGetID")
#pragma comment(linker, "/export:waveOutGetNumDevs=C:\\Windows\\System32\\winmm.waveOutGetNumDevs")
#pragma comment(linker, "/export:waveOutGetPitch=C:\\Windows\\System32\\winmm.waveOutGetPitch")
#pragma comment(linker, "/export:waveOutGetPlaybackRate=C:\\Windows\\System32\\winmm.waveOutGetPlaybackRate")
#pragma comment(linker, "/export:waveOutGetPosition=C:\\Windows\\System32\\winmm.waveOutGetPosition")
#pragma comment(linker, "/export:waveOutGetVolume=C:\\Windows\\System32\\winmm.waveOutGetVolume")
#pragma comment(linker, "/export:waveOutMessage=C:\\Windows\\System32\\winmm.waveOutMessage")
#pragma comment(linker, "/export:waveOutOpen=C:\\Windows\\System32\\winmm.waveOutOpen")
#pragma comment(linker, "/export:waveOutPause=C:\\Windows\\System32\\winmm.waveOutPause")
#pragma comment(linker, "/export:waveOutPrepareHeader=C:\\Windows\\System32\\winmm.waveOutPrepareHeader")
#pragma comment(linker, "/export:waveOutReset=C:\\Windows\\System32\\winmm.waveOutReset")
#pragma comment(linker, "/export:waveOutRestart=C:\\Windows\\System32\\winmm.waveOutRestart")
#pragma comment(linker, "/export:waveOutSetPitch=C:\\Windows\\System32\\winmm.waveOutSetPitch")
#pragma comment(linker, "/export:waveOutSetPlaybackRate=C:\\Windows\\System32\\winmm.waveOutSetPlaybackRate")
#pragma comment(linker, "/export:waveOutSetVolume=C:\\Windows\\System32\\winmm.waveOutSetVolume")
#pragma comment(linker, "/export:waveOutUnprepareHeader=C:\\Windows\\System32\\winmm.waveOutUnprepareHeader")
#pragma comment(linker, "/export:waveOutWrite=C:\\Windows\\System32\\winmm.waveOutWrite")
#pragma comment(linker, "/export:wid32Message=C:\\Windows\\System32\\winmm.wid32Message")
#pragma comment(linker, "/export:wod32Message=C:\\Windows\\System32\\winmm.wod32Message")

/* ---------- settings / paths ---------- */
static HINSTANCE g_inst = NULL;
static char g_iniPath[MAX_PATH + 16] = "p5r_bg.ini";
static char g_logPath[MAX_PATH + 16] = "p5r_bg.log";

static volatile LONG cfg_background       = 1;
static volatile LONG cfg_spoof_focus      = 1;
static volatile LONG cfg_block_keyboard   = 1;
static volatile LONG cfg_block_mouse      = 1;
static volatile LONG cfg_block_controller = 1;

static void LogMsg(const char *fmt, ...)
{
    FILE *f = fopen(g_logPath, "a");
    va_list ap;
    if (!f) return;
    va_start(ap, fmt);
    vfprintf(f, fmt, ap);
    va_end(ap);
    fputc('\n', f);
    fclose(f);
}

static void InitPaths(void)
{
    char dir[MAX_PATH + 16];
    char *p;
    DWORD n = GetModuleFileNameA(g_inst, dir, MAX_PATH);
    if (n == 0 || n >= MAX_PATH) return;
    p = strrchr(dir, '\\');
    if (!p) return;
    p[1] = 0;
    lstrcpyA(g_iniPath, dir); lstrcatA(g_iniPath, "p5r_bg.ini");
    lstrcpyA(g_logPath, dir); lstrcatA(g_logPath, "p5r_bg.log");
}

static void WriteDefaultIni(void)
{
    FILE *f;
    if (GetFileAttributesA(g_iniPath) != INVALID_FILE_ATTRIBUTES) return;
    f = fopen(g_iniPath, "w");
    if (!f) return;
    fputs("; p5r_bg settings. 1 = on, 0 = off.\n"
          "; Changes apply within about a second, no need to restart the game.\n"
          "[main]\n"
          "; game keeps running when you alt-tab\n"
          "background=1\n"
          "; game believes its window is always the active one (needed by some games, try 0 if something breaks)\n"
          "spoof_focus=1\n"
          "; while another app is in front, the game ignores the keyboard\n"
          "block_keyboard=1\n"
          "; ... the mouse\n"
          "block_mouse=1\n"
          "; ... the controller\n"
          "block_controller=1\n", f);
    fclose(f);
}

static void ReadConfig(void)
{
    InterlockedExchange(&cfg_background,       GetPrivateProfileIntA("main", "background",       1, g_iniPath));
    InterlockedExchange(&cfg_spoof_focus,      GetPrivateProfileIntA("main", "spoof_focus",      1, g_iniPath));
    InterlockedExchange(&cfg_block_keyboard,   GetPrivateProfileIntA("main", "block_keyboard",   1, g_iniPath));
    InterlockedExchange(&cfg_block_mouse,      GetPrivateProfileIntA("main", "block_mouse",      1, g_iniPath));
    InterlockedExchange(&cfg_block_controller, GetPrivateProfileIntA("main", "block_controller", 1, g_iniPath));
}

static HWND    g_hwnd = NULL;
static WNDPROC g_orig = NULL;
static BOOL    g_unicode = TRUE;

/* TRUE only when ANOTHER program's window is really in front.
   (If we can't tell, we say FALSE so the game keeps working normally.
   Our own imports are not patched, so GetForegroundWindow here is the real one.) */
static BOOL ForegroundIsOtherApp(void)
{
    static DWORD last = 0;
    static BOOL cached = FALSE;
    DWORD now = GetTickCount();
    if (now - last >= 15) {
        DWORD pid = 0;
        HWND fg = GetForegroundWindow();
        if (fg) GetWindowThreadProcessId(fg, &pid);
        cached = (fg != NULL && pid != GetCurrentProcessId());
        last = now;
    }
    return cached;
}

/* ---------- window procedure ---------- */
static LRESULT CALLBACK NewProc(HWND h, UINT m, WPARAM w, LPARAM l)
{
    if (ForegroundIsOtherApp()) {
        if (cfg_block_keyboard && m >= WM_KEYFIRST && m <= WM_KEYLAST) return 0;
        if (cfg_block_mouse && m >= WM_MOUSEFIRST && m <= WM_MOUSELAST) return 0;
        if ((cfg_block_keyboard || cfg_block_mouse) && m == WM_INPUT)
            return g_unicode ? DefWindowProcW(h, m, w, l) : DefWindowProcA(h, m, w, l);
    }
    if (cfg_background) {
        switch (m) {
        case WM_ACTIVATEAPP:
            if (!w) return 0;
            break;
        case WM_ACTIVATE:
            if (LOWORD(w) == WA_INACTIVE) return 0;
            break;
        case WM_KILLFOCUS:
            return 0;
        case WM_NCACTIVATE:
            w = TRUE;
            break;
        }
    }
    return g_unicode ? CallWindowProcW(g_orig, h, m, w, l)
                     : CallWindowProcA(g_orig, h, m, w, l);
}

/* ---------- find the game's main window ---------- */
static BOOL CALLBACK EnumCb(HWND h, LPARAM lp)
{
    DWORD pid = 0;
    RECT r;
    GetWindowThreadProcessId(h, &pid);
    if (pid != GetCurrentProcessId()) return TRUE;
    if (!IsWindowVisible(h)) return TRUE;
    if (GetWindow(h, GW_OWNER) != NULL) return TRUE;
    if (!GetWindowRect(h, &r)) return TRUE;
    if ((r.right - r.left) < 640 || (r.bottom - r.top) < 360) return TRUE;
    *(HWND *)lp = h;
    return FALSE;
}

static void EnsureSubclassed(void)
{
    WNDPROC cur;
    g_unicode = IsWindowUnicode(g_hwnd);
    cur = (WNDPROC)(g_unicode ? GetWindowLongPtrW(g_hwnd, GWLP_WNDPROC)
                              : GetWindowLongPtrA(g_hwnd, GWLP_WNDPROC));
    if (cur == NewProc) return;
    g_orig = (WNDPROC)(g_unicode ? SetWindowLongPtrW(g_hwnd, GWLP_WNDPROC, (LONG_PTR)NewProc)
                                 : SetWindowLongPtrA(g_hwnd, GWLP_WNDPROC, (LONG_PTR)NewProc));
    LogMsg("window %p subclassed (unicode=%d)", (void *)g_hwnd, (int)g_unicode);
}

/* ---------- IAT hooks ---------- */
static HWND (WINAPI *oGetForegroundWindow)(void) = NULL;
static HWND (WINAPI *oGetActiveWindow)(void)     = NULL;
static HWND (WINAPI *oGetFocus)(void)            = NULL;

static HWND WINAPI hkGetForegroundWindow(void) { return (cfg_spoof_focus && g_hwnd && IsWindow(g_hwnd)) ? g_hwnd : oGetForegroundWindow(); }
static HWND WINAPI hkGetActiveWindow(void)     { return (cfg_spoof_focus && g_hwnd && IsWindow(g_hwnd)) ? g_hwnd : oGetActiveWindow(); }
static HWND WINAPI hkGetFocus(void)            { return (cfg_spoof_focus && g_hwnd && IsWindow(g_hwnd)) ? g_hwnd : oGetFocus(); }

typedef struct { WORD wButtons; BYTE bLeftTrigger; BYTE bRightTrigger; SHORT sThumbLX; SHORT sThumbLY; SHORT sThumbRX; SHORT sThumbRY; } MY_XGAMEPAD;
typedef struct { DWORD dwPacketNumber; MY_XGAMEPAD Gamepad; } MY_XSTATE;

static SHORT (WINAPI *oGetAsyncKeyState)(int)              = NULL;
static SHORT (WINAPI *oGetKeyState)(int)                   = NULL;
static BOOL  (WINAPI *oGetKeyboardState)(PBYTE)            = NULL;
static DWORD (WINAPI *oXInputGetState)(DWORD, MY_XSTATE *) = NULL;

static BOOL BlockKey(int vk)
{
    BOOL isMouse = (vk >= 1 && vk <= 6 && vk != 3);
    if (!ForegroundIsOtherApp()) return FALSE;
    return isMouse ? (cfg_block_mouse != 0) : (cfg_block_keyboard != 0);
}

static SHORT WINAPI hkGetAsyncKeyState(int vk) { return BlockKey(vk) ? 0 : oGetAsyncKeyState(vk); }
static SHORT WINAPI hkGetKeyState(int vk)      { return BlockKey(vk) ? 0 : oGetKeyState(vk); }
static BOOL  WINAPI hkGetKeyboardState(PBYTE buf)
{
    BOOL r = oGetKeyboardState(buf);
    if (r && buf && cfg_block_keyboard && ForegroundIsOtherApp()) memset(buf, 0, 256);
    return r;
}
static DWORD WINAPI hkXInputGetState(DWORD idx, MY_XSTATE *st)
{
    DWORD r = oXInputGetState(idx, st);
    if (r == ERROR_SUCCESS && st && cfg_block_controller && ForegroundIsOtherApp())
        memset(&st->Gamepad, 0, sizeof(st->Gamepad));
    return r;
}

static BOOL PatchIAT(HMODULE mod, const char *dll, const char *fn, void *repl, void **orig)
{
    BYTE *base = (BYTE *)mod;
    IMAGE_DOS_HEADER *dos = (IMAGE_DOS_HEADER *)base;
    IMAGE_NT_HEADERS *nt = (IMAGE_NT_HEADERS *)(base + dos->e_lfanew);
    IMAGE_DATA_DIRECTORY dir = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    IMAGE_IMPORT_DESCRIPTOR *imp;

    if (!dir.VirtualAddress) return FALSE;
    imp = (IMAGE_IMPORT_DESCRIPTOR *)(base + dir.VirtualAddress);
    for (; imp->Name; imp++) {
        IMAGE_THUNK_DATA *oft, *ft;
        if (_stricmp((const char *)(base + imp->Name), dll) != 0) continue;
        if (!imp->OriginalFirstThunk) continue;
        oft = (IMAGE_THUNK_DATA *)(base + imp->OriginalFirstThunk);
        ft  = (IMAGE_THUNK_DATA *)(base + imp->FirstThunk);
        for (; oft->u1.AddressOfData; oft++, ft++) {
            IMAGE_IMPORT_BY_NAME *ibn;
            DWORD old;
            if (IMAGE_SNAP_BY_ORDINAL(oft->u1.Ordinal)) continue;
            ibn = (IMAGE_IMPORT_BY_NAME *)(base + oft->u1.AddressOfData);
            if (strcmp((const char *)ibn->Name, fn) != 0) continue;
            if (!VirtualProtect(&ft->u1.Function, sizeof(void *), PAGE_READWRITE, &old)) return FALSE;
            *orig = (void *)ft->u1.Function;
            ft->u1.Function = (ULONGLONG)(ULONG_PTR)repl;
            VirtualProtect(&ft->u1.Function, sizeof(void *), old, &old);
            return TRUE;
        }
    }
    return FALSE;
}

/* ---------- log what the game imports (for debugging) ---------- */
static void LogImports(HMODULE mod)
{
    static const char *kw[] = { "Key", "Input", "Async", "Cursor", "Focus", "Foreground", "Active", "Raw", "Joy", "Ime", "Char", "Text" };
    BYTE *base = (BYTE *)mod;
    IMAGE_DOS_HEADER *dos = (IMAGE_DOS_HEADER *)base;
    IMAGE_NT_HEADERS *nt = (IMAGE_NT_HEADERS *)(base + dos->e_lfanew);
    IMAGE_DATA_DIRECTORY dir = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    IMAGE_IMPORT_DESCRIPTOR *imp;
    if (!dir.VirtualAddress) { LogMsg("no import table"); return; }
    imp = (IMAGE_IMPORT_DESCRIPTOR *)(base + dir.VirtualAddress);
    for (; imp->Name; imp++) {
        const char *dn = (const char *)(base + imp->Name);
        int isu = (_stricmp(dn, "user32.dll") == 0 || _stricmp(dn, "imm32.dll") == 0);
        LogMsg("imports dll: %s", dn);
        if (isu && imp->OriginalFirstThunk) {
            IMAGE_THUNK_DATA *oft = (IMAGE_THUNK_DATA *)(base + imp->OriginalFirstThunk);
            for (; oft->u1.AddressOfData; oft++) {
                IMAGE_IMPORT_BY_NAME *ibn;
                size_t i;
                if (IMAGE_SNAP_BY_ORDINAL(oft->u1.Ordinal)) continue;
                ibn = (IMAGE_IMPORT_BY_NAME *)(base + oft->u1.AddressOfData);
                for (i = 0; i < sizeof(kw) / sizeof(kw[0]); i++) {
                    if (strstr((const char *)ibn->Name, kw[i])) { LogMsg("  %s: %s", dn, (const char *)ibn->Name); break; }
                }
            }
        }
    }
}

/* ---------- worker thread ---------- */
static DWORD WINAPI Worker(LPVOID p)
{
    HMODULE exe;
    int tick = 0;
    (void)p;
    InitPaths();
    { FILE *f = fopen(g_logPath, "w"); if (f) fclose(f); }   /* fresh log each launch */
    WriteDefaultIni();
    ReadConfig();
    LogMsg("p5r_bg v3 started");
    LogMsg("config: background=%ld spoof_focus=%ld block_keyboard=%ld block_mouse=%ld block_controller=%ld",
           cfg_background, cfg_spoof_focus, cfg_block_keyboard, cfg_block_mouse, cfg_block_controller);
    Sleep(2000);
    exe = GetModuleHandleW(NULL);
    LogMsg("IAT GetForegroundWindow: %d", (int)PatchIAT(exe, "user32.dll", "GetForegroundWindow", (void *)hkGetForegroundWindow, (void **)&oGetForegroundWindow));
    LogMsg("IAT GetActiveWindow: %d",     (int)PatchIAT(exe, "user32.dll", "GetActiveWindow",     (void *)hkGetActiveWindow,     (void **)&oGetActiveWindow));
    LogMsg("IAT GetFocus: %d",            (int)PatchIAT(exe, "user32.dll", "GetFocus",            (void *)hkGetFocus,            (void **)&oGetFocus));
    LogMsg("IAT GetAsyncKeyState: %d",    (int)PatchIAT(exe, "user32.dll", "GetAsyncKeyState",    (void *)hkGetAsyncKeyState,    (void **)&oGetAsyncKeyState));
    LogMsg("IAT GetKeyState: %d",         (int)PatchIAT(exe, "user32.dll", "GetKeyState",         (void *)hkGetKeyState,         (void **)&oGetKeyState));
    LogMsg("IAT GetKeyboardState: %d",    (int)PatchIAT(exe, "user32.dll", "GetKeyboardState",    (void *)hkGetKeyboardState,    (void **)&oGetKeyboardState));
    {
        static const char *xl[] = { "xinput1_4.dll", "xinput1_3.dll", "xinput9_1_0.dll", "xinput1_2.dll", "xinput1_1.dll" };
        size_t i;
        BOOL ok = FALSE;
        for (i = 0; i < sizeof(xl) / sizeof(xl[0]) && !ok; i++)
            ok = PatchIAT(exe, xl[i], "XInputGetState", (void *)hkXInputGetState, (void **)&oXInputGetState);
        LogMsg("IAT XInputGetState: %d", (int)ok);
    }
    LogImports(exe);

    for (;;) {
        {
            static BOOL lastOther = FALSE;
            BOOL o = ForegroundIsOtherApp();
            if (o != lastOther) { LogMsg("another app in front: %d", (int)o); lastOther = o; }
        }
        if (!g_hwnd || !IsWindow(g_hwnd)) {
            HWND found = NULL;
            g_hwnd = NULL;
            EnumWindows(EnumCb, (LPARAM)&found);
            if (found) { g_hwnd = found; LogMsg("game window found: %p", (void *)found); }
        }
        if (g_hwnd) EnsureSubclassed();
        if ((tick++ & 1) == 0) ReadConfig();
        Sleep(500);
    }
    return 0;
}

BOOL WINAPI DllMain(HINSTANCE inst, DWORD reason, LPVOID reserved)
{
    (void)reserved;
    if (reason == DLL_PROCESS_ATTACH) {
        HANDLE t;
        g_inst = inst;
        DisableThreadLibraryCalls(inst);
        t = CreateThread(NULL, 0, Worker, NULL, 0, NULL);
        if (t) CloseHandle(t);
    }
    return TRUE;
}
