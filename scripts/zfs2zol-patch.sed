#!/bin/sed -f

s:usr/src/uts/common/fs/zfs/sys:include/sys:g
s:usr/src/uts/common/fs/zfs:module/zfs:g
s:usr/src/lib/libzpool:lib/libzpool:g
s:usr/src/cmd:cmd:g
s:usr/src/common/nvpair:module/nvpair:g
s:usr/src/common/zfs/zfeature_common.c:module/zfs/zfeature_common.c:g
s:usr/src/common/zfs/zfs_prop.c:module/zcommon/zfs_prop.c:g
s:usr/src/common/zfs/zfeature_common.h:include/zfeature_common.h:g
s:usr/src/lib/libzfs/common/libzfs.h:include/libzfs.h:g
s:usr/src/man/man1m/zfs.1m:man/man8/zfs.8:g
s:usr/src/man/man5/zpool-features.5:man/man5/zpool-features.5:g
s:usr/src/uts/common/sys:include/sys:g
s:usr/src/lib/libzfs_core/common/libzfs_core.h:include/libzfs_core.h:g
s:usr/src/lib/libzfs/common:lib/libzfs:g
s:usr/src/lib/libzfs_core/common:lib/libzfs_core:g
s:lib/libzpool/common/sys:include/sys:g
s:lib/libzpool/common:lib/libzpool:g
