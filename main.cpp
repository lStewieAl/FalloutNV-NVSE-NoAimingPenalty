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
	UInt32 isMeleeAimingAddr = 0x941A82;
	UInt32 isWeaponAimingAddr = 0x941AA8;

	SafeWrite16(isMeleeAimingAddr, 0x9090);
	SafeWrite16(isWeaponAimingAddr, 0x9090);
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
