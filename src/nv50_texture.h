#ifndef __NV50_TEXTURE_H__
#define __NV50_TEXTURE_H__

/* It'd be really nice to have these in nouveau_class.h generated by
 * renouveau like the rest of the object header - but not sure it can
 * handle non-object stuff nicely - need to look into it.
 */

/* Texture image control block */
#define NV50TIC_0_0_MAPA_MASK                                     0x38000000
#define NV50TIC_0_0_MAPA_ZERO                                     0x00000000
#define NV50TIC_0_0_MAPA_C0                                       0x10000000
#define NV50TIC_0_0_MAPA_C1                                       0x18000000
#define NV50TIC_0_0_MAPA_C2                                       0x20000000
#define NV50TIC_0_0_MAPA_C3                                       0x28000000
#define NV50TIC_0_0_MAPA_ONE                                      0x38000000
#define NV50TIC_0_0_MAPR_MASK                                     0x07000000
#define NV50TIC_0_0_MAPR_ZERO                                     0x00000000
#define NV50TIC_0_0_MAPR_C0                                       0x02000000
#define NV50TIC_0_0_MAPR_C1                                       0x03000000
#define NV50TIC_0_0_MAPR_C2                                       0x04000000
#define NV50TIC_0_0_MAPR_C3                                       0x05000000
#define NV50TIC_0_0_MAPR_ONE                                      0x07000000
#define NV50TIC_0_0_MAPG_MASK                                     0x00e00000
#define NV50TIC_0_0_MAPG_ZERO                                     0x00000000
#define NV50TIC_0_0_MAPG_C0                                       0x00400000
#define NV50TIC_0_0_MAPG_C1                                       0x00600000
#define NV50TIC_0_0_MAPG_C2                                       0x00800000
#define NV50TIC_0_0_MAPG_C3                                       0x00a00000
#define NV50TIC_0_0_MAPG_ONE                                      0x00e00000
#define NV50TIC_0_0_MAPB_MASK                                     0x001c0000
#define NV50TIC_0_0_MAPB_ZERO                                     0x00000000
#define NV50TIC_0_0_MAPB_C0                                       0x00080000
#define NV50TIC_0_0_MAPB_C1                                       0x000c0000
#define NV50TIC_0_0_MAPB_C2                                       0x00100000
#define NV50TIC_0_0_MAPB_C3                                       0x00140000
#define NV50TIC_0_0_MAPB_ONE                                      0x001c0000
#define NV50TIC_0_0_TYPEA_MASK                                    0x00038000
#define NV50TIC_0_0_TYPEA_UNORM                                   0x00010000
#define NV50TIC_0_0_TYPER_MASK                                    0x00007000
#define NV50TIC_0_0_TYPER_UNORM                                   0x00002000
#define NV50TIC_0_0_TYPEG_MASK                                    0x00000e00
#define NV50TIC_0_0_TYPEG_UNORM                                   0x00000400
#define NV50TIC_0_0_TYPEB_MASK                                    0x000001c0
#define NV50TIC_0_0_TYPEB_UNORM                                   0x00000080
#define NV50TIC_0_0_FMT_MASK                                      0x0000003c
#define NV50TIC_0_0_FMT_8_8_8_8                                   0x00000008
#define NV50TIC_0_0_FMT_2_10_10_10                                0x00000009
#define NV50TIC_0_0_FMT_5_6_5                                     0x00000015
#define NV50TIC_0_0_FMT_8_8                                       0x00000018
#define NV50TIC_0_0_FMT_8                                         0x0000001d

#define NV50TIC_0_1_OFFSET_LOW_MASK                               0xffffffff
#define NV50TIC_0_1_OFFSET_LOW_SHIFT                                       0

#define NV50TIC_0_2_UNKNOWN_MASK                                  0xffffffff

#define NV50TIC_0_3_UNKNOWN_MASK                                  0xffffffff

#define NV50TIC_0_4_WIDTH_MASK                                    0x0000ffff
#define NV50TIC_0_4_WIDTH_SHIFT                                            0

#define NV50TIC_0_5_DEPTH_MASK                                    0xffff0000
#define NV50TIC_0_5_DEPTH_SHIFT                                           16
#define NV50TIC_0_5_HEIGHT_MASK                                   0x0000ffff
#define NV50TIC_0_5_HEIGHT_SHIFT                                           0

