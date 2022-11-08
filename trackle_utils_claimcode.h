#ifndef TRACKLE_UTILS_CLAIMCODE_H
#define TRACKLE_UTILS_CLAIMCODE_H

#include <esp_types.h>
#include <inttypes.h>

#define CLAIM_CODE_LENGTH 63

void Trackle_saveClaimCode(const char *claimCode);
void Trackle_loadClaimCode();

#endif
