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
  // Set the NPC flag on the AND, so it always evaluates false for the player

  // When moving between left/right, front/back
  //0x492F33 - and edx,00000240
  UInt32 isAimingCheckAddrAnd = 0x492F33;
  SafeWriteBuf(isAimingCheckAddrAnd, "\x81\xE2\x40\x02\x00\x00", 6);

  // Forward/Sideward movement
  //0x896B8C - and eax,00000200
  isAimingCheckAddrAnd = 0x896B8C;
  SafeWriteBuf(isAimingCheckAddrAnd, "\x25\x40\x02\x00\x00", 5);

  // Diagonal movement
  //0x8A0B46 - and eax,00000200
  isAimingCheckAddrAnd = 0x8A0B46;
  SafeWriteBuf(isAimingCheckAddrAnd, "\x25\x40\x02\x00\x00", 5);
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
