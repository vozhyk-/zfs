/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License (the "License").
 * You may not use this file except in compliance with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */

/*
 * Copyright 2009 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */
/*
 * Copyright (c) 2013 by Saso Kiselkov. All rights reserved.
 */

/*
 * Copyright (c) 2013 by Delphix. All rights reserved.
 */

/*
 * Copyright (c) 2015 by Witaut Bajaryn. All rights reserved.
 */

#include <sys/zfs_context.h>
#include <sys/compress.h>
#include <sys/spa.h>
#include <sys/zfeature.h>
#include <sys/zio.h>
#include <sys/zio_compress.h>

/*
 * Compression vectors.
 */

zio_compress_info_t zio_compress_table[ZIO_COMPRESS_FUNCTIONS] = {
	{NULL,			BP_COMPRESS_INHERIT,	0,	"inherit"},
	{NULL,			BP_COMPRESS_ON,		0,	"on"},
	{NULL,			BP_COMPRESS_OFF,	0,	"uncompressed"},
	{lzjb_compress,		BP_COMPRESS_LZJB,	0,	"lzjb"},
	{NULL,			BP_COMPRESS_EMPTY,	0,	"empty"},
	{gzip_compress,		BP_COMPRESS_GZIP_1,	1,	"gzip-1"},
	{gzip_compress,		BP_COMPRESS_GZIP_2,	2,	"gzip-2"},
	{gzip_compress,		BP_COMPRESS_GZIP_3,	3,	"gzip-3"},
	{gzip_compress,		BP_COMPRESS_GZIP_4,	4,	"gzip-4"},
	{gzip_compress,		BP_COMPRESS_GZIP_5,	5,	"gzip-5"},
	{gzip_compress,		BP_COMPRESS_GZIP_6,	6,	"gzip-6"},
	{gzip_compress,		BP_COMPRESS_GZIP_7,	7,	"gzip-7"},
	{gzip_compress,		BP_COMPRESS_GZIP_8,	8,	"gzip-8"},
	{gzip_compress,		BP_COMPRESS_GZIP_9,	9,	"gzip-9"},
	{zle_compress,		BP_COMPRESS_ZLE,	64,	"zle"},
	{lz4_compress_zfs,	BP_COMPRESS_LZ4,	0,	"lz4"},
	{lz4hc_compress_zfs,	BP_COMPRESS_LZ4,	1,	"lz4hc-1"},
	{lz4hc_compress_zfs,	BP_COMPRESS_LZ4,	2,	"lz4hc-2"},
	{lz4hc_compress_zfs,	BP_COMPRESS_LZ4,	3,	"lz4hc-3"},
	{lz4hc_compress_zfs,	BP_COMPRESS_LZ4,	4,	"lz4hc-4"},
	{lz4hc_compress_zfs,	BP_COMPRESS_LZ4,	5,	"lz4hc-5"},
	{lz4hc_compress_zfs,	BP_COMPRESS_LZ4,	6,	"lz4hc-6"},
	{lz4hc_compress_zfs,	BP_COMPRESS_LZ4,	7,	"lz4hc-7"},
	{lz4hc_compress_zfs,	BP_COMPRESS_LZ4,	8,	"lz4hc-8"},
	{lz4hc_compress_zfs,	BP_COMPRESS_LZ4,	9,	"lz4hc-9"},
	{lz4hc_compress_zfs,	BP_COMPRESS_LZ4,	10,	"lz4hc-10"},
	{lz4hc_compress_zfs,	BP_COMPRESS_LZ4,	11,	"lz4hc-11"},
	{lz4hc_compress_zfs,	BP_COMPRESS_LZ4,	12,	"lz4hc-12"},
	{lz4hc_compress_zfs,	BP_COMPRESS_LZ4,	13,	"lz4hc-13"},
	{lz4hc_compress_zfs,	BP_COMPRESS_LZ4,	14,	"lz4hc-14"},
	{lz4hc_compress_zfs,	BP_COMPRESS_LZ4,	15,	"lz4hc-15"},
	{lz4hc_compress_zfs,	BP_COMPRESS_LZ4,	16,	"lz4hc-16"},
};

zio_decompress_info_t zio_decompress_table[BP_COMPRESS_VALUES] = {
	{NULL,			0,	"inherit"},
	{NULL,			0,	"on"},
	{NULL,			0,	"uncompressed"},
	{lzjb_decompress,	0,	"lzjb"},
	{NULL,			0,	"empty"},
	{gzip_decompress,	1,	"gzip-1"},
	{gzip_decompress,	2,	"gzip-2"},
	{gzip_decompress,	3,	"gzip-3"},
	{gzip_decompress,	4,	"gzip-4"},
	{gzip_decompress,	5,	"gzip-5"},
	{gzip_decompress,	6,	"gzip-6"},
	{gzip_decompress,	7,	"gzip-7"},
	{gzip_decompress,	8,	"gzip-8"},
	{gzip_decompress,	9,	"gzip-9"},
	{zle_decompress,	64,	"zle"},
	{lz4_decompress_zfs,	0,	"lz4"},
};

enum zio_compress
zio_compress_select(spa_t *spa, enum zio_compress child,
    enum zio_compress parent)
{
	enum zio_compress result;

	ASSERT(child < ZIO_COMPRESS_FUNCTIONS);
	ASSERT(parent < ZIO_COMPRESS_FUNCTIONS);
	ASSERT(parent != ZIO_COMPRESS_INHERIT);

	result = child;
	if (result == ZIO_COMPRESS_INHERIT)
		result = parent;

	if (result == ZIO_COMPRESS_ON) {
		if (spa_feature_is_active(spa, SPA_FEATURE_LZ4_COMPRESS))
			result = ZIO_COMPRESS_LZ4_ON_VALUE;
		else
			result = ZIO_COMPRESS_LEGACY_ON_VALUE;
	}

	return (result);
}

size_t
zio_compress_data(enum zio_compress c, void *src, void *dst, size_t s_len)
{
	uint64_t *word, *word_end;
	size_t c_len, d_len;
	zio_compress_info_t *ci = &zio_compress_table[c];

	ASSERT((uint_t)c < ZIO_COMPRESS_FUNCTIONS);
	ASSERT((uint_t)c == ZIO_COMPRESS_EMPTY || ci->ci_compress != NULL);

	/*
	 * If the data is all zeroes, we don't even need to allocate
	 * a block for it.  We indicate this by returning zero size.
	 */
	word_end = (uint64_t *)((char *)src + s_len);
	for (word = src; word < word_end; word++)
		if (*word != 0)
			break;

	if (word == word_end)
		return (0);

	if (c == ZIO_COMPRESS_EMPTY)
		return (s_len);

	/* Compress at least 12.5% */
	d_len = s_len - (s_len >> 3);
	c_len = ci->ci_compress(src, dst, s_len, d_len, ci->ci_level);

	if (c_len > d_len)
		return (s_len);

	ASSERT3U(c_len, <=, d_len);
	return (c_len);
}

int
zio_decompress_data(enum bp_compress c, void *src, void *dst,
    size_t s_len, size_t d_len)
{
	zio_decompress_info_t *di = &zio_decompress_table[c];

	if ((uint_t)c >= BP_COMPRESS_VALUES || di->di_decompress == NULL)
		return (SET_ERROR(EINVAL));

	return (di->di_decompress(src, dst, s_len, d_len, di->di_level));
}
