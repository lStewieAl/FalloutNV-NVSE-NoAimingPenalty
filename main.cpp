#include "nvse/nvse/PluginAPI.h"
#include "nvse/nvse/nvse_version.h"
#include "nvse/nvse/SafeWrite.h"

bool versionCheck(const NVSEInterface* nvse);
void handleIniOptions();
void backpedalHook();

HMODULE slowerBackpedalHandle;
float g_fMeleeAimSpeedMultiplier;
float g_fAimSpeedMultiplier;

extern "C" {

	BOOL WINAPI DllMain(HANDLE hDllHandle, DWORD dwReason, LPVOID lpreserved) {
		if (dwReason == DLL_PROCESS_ATTACH)
			slowerBackpedalHandle = (HMODULE)hDllHandle;
		return TRUE;
	}


	bool NVSEPlugin_Query(const NVSEInterface *nvse, PluginInfo *info) {
		/* fill out the info structure */
		info->infoVersion = PluginInfo::kInfoVersion;
		info->name = "Configurable Aim Penalty";
		info->version = 1;
		handleIniOptions();
		return versionCheck(nvse);
	}

	bool NVSEPlugin_Load(const NVSEInterface *nvse) {
		SafeWrite16(0x941A7D, 0x9090);
		SafeWrite16(0x941A82, 0x9090);

		SafeWriteBuf(0x941A9E, "\x90\x90\x90\x90\x90", 5);
		SafeWrite16(0x941AA8, 0x9090);

		WriteRelJump(0x941983, UInt32(backpedalHook));
		return true;
	}

};

void handleIniOptions() {
	char filename[MAX_PATH];
	GetModuleFileNameA(slowerBackpedalHandle, filename, MAX_PATH);
	strcpy((char *)(strrchr(filename, '\\') + 1), "nvse_aiming_speed.ini");

	g_fMeleeAimSpeedMultiplier = GetPrivateProfileIntA("Main", "iMeleeAimSpeedPercentage", 70, filename) / 100.0;
	g_fAimSpeedMultiplier = GetPrivateProfileIntA("Main", "iAimSpeedPercentage", 70, filename) / 100.0;

}

_declspec(naked) void backpedalHook() {
	static const UInt32 retnAddr = 0x94198B;
	_asm {

		/* melee aiming */
		mov     ecx, [ebp - 0x18]
		mov     edx, [ecx + 0x68]
		mov     eax, [ebp - 0x18]
		mov     ecx, [eax + 0x68]
		mov     edx, [edx]
		mov     eax, [edx + 0x3E4]
		call    eax
		cmp     eax, 7
		jz meleeSpeed

		/* non-melee aiming */
		mov     ecx, [ebp - 18h]
		mov eax, 0x8BBC10
		call    eax
		movzx   edx, al
		test    edx, edx
		jnz     aimSpeed

	moveNormal :
		fld1
		jmp doRetn

	meleeSpeed:
		fld g_fMeleeAimSpeedMultiplier
		jmp doRetn

	aimSpeed:
		fld g_fAimSpeedMultiplier
		
	doRetn :
		fstp dword ptr ds : [0x011A3B3C]
		jmp retnAddr
	}
}

bool versionCheck(const NVSEInterface* nvse) {
	if (nvse->isEditor) return false;
	if (nvse->nvseVersion < NVSE_VERSION_INTEGER) {
		_ERROR("NVSE version too old (got %08X expected at least %08X)", nvse->nvseVersion, NVSE_VERSION_INTEGER);
		return false;
	}
	return true;
}
