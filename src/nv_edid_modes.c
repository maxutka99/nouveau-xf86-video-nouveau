/*
 * Copyright 2006 Luc Verhaegen.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sub license,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/**
 * @file This is a copy of edid_modes.c from the X Server, for compatibility
 * with old X Servers.
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "xf86.h"
#include "xf86DDC.h"
#include <X11/Xatom.h>
#include "property.h"
#include "propertyst.h"
#include "xf86DDC.h"
#include "xorgVersion.h"
#include "nv_local.h"
#include <string.h>
#include <math.h>

#if XORG_VERSION_CURRENT <= XORG_VERSION_NUMERIC(7,2,99,2,0)

/* Compat definitions for older X Servers. */
#ifndef M_T_PREFERRED
#define M_T_PREFERRED   0x08
#endif
#ifndef M_T_DRIVER
#define M_T_DRIVER      0x40
#endif


/*
 * Quirks to work around broken EDID data from various monitors.
 */

typedef enum {
    DDC_QUIRK_NONE = 0,
    /* Force detailed sync polarity to -h +v */
    DDC_QUIRK_DT_SYNC_HM_VP = 1 << 0,
    /* First detailed mode is bogus, prefer largest mode at 60hz */
    DDC_QUIRK_PREFER_LARGE_60 = 1 << 1,
} ddc_quirk_t;

static Bool quirk_dt_sync_hm_vp (int scrnIndex, xf86MonPtr DDC)
{
    /* Belinea 1924S1W */
    if (memcmp (DDC->vendor.name, "MAX", 4) == 0 &&
	DDC->vendor.prod_id == 1932)
	return TRUE;
    /* Belinea 10 20 30W */
    if (memcmp (DDC->vendor.name, "MAX", 4) == 0 &&
	DDC->vendor.prod_id == 2007)
	return TRUE;
    
    return FALSE;
}

static Bool quirk_prefer_large_60 (int scrnIndex, xf86MonPtr DDC)
{
    /* Belinea 10 15 55 */
    if (memcmp (DDC->vendor.name, "MAX", 4) == 0 &&
	DDC->vendor.prod_id == 1516)
	return TRUE;
    
    return FALSE;
}

typedef struct {
    Bool	(*detect) (int scrnIndex, xf86MonPtr DDC);
    ddc_quirk_t	quirk;
    char	*description;
} ddc_quirk_map_t;

static const ddc_quirk_map_t ddc_quirks[] = {
    { 
	quirk_dt_sync_hm_vp,	DDC_QUIRK_DT_SYNC_HM_VP,
	"Set detailed timing sync polarity to -h +v"
    },
    {
	quirk_prefer_large_60,   DDC_QUIRK_PREFER_LARGE_60,
	"Detailed timing is not preferred, use largest mode at 60Hz"
    },
    { 
	NULL,		DDC_QUIRK_NONE,
	"No known quirks"
    },
};

/*
 * TODO:
 *  - for those with access to the VESA DMT standard; review please.
 */
#define MODEPREFIX(name) NULL, NULL, name, 0,M_T_DRIVER
#define MODESUFFIX   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,FALSE,FALSE,0,NULL,0,0.0,0.0

