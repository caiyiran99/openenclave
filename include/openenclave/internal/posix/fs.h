// Copyright (c) Microsoft Corporation. All rights reserved._ops
// Licensed under the MIT License.

#ifndef _OE_INTERNAL_FS_H
#define _OE_INTERNAL_FS_H

#include <openenclave/bits/defs.h>
#include <openenclave/bits/types.h>
#include <openenclave/corelibc/limits.h>
#include <openenclave/internal/posix/device.h>

OE_EXTERNC_BEGIN

/* Get the access mode from the open() flags. */
OE_INLINE int oe_get_open_access_mode(int flags)
{
    return (flags & 000000003);
}

/* The enclave calls this to get an instance of host file system (SGXFS). */
oe_device_t* oe_get_sgxfs_device(void);

/* Initialize the stdin, stdout, and stderr devices. */
int oe_initialize_console_devices(void);

/* Use mounter to resolve this path to a target path. */
oe_device_t* oe_mount_resolve(const char* path, char suffix[OE_PATH_MAX]);

OE_EXTERNC_END

#endif // _OE_INTERNAL_FS_H