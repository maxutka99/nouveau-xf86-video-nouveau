/*
 * Copyright 2007 Maarten Maathuis
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
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "xf86.h"
#include "xf86_OSproc.h"
#include "xf86Resources.h"
#include "compiler.h"
#include "xf86PciInfo.h"
#include "xf86Pci.h"
#include "xf86fbman.h"
#include "regionstr.h"

#include "xf86xv.h"
#include <X11/extensions/Xv.h>
#include "exa.h"
#include "damage.h"
#include "dixstruct.h"
#include "fourcc.h"

#include "nv_include.h"
#include "nv_dma.h"

#include "nv_shaders.h"

static nv_shader_t nv40_video = {
	.card_priv.NV30VP.vp_in_reg  = 0x00000309,
	.card_priv.NV30VP.vp_out_reg = 0x0000c001,
	.size = (3*4),
	.data = {
		/* MOV result.position, vertex.position */
		0x40041c6c, 0x0040000d, 0x8106c083, 0x6041ff80,
		/* MOV result.texcoord[0], vertex.texcoord[0] */
		0x401f9c6c, 0x0040080d, 0x8106c083, 0x6041ff9c,
		/* MOV result.texcoord[1], vertex.texcoord[1] */
		0x401f9c6c, 0x0040090d, 0x8106c083, 0x6041ffa1,
	}
};

static nv_shader_t nv40_yv12 = {
	.card_priv.NV30FP.num_regs = 4,
	.size = (17*4),
	.data = {
		/* INST 0.0, TEX R1 (TR0.xyzw), attrib.texcoord[0] */
		0x17009e02, 0x1c9dc811, 0x0001c801, 0x0001c801,
		/* INST 1.0, TEX R2 (TR0.xyzw), attrib.texcoord[1] */
		0x1702be04, 0x1c9dc815, 0x0001c801, 0x0001c801,
		/* INST 2.0, DP4R R3.x (TR0.xyzw), R1, { 0.00, 0.00, 0.00, 1.00 } */
		0x06000206, 0x1c9dc804, 0x0001c802, 0x0001c801,
		/* const */
		0x00000000, 0x00000000, 0x00000000, 0x3f800000,
		/* INST 3.0, DP4R R3.y (TR0.xyzw), R2, { 0.00, 1.00, 0.00, 0.00 } */
		0x06000406, 0x1c9dc808, 0x0001c802, 0x0001c801,
		/* const */
		0x00000000, 0x3f800000, 0x00000000, 0x00000000,
		/* INST 4.0, DP4R R3.z (TR0.xyzw), R2, { 1.00, 0.00, 0.00, 0.00 } */
		0x06000806, 0x1c9dc808, 0x0001c802, 0x0001c801,
		/* const */
		0x3f800000, 0x00000000, 0x00000000, 0x00000000,
		/* INST 5.0, ADDR R3 (TR0.xyzw), R3, { 0.00, -0.50, -0.50, 0.00 } */
		0x03001e06, 0x1c9dc80c, 0x0001c802, 0x0001c801,
		/* const */
		0x00000000, 0xBF000000, 0xBF000000, 0x00000000,
		/* INST 6.0, DP3R R0.x (TR0.xyzw), R3, { 1.00, 0.00, 1.4022, 0.00 } */
		0x05000280, 0x1c9dc80c, 0x0001c802, 0x0001c801,
		/* const */
		0x3f800000, 0x00000000, 0x3FB37B4A, 0x00000000,
		/* INST 7.0, DP3R R0.y (TR0.xyzw), R3, { 1.00, -0.3457, -0.7145, 0.00 } */
		0x05000480, 0x1c9dc80c, 0x0001c802, 0x0001c801,
		/* const */
		0x3f800000, 0xBEB0FF97, 0xBF36E979, 0x00000000,
		/* INST 8.0, DP3R R0.z (TR0.xyzw), R3, { 1.00, 1.7710, 0.00, 0.00 } */
		0x05000880, 0x1c9dc80c, 0x0001c802, 0x0001c801,
		/* const */
		0x3f800000, 0x3FE2B021, 0x00000000, 0x00000000,
		/* INST 9.0, MOVR R0.w (TR0.xyzw), R3.wwww + END */
		0x01001081, 0x1c9dfe0c, 0x0001c801, 0x0001c801,
	}
};

