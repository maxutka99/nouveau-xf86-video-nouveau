/*
 * Copyright © 2006 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Authors:
 *    Eric Anholt <eric@anholt.net>
 *
 */

#ifndef _NV_XF86MODES_H_
#define _NV_XF86MODES_H_
#include "xorgVersion.h"
#include "xf86Parser.h"

#if XORG_VERSION_CURRENT <= XORG_VERSION_NUMERIC(7,2,99,2,0)
double nv_xf86ModeHSync(DisplayModePtr mode);
double nv_xf86ModeVRefresh(DisplayModePtr mode);
DisplayModePtr nv_xf86DuplicateMode(DisplayModePtr pMode);
DisplayModePtr nv_xf86DuplicateModes(ScrnInfoPtr pScrn,
				       DisplayModePtr modeList);
void nv_xf86SetModeDefaultName(DisplayModePtr mode);
void nv_xf86SetModeCrtc(DisplayModePtr p, int adjustFlags);
Bool nv_xf86ModesEqual(DisplayModePtr pMode1, DisplayModePtr pMode2);
void nv_xf86PrintModeline(int scrnIndex,DisplayModePtr mode);
DisplayModePtr nv_xf86ModesAdd(DisplayModePtr modes, DisplayModePtr new);

#define xf86ModeHSync nv_xf86ModeHSync
#define xf86ModeVRefresh nv_xf86ModeVRefresh
#define xf86DuplicateMode nv_xf86DuplicateMode
#define xf86DuplicateModes nv_xf86DuplicateModes
#define xf86SetModeDefaultName nv_xf86SetModeDefaultName
#define xf86SetModeCrtc nv_xf86SetModeCrtc
#define xf86ModesEqual nv_xf86ModesEqual
#define xf86PrintModeline nv_xf86PrintModeline
#define xf86ModesAdd nv_xf86ModesAdd
#endif /* XORG_VERSION_CURRENT <= 7.2.99.2 */

void
nvxf86ValidateModesFlags(ScrnInfoPtr pScrn, DisplayModePtr modeList,
			    int flags);

void
nvxf86ValidateModesClocks(ScrnInfoPtr pScrn, DisplayModePtr modeList,
			    int *min, int *max, int n_ranges);

void
nvxf86ValidateModesSize(ScrnInfoPtr pScrn, DisplayModePtr modeList,
			  int maxX, int maxY, int maxPitch);

void
nvxf86ValidateModesSync(ScrnInfoPtr pScrn, DisplayModePtr modeList,
			  MonPtr mon);

void
nvxf86PruneInvalidModes(ScrnInfoPtr pScrn, DisplayModePtr *modeList,
			  Bool verbose);

void
nvxf86ValidateModesFlags(ScrnInfoPtr pScrn, DisplayModePtr modeList,
			    int flags);

void
nvxf86ValidateModesUserConfig(ScrnInfoPtr pScrn, DisplayModePtr modeList);

DisplayModePtr
nvxf86GetMonitorModes (ScrnInfoPtr pScrn, XF86ConfMonitorPtr conf_monitor);

DisplayModePtr
nvxf86GetDefaultModes (Bool interlaceAllowed, Bool doubleScanAllowed);

#endif /* _NV_XF86MODES_H_ */