static DisplayModeRec DDCEstablishedModes[17] = {
    { MODEPREFIX("800x600"),    40000,  800,  840,  968, 1056, 0,  600,  601,  605,  628, 0, V_PHSYNC | V_PVSYNC, MODESUFFIX }, /* 800x600@60Hz */
    { MODEPREFIX("800x600"),    36000,  800,  824,  896, 1024, 0,  600,  601,  603,  625, 0, V_PHSYNC | V_PVSYNC, MODESUFFIX }, /* 800x600@56Hz */
    { MODEPREFIX("640x480"),    31500,  640,  656,  720,  840, 0,  480,  481,  484,  500, 0, V_NHSYNC | V_NVSYNC, MODESUFFIX }, /* 640x480@75Hz */
    { MODEPREFIX("640x480"),    31500,  640,  664,  704,  832, 0,  480,  489,  491,  520, 0, V_NHSYNC | V_NVSYNC, MODESUFFIX }, /* 640x480@72Hz */
    { MODEPREFIX("640x480"),    30240,  640,  704,  768,  864, 0,  480,  483,  486,  525, 0, V_NHSYNC | V_NVSYNC, MODESUFFIX }, /* 640x480@67Hz */
    { MODEPREFIX("640x480"),    25200,  640,  656,  752,  800, 0,  480,  490,  492,  525, 0, V_NHSYNC | V_NVSYNC, MODESUFFIX }, /* 640x480@60Hz */
    { MODEPREFIX("720x400"),    35500,  720,  738,  846,  900, 0,  400,  421,  423,  449, 0, V_NHSYNC | V_NVSYNC, MODESUFFIX }, /* 720x400@88Hz */
    { MODEPREFIX("720x400"),    28320,  720,  738,  846,  900, 0,  400,  412,  414,  449, 0, V_NHSYNC | V_PVSYNC, MODESUFFIX }, /* 720x400@70Hz */
    { MODEPREFIX("1280x1024"), 135000, 1280, 1296, 1440, 1688, 0, 1024, 1025, 1028, 1066, 0, V_PHSYNC | V_PVSYNC, MODESUFFIX }, /* 1280x1024@75Hz */
    { MODEPREFIX("1024x768"),   78800, 1024, 1040, 1136, 1312, 0,  768,  769,  772,  800, 0, V_PHSYNC | V_PVSYNC, MODESUFFIX }, /* 1024x768@75Hz */
    { MODEPREFIX("1024x768"),   75000, 1024, 1048, 1184, 1328, 0,  768,  771,  777,  806, 0, V_NHSYNC | V_NVSYNC, MODESUFFIX }, /* 1024x768@70Hz */
    { MODEPREFIX("1024x768"),   65000, 1024, 1048, 1184, 1344, 0,  768,  771,  777,  806, 0, V_NHSYNC | V_NVSYNC, MODESUFFIX }, /* 1024x768@60Hz */
    { MODEPREFIX("1024x768"),   44900, 1024, 1032, 1208, 1264, 0,  768,  768,  776,  817, 0, V_PHSYNC | V_PVSYNC | V_INTERLACE, MODESUFFIX }, /* 1024x768@43Hz */
    { MODEPREFIX("832x624"),    57284,  832,  864,  928, 1152, 0,  624,  625,  628,  667, 0, V_NHSYNC | V_NVSYNC, MODESUFFIX }, /* 832x624@75Hz */
    { MODEPREFIX("800x600"),    49500,  800,  816,  896, 1056, 0,  600,  601,  604,  625, 0, V_PHSYNC | V_PVSYNC, MODESUFFIX }, /* 800x600@75Hz */
    { MODEPREFIX("800x600"),    50000,  800,  856,  976, 1040, 0,  600,  637,  643,  666, 0, V_PHSYNC | V_PVSYNC, MODESUFFIX }, /* 800x600@72Hz */
    { MODEPREFIX("1152x864"),  108000, 1152, 1216, 1344, 1600, 0,  864,  865,  868,  900, 0, V_PHSYNC | V_PVSYNC, MODESUFFIX }, /* 1152x864@75Hz */
};

static DisplayModePtr
DDCModesFromEstablished(int scrnIndex, struct established_timings *timing,
			ddc_quirk_t quirks)
{
    DisplayModePtr Modes = NULL, Mode = NULL;
    CARD32 bits = (timing->t1) | (timing->t2 << 8) |
        ((timing->t_manu & 0x80) << 9);
    int i;

    for (i = 0; i < 17; i++) {
        if (bits & (0x01 << i)) {
            Mode = xf86DuplicateMode(&DDCEstablishedModes[i]);
            Modes = xf86ModesAdd(Modes, Mode);
        }
    }

    return Modes;
}

/*
 *
 */
static DisplayModePtr
DDCModesFromStandardTiming(int scrnIndex, struct std_timings *timing,
			   ddc_quirk_t quirks)
{
    DisplayModePtr Modes = NULL, Mode = NULL;
    int i;

    for (i = 0; i < STD_TIMINGS; i++) {
        if (timing[i].hsize && timing[i].vsize && timing[i].refresh) {
            Mode =  xf86CVTMode(timing[i].hsize, timing[i].vsize,
                                timing[i].refresh, FALSE, FALSE);
	    Mode->type = M_T_DRIVER;
            Modes = xf86ModesAdd(Modes, Mode);
        }
    }

    return Modes;
}

/*
 *
 */
static DisplayModePtr
DDCModeFromDetailedTiming(int scrnIndex, struct detailed_timings *timing,
			  int preferred, ddc_quirk_t quirks)
{
    DisplayModePtr Mode;

    /* We don't do stereo */
    if (timing->stereo) {
        xf86DrvMsg(scrnIndex, X_INFO,
		   "%s: Ignoring: We don't handle stereo.\n", __func__);
        return NULL;
    }

    /* We only do seperate sync currently */
    if (timing->sync != 0x03) {
         xf86DrvMsg(scrnIndex, X_INFO,
		    "%s: %dx%d Warning: We only handle seperate"
                    " sync.\n", __func__, timing->h_active, timing->v_active);
    }

    Mode = xnfalloc(sizeof(DisplayModeRec));
    memset(Mode, 0, sizeof(DisplayModeRec));

    Mode->type = M_T_DRIVER;
    if (preferred)
	Mode->type |= M_T_PREFERRED;

    Mode->Clock = timing->clock / 1000.0;

    Mode->HDisplay = timing->h_active;
    Mode->HSyncStart = timing->h_active + timing->h_sync_off;
    Mode->HSyncEnd = Mode->HSyncStart + timing->h_sync_width;
    Mode->HTotal = timing->h_active + timing->h_blanking;

    Mode->VDisplay = timing->v_active;
    Mode->VSyncStart = timing->v_active + timing->v_sync_off;
    Mode->VSyncEnd = Mode->VSyncStart + timing->v_sync_width;
    Mode->VTotal = timing->v_active + timing->v_blanking;

    xf86SetModeDefaultName(Mode);

    /* We ignore h/v_size and h/v_border for now. */

    if (timing->interlaced)
        Mode->Flags |= V_INTERLACE;

    if (quirks & DDC_QUIRK_DT_SYNC_HM_VP)
	Mode->Flags |= V_NHSYNC | V_PVSYNC;
    else
    {
	if (timing->misc & 0x02)
	    Mode->Flags |= V_PHSYNC;
	else
	    Mode->Flags |= V_NHSYNC;
    
	if (timing->misc & 0x01)
	    Mode->Flags |= V_PVSYNC;
	else
	    Mode->Flags |= V_NVSYNC;
    }

    return Mode;
}

DisplayModePtr
xf86DDCGetModes(int scrnIndex, xf86MonPtr DDC)
{
    int preferred, i;
    DisplayModePtr  Modes = NULL, Mode;
    ddc_quirk_t	    quirks;

    xf86DrvMsg (scrnIndex, X_INFO, "EDID vendor \"%s\", prod id %d\n",
		DDC->vendor.name, DDC->vendor.prod_id);
    quirks = DDC_QUIRK_NONE;
    for (i = 0; ddc_quirks[i].detect; i++)
	if (ddc_quirks[i].detect (scrnIndex, DDC))
	{
	    xf86DrvMsg (scrnIndex, X_INFO, "    EDID quirk: %s\n",
			ddc_quirks[i].description);
	    quirks |= ddc_quirks[i].quirk;
	}
    
    preferred = PREFERRED_TIMING_MODE(DDC->features.msc);
    if (quirks & DDC_QUIRK_PREFER_LARGE_60)
	preferred = 0;

    for (i = 0; i < DET_TIMINGS; i++) {
	struct detailed_monitor_section *det_mon = &DDC->det_mon[i];

        switch (det_mon->type) {
        case DT:
            Mode = DDCModeFromDetailedTiming(scrnIndex,
                                             &det_mon->section.d_timings,
					     preferred,
					     quirks);
	    preferred = 0;
            Modes = xf86ModesAdd(Modes, Mode);
            break;
        case DS_STD_TIMINGS:
            Mode = DDCModesFromStandardTiming(scrnIndex,
					      det_mon->section.std_t,
					      quirks);
            Modes = xf86ModesAdd(Modes, Mode);
            break;
        default:
            break;
        }
    }

    /* Add established timings */
    Mode = DDCModesFromEstablished(scrnIndex, &DDC->timings1, quirks);
    Modes = xf86ModesAdd(Modes, Mode);

    /* Add standard timings */
    Mode = DDCModesFromStandardTiming(scrnIndex, DDC->timings2, quirks);
    Modes = xf86ModesAdd(Modes, Mode);

    if (quirks & DDC_QUIRK_PREFER_LARGE_60)
    {
	DisplayModePtr	best = Modes;
	for (Mode = Modes; Mode; Mode = Mode->next)
	{
	    if (Mode == best) continue;
	    if (Mode->HDisplay * Mode->VDisplay > best->HDisplay * best->VDisplay)
	    {
		best = Mode;
		continue;
	    }
	    if (Mode->HDisplay * Mode->VDisplay == best->HDisplay * best->VDisplay)
	    {
		double	mode_refresh = xf86ModeVRefresh (Mode);
		double	best_refresh = xf86ModeVRefresh (best);
		double	mode_dist = fabs(mode_refresh - 60.0);
		double	best_dist = fabs(best_refresh - 60.0);
		if (mode_dist < best_dist)
		{
		    best = Mode;
		    continue;
		}
	    }
	}
	if (best)
	    best->type |= M_T_PREFERRED;
    }
    return Modes;
}

#endif /* XORG_VERSION_CURRENT <= XORG_VERSION_NUMERIC(7,2,99,2,0) */