#define SWIZZLE(ts0x,ts0y,ts0z,ts0w,ts1x,ts1y,ts1z,ts1w)							\
	(																	\
	NV40TCL_TEX_SWIZZLE_S0_X_##ts0x | NV40TCL_TEX_SWIZZLE_S0_Y_##ts0y		|	\
	NV40TCL_TEX_SWIZZLE_S0_Z_##ts0z | NV40TCL_TEX_SWIZZLE_S0_W_##ts0w	|	\
	NV40TCL_TEX_SWIZZLE_S1_X_##ts1x | NV40TCL_TEX_SWIZZLE_S1_Y_##ts1y 	|	\
	NV40TCL_TEX_SWIZZLE_S1_Z_##ts1z | NV40TCL_TEX_SWIZZLE_S1_W_##ts1w		\
	)

static Bool
NV40VideoTexture(ScrnInfoPtr pScrn, int offset, uint16_t width, uint16_t height, uint16_t src_pitch, int unit)
{
	NVPtr pNv = NVPTR(pScrn);

	uint32_t card_fmt = 0;
	uint32_t card_swz = 0;

	if (unit == 0) {
		/* Pretend we've got a normal 8 bits format. */
		card_fmt = NV40TCL_TEX_FORMAT_FORMAT_L8;
		card_swz = SWIZZLE(ZERO, ZERO, ZERO, S1, X, X, X, X);
	} else {
		/* Pretend we've got a normal 2x8 bits format. */
		card_fmt = NV40TCL_TEX_FORMAT_FORMAT_A8L8;
		card_swz = SWIZZLE(S1, S1, S1, S1, Y, X, W, Z); /* x = V, y = U */
	}

	BEGIN_RING(Nv3D, NV40TCL_TEX_OFFSET(unit), 8);
	/* We get an obsolute offset, which needs to be corrected. */
	OUT_RELOCl(pNv->FB, (uint32_t)(offset - pNv->FB->offset), NOUVEAU_BO_VRAM | NOUVEAU_BO_RD);
	OUT_RELOCd(pNv->FB, card_fmt | NV40TCL_TEX_FORMAT_LINEAR |
			NV40TCL_TEX_FORMAT_DIMS_2D | NV40TCL_TEX_FORMAT_NO_BORDER |
			(0x8000) | (1 << NV40TCL_TEX_FORMAT_MIPMAP_COUNT_SHIFT),
			NOUVEAU_BO_VRAM | NOUVEAU_BO_RD,
			NV40TCL_TEX_FORMAT_DMA0, 0);

	OUT_RING(NV40TCL_TEX_WRAP_S_CLAMP_TO_BORDER |
			NV40TCL_TEX_WRAP_T_CLAMP_TO_BORDER |
			NV40TCL_TEX_WRAP_R_CLAMP_TO_BORDER);
	OUT_RING(NV40TCL_TEX_ENABLE_ENABLE);
	OUT_RING(card_swz);
	if (unit == 0) {
		OUT_RING(NV40TCL_TEX_FILTER_MIN_LINEAR |
				NV40TCL_TEX_FILTER_MAG_LINEAR |
				0x3fd6);
	} else { /* UV texture cannot be linearly filtered, because it's just offsets. */
		OUT_RING(NV40TCL_TEX_FILTER_MIN_NEAREST |
				NV40TCL_TEX_FILTER_MAG_NEAREST |
				0x3fd6);
	}
	OUT_RING((width << 16) | height);
	OUT_RING(0); /* border ARGB */
	BEGIN_RING(Nv3D, NV40TCL_TEX_SIZE1(unit), 1);
	OUT_RING((1 << NV40TCL_TEX_SIZE1_DEPTH_SHIFT) |
			(uint16_t) src_pitch);

	return TRUE;
}

Bool
NV40GetSurfaceFormat(PixmapPtr pPix, int *fmt_ret)
{
	switch (pPix->drawable.bitsPerPixel) {
		case 32:
			*fmt_ret = NV40TCL_RT_FORMAT_COLOR_A8R8G8B8;
			break;
		case 24:
			*fmt_ret = NV40TCL_RT_FORMAT_COLOR_X8R8G8B8;
			break;
		case 16:
			*fmt_ret = NV40TCL_RT_FORMAT_COLOR_R5G6B5;
			break;
		case 8:
			*fmt_ret = NV40TCL_RT_FORMAT_COLOR_B8;
			break;
		default:
			return FALSE;
	}

	return TRUE;
}