#define NV50TIC_0_6_UNKNOWN_MASK                                  0xffffffff

#define NV50TIC_0_7_OFFSET_HIGH_MASK                              0xffffffff
#define NV50TIC_0_7_OFFSET_HIGH_SHIFT                                      0

/* Texture sampler control block */
#define NV50TSC_1_0_WRAPS_MASK                                   0x00000007
#define NV50TSC_1_0_WRAPS_REPEAT                                 0x00000000
#define NV50TSC_1_0_WRAPS_MIRROR_REPEAT                          0x00000001
#define NV50TSC_1_0_WRAPS_CLAMP_TO_EDGE                          0x00000002
#define NV50TSC_1_0_WRAPS_CLAMP_TO_BORDER                        0x00000003
#define NV50TSC_1_0_WRAPS_CLAMP                                  0x00000004
#define NV50TSC_1_0_WRAPS_MIRROR_CLAMP_TO_EDGE                   0x00000005
#define NV50TSC_1_0_WRAPS_MIRROR_CLAMP_TO_BORDER                 0x00000006
#define NV50TSC_1_0_WRAPS_MIRROR_CLAMP                           0x00000007
#define NV50TSC_1_0_WRAPT_MASK                                   0x00000038
#define NV50TSC_1_0_WRAPT_REPEAT                                 0x00000000
#define NV50TSC_1_0_WRAPT_MIRROR_REPEAT                          0x00000008
#define NV50TSC_1_0_WRAPT_CLAMP_TO_EDGE                          0x00000010
#define NV50TSC_1_0_WRAPT_CLAMP_TO_BORDER                        0x00000018
#define NV50TSC_1_0_WRAPT_CLAMP                                  0x00000020
#define NV50TSC_1_0_WRAPT_MIRROR_CLAMP_TO_EDGE                   0x00000028
#define NV50TSC_1_0_WRAPT_MIRROR_CLAMP_TO_BORDER                 0x00000030
#define NV50TSC_1_0_WRAPT_MIRROR_CLAMP                           0x00000038
#define NV50TSC_1_0_WRAPR_MASK                                   0x000001c0
#define NV50TSC_1_0_WRAPR_REPEAT                                 0x00000000
#define NV50TSC_1_0_WRAPR_MIRROR_REPEAT                          0x00000040
#define NV50TSC_1_0_WRAPR_CLAMP_TO_EDGE                          0x00000080
#define NV50TSC_1_0_WRAPR_CLAMP_TO_BORDER                        0x000000c0
#define NV50TSC_1_0_WRAPR_CLAMP                                  0x00000100
#define NV50TSC_1_0_WRAPR_MIRROR_CLAMP_TO_EDGE                   0x00000140
#define NV50TSC_1_0_WRAPR_MIRROR_CLAMP_TO_BORDER                 0x00000180
#define NV50TSC_1_0_WRAPR_MIRROR_CLAMP                           0x000001c0

#define NV50TSC_1_1_MAGF_MASK                                    0x00000003
#define NV50TSC_1_1_MAGF_NEAREST                                 0x00000001
#define NV50TSC_1_1_MAGF_LINEAR                                  0x00000002
#define NV50TSC_1_1_MINF_MASK                                    0x00000030
#define NV50TSC_1_1_MINF_NEAREST                                 0x00000010
#define NV50TSC_1_1_MINF_LINEAR                                  0x00000020
#define NV50TSC_1_1_MIPF_MASK                                    0x000000c0
#define NV50TSC_1_1_MIPF_NONE                                    0x00000040
#define NV50TSC_1_1_MIPF_NEAREST                                 0x00000080
#define NV50TSC_1_1_MIPF_LINEAR                                  0x000000c0

#define NV50TSC_1_2_UNKNOWN_MASK                                 0xffffffff

#define NV50TSC_1_3_UNKNOWN_MASK                                 0xffffffff

#define NV50TSC_1_4_UNKNOWN_MASK                                 0xffffffff

#define NV50TSC_1_5_UNKNOWN_MASK                                 0xffffffff

#define NV50TSC_1_6_UNKNOWN_MASK                                 0xffffffff

#define NV50TSC_1_7_UNKNOWN_MASK                                 0xffffffff

#endif
