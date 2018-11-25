#include "nvse/nvse/PluginAPI.h"
#include "nvse/nvse/nvse_version.h"
#include "nvse/nvse/SafeWrite.h"

void patchAimingChecks();
bool versionCheck(const NVSEInterface* nvse);

extern "C" {

	BOOL WINAPI DllMain(HANDLE hDllHandle, DWORD dwReason, LPVOID lpreserved) {
		return TRUE;
	}

	bool NVSEPlugin_Query(const NVSEInterface *nvse, PluginInfo *info) {
		/* fill out the info structure */
		info->infoVersion = PluginInfo::kInfoVersion;
		info->name = "No Aiming Slowdown";
		info->version = 1;
		return versionCheck(nvse);
	}

	bool NVSEPlugin_Load(const NVSEInterface *nvse) {
		patchAimingChecks();
		return true;
	}

};


void patchAimingChecks() {
	// When moving between left/right, front/back
	//0x492F39 - je 0x492F9B
	UInt32 isAimingCheckAddr = 0x492F39;
	SafeWriteBuf(isAimingCheckAddr, "\x90\x90", 2);

	// Forward/Sideward movement
	//0x896B91 - je 0x896C17
	isAimingCheckAddr = 0x896B91;
	SafeWriteBuf(isAimingCheckAddr, "\x90\x90\x90\x90\x90\x90", 6);

	// Diagonal movement
	//0x8A0B4B - je 0x8A0B92
	isAimingCheckAddr = 0x8A0B4B;
	SafeWriteBuf(isAimingCheckAddr, "\x90\x90", 2);
}



bool versionCheck(const NVSEInterface* nvse) {
	if (nvse->nvseVersion < NVSE_VERSION_INTEGER) {
		_ERROR("NVSE version too old (got %08X expected at least %08X)", nvse->nvseVersion, NVSE_VERSION_INTEGER);
		return false;
	}

	if (nvse->isEditor) return false;
	// version checks pass
	return true;
}