#ifndef ExaOffscreenMarkUsed
extern void ExaOffscreenMarkUsed(PixmapPtr);
#endif
#ifndef exaGetDrawablePixmap
extern PixmapPtr exaGetDrawablePixmap(DrawablePtr);
#endif
#ifndef exaPixmapIsOffscreen
extern Bool exaPixmapIsOffscreen(PixmapPtr p);
#endif
/* To support EXA 2.0, 2.1 has this in the header */
#ifndef exaMoveInPixmap
extern void exaMoveInPixmap(PixmapPtr pPixmap);
#endif

#define SF(bf) (NV40TCL_BLEND_FUNC_SRC_RGB_##bf |                              \
		NV40TCL_BLEND_FUNC_SRC_ALPHA_##bf)
#define DF(bf) (NV40TCL_BLEND_FUNC_DST_RGB_##bf |                              \
		NV40TCL_BLEND_FUNC_DST_ALPHA_##bf)

#define VERTEX_OUT(sx,sy,dx,dy) do {                                        \
	BEGIN_RING(Nv3D, NV40TCL_VTX_ATTR_2F_X(8), 4);                         \
	OUT_RINGf ((sx)); OUT_RINGf ((sy));                                    \
	OUT_RINGf ((sx)); OUT_RINGf ((sy));                                    \
	BEGIN_RING(Nv3D, NV40TCL_VTX_ATTR_2I(0), 1);                           \
	OUT_RING  (((dy)<<16)|(dx));                                           \
} while(0)

void NV40PutTextureImage(ScrnInfoPtr pScrn, int src_offset,
		int src_offset2, int id,
		int src_pitch, BoxPtr dstBox,
		int x1, int y1, int x2, int y2,
		uint16_t width, uint16_t height,
		uint16_t src_w, uint16_t src_h,
		uint16_t drw_w, uint16_t drw_h,
		RegionPtr clipBoxes,
		DrawablePtr pDraw)
{
	/* Remove some warnings. */
	/* This has to be done better at some point. */
	(void)nv40_vp_exa_render;
	(void)nv30_fp_pass_col0;
	(void)nv30_fp_pass_tex0;
	(void)nv30_fp_composite_mask;
	(void)nv30_fp_composite_mask_sa_ca;
	(void)nv30_fp_composite_mask_ca;

	NVPtr pNv = NVPTR(pScrn);
	float X1, X2, Y1, Y2;
	float scaleX1, scaleX2, scaleY1, scaleY2;
	float scaleX, scaleY;
	ScreenPtr pScreen = pScrn->pScreen;
	PixmapPtr pPix = exaGetDrawablePixmap(pDraw);
	BoxPtr pbox;
	int nbox;
	int dst_format = 0;
	if (!NV40GetSurfaceFormat(pPix, &dst_format)) {
		ErrorF("No surface format, bad.\n");
	}

	/* Try to get the dest drawable into vram */
	if (!exaPixmapIsOffscreen(pPix)) {
		exaMoveInPixmap(pPix);
		ExaOffscreenMarkUsed(pPix);
	}

	/* If we failed, draw directly onto the screen pixmap.
	 * Not sure if this is the best approach, maybe failing
	 * with BadAlloc would be better?
	 */
	if (!exaPixmapIsOffscreen(pPix)) {
		xf86DrvMsg(pScrn->scrnIndex, X_ERROR,
			"XV: couldn't move dst surface into vram\n");
		pPix = pScreen->GetScreenPixmap(pScreen);
	}

#ifdef COMPOSITE
	/* Adjust coordinates if drawing to an offscreen pixmap */
	if (pPix->screen_x || pPix->screen_y) {
		REGION_TRANSLATE(pScrn->pScreen, clipBoxes,
							-pPix->screen_x,
							-pPix->screen_y);
		dstBox->x1 -= pPix->screen_x;
		dstBox->x2 -= pPix->screen_x;
		dstBox->y1 -= pPix->screen_y;
		dstBox->y2 -= pPix->screen_y;
	}

	DamageDamageRegion(pDraw, clipBoxes);
#endif

	pbox = REGION_RECTS(clipBoxes);
	nbox = REGION_NUM_RECTS(clipBoxes);

	/* Disable blending */
	BEGIN_RING(Nv3D, NV40TCL_BLEND_ENABLE, 1);
	OUT_RING(0);

	/* Setup surface */
	BEGIN_RING(Nv3D, NV40TCL_RT_FORMAT, 3);
	OUT_RING  (NV40TCL_RT_FORMAT_TYPE_LINEAR |
			NV40TCL_RT_FORMAT_ZETA_Z24S8 |
			dst_format);
	OUT_RING  (exaGetPixmapPitch(pPix));
	OUT_PIXMAPl(pPix, 0, NOUVEAU_BO_VRAM | NOUVEAU_BO_WR);

	NV40VideoTexture(pScrn, src_offset, src_w, src_h, src_pitch, 0);
	/* We've got NV12 format, which means half width and half height texture of chroma channels. */
	NV40VideoTexture(pScrn, src_offset2, src_w/2, src_h/2, src_pitch, 1);

	NV40_LoadVtxProg(pScrn, &nv40_video);
	NV40_LoadFragProg(pScrn, &nv40_yv12);

	/* Appears to be some kind of cache flush, needed here at least
	 * sometimes.. funky text rendering otherwise :)
	 */
	BEGIN_RING(Nv3D, NV40TCL_TEX_CACHE_CTL, 1);
	OUT_RING  (2);
	BEGIN_RING(Nv3D, NV40TCL_TEX_CACHE_CTL, 1);
	OUT_RING  (1);

	/* These are fixed point values in the 16.16 format. */
	x1 >>= 16;
	x2 >>= 16;
	y1 >>= 16;
	y2 >>= 16;

	X1 = (float)x1/(float)src_w;
	Y1 = (float)y1/(float)src_h;
	X2 = (float)x2/(float)src_w;
	Y2 = (float)y2/(float)src_h;

	/* The corrections here are emperical, i tried to explain them as best as possible. */

	/* This correction is need for when the image clips the screen at the right or bottom. */
	/* In this case x2 and/or y2 is adjusted for the clipping. */
	/* Otherwise the lower right coordinate stretches in the clipping direction. */
	scaleX = (float)src_w/(float)(x2 - x1);
	scaleY = (float)src_h/(float)(y2 - y1);

	while(nbox--) {
		BEGIN_RING(Nv3D, NV40TCL_BEGIN_END, 1);
		OUT_RING  (NV40TCL_BEGIN_END_QUADS);

		/* The src coordinates needs to be scaled to the draw size. */
		/* This happens when clipping the screen at the top and left. */
		/* In this case x1, x2, y1 and y2 are not adjusted for the clipping. */
		/* Otherwise the image stretches (in both directions tangential to the clipping). */
		scaleX1 = (float)(pbox->x1 - dstBox->x1)/(float)drw_w;
		scaleX2 = (float)(pbox->x2 - dstBox->x1)/(float)drw_w;
		scaleY1 = (float)(pbox->y1 - dstBox->y1)/(float)drw_h;
		scaleY2 = (float)(pbox->y2 - dstBox->y1)/(float)drw_h;

		/* Submit the appropriate vertices. */
		/* This submits the same vertices for the Y and the UV texture. */
		VERTEX_OUT(X1 + (X2 - X1) * scaleX1 * scaleX, Y1 + (Y2 - Y1) * scaleY1 * scaleY, pbox->x1, pbox->y1);
		VERTEX_OUT(X1 + (X2 - X1) * scaleX2 * scaleX, Y1 + (Y2 - Y1) * scaleY1 * scaleY, pbox->x2, pbox->y1);
		VERTEX_OUT(X1 + (X2 - X1) * scaleX2 * scaleX, Y1 + (Y2 - Y1) * scaleY2 * scaleY, pbox->x2, pbox->y2);
		VERTEX_OUT(X1 + (X2 - X1) * scaleX1 * scaleX, Y1 + (Y2 - Y1) * scaleY2 * scaleY, pbox->x1, pbox->y2);

		BEGIN_RING(Nv3D, NV40TCL_BEGIN_END, 1);
		OUT_RING  (NV40TCL_BEGIN_END_STOP);
		pbox++;
	}

	FIRE_RING();
}