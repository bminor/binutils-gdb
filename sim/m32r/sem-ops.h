/* Semantics ops support for CGEN-based simulators.

This file is machine generated.

Copyright (C) 1996, 1997 Free Software Foundation, Inc.

This file is part of the GNU Binutils and/or GDB, the GNU debugger.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

*/

#ifndef CGEN_SEM_OPS_H
#define CGEN_SEM_OPS_H

/* Semantic operations.  */

#define ADDBI(x, y) ((x) + (y))
#define SUBBI(x, y) ((x) - (y))
#define MULBI(x, y) ((x) * (y))
#define DIVBI(x, y) ((BI) (x) / (BI) (y))
#define UDIVBI(x, y) ((UBI) (x) / (UBI) (y))
#define MODBI(x, y) ((BI) (x) % (BI) (y))
#define UMODBI(x, y) ((UBI) (x) % (UBI) (y))
#define SRABI(x, y) ((BI) (x) >> (y))
#define SRLBI(x, y) ((UBI) (x) >> (y))
#define SHLBI(x, y) ((UBI) (x) << (y))
extern BI RORBI PARAMS ((BI, int));
extern BI ROLBI PARAMS ((BI, int));
#define ANDBI(x, y) ((x) & (y))
#define ORBI(x, y) ((x) | (y))
#define XORBI(x, y) ((x) ^ (y))
#define ANDIFBI(x, y) ((BI) (x) && (BI) (y))
#define ORIFBI(x, y) ((BI) (x) || (BI) (y))
#define NEGBI(x) (- (x))
#define NOTBI(x) (! (BI) (x))
#define INVBI(x) (~ (x))
#define EQBI(x, y) ((BI) (x) == (BI) (y))
#define NEBI(x, y) ((BI) (x) != (BI) (y))
#define LTBI(x, y) ((BI) (x) < (BI) (y))
#define LEBI(x, y) ((BI) (x) <= (BI) (y))
#define GTBI(x, y) ((BI) (x) > (BI) (y))
#define GEBI(x, y) ((BI) (x) >= (BI) (y))
#define LTUBI(x, y) ((UBI) (x) < (UBI) (y))
#define LEUBI(x, y) ((UBI) (x) <= (UBI) (y))
#define GTUBI(x, y) ((UBI) (x) > (UBI) (y))
#define GEUBI(x, y) ((UBI) (x) >= (UBI) (y))

#define ADDQI(x, y) ((x) + (y))
#define SUBQI(x, y) ((x) - (y))
#define MULQI(x, y) ((x) * (y))
#define DIVQI(x, y) ((QI) (x) / (QI) (y))
#define UDIVQI(x, y) ((UQI) (x) / (UQI) (y))
#define MODQI(x, y) ((QI) (x) % (QI) (y))
#define UMODQI(x, y) ((UQI) (x) % (UQI) (y))
#define SRAQI(x, y) ((QI) (x) >> (y))
#define SRLQI(x, y) ((UQI) (x) >> (y))
#define SHLQI(x, y) ((UQI) (x) << (y))
extern QI RORQI PARAMS ((QI, int));
extern QI ROLQI PARAMS ((QI, int));
#define ANDQI(x, y) ((x) & (y))
#define ORQI(x, y) ((x) | (y))
#define XORQI(x, y) ((x) ^ (y))
#define ANDIFQI(x, y) ((QI) (x) && (QI) (y))
#define ORIFQI(x, y) ((QI) (x) || (QI) (y))
#define NEGQI(x) (- (x))
#define NOTQI(x) (! (QI) (x))
#define INVQI(x) (~ (x))
#define EQQI(x, y) ((QI) (x) == (QI) (y))
#define NEQI(x, y) ((QI) (x) != (QI) (y))
#define LTQI(x, y) ((QI) (x) < (QI) (y))
#define LEQI(x, y) ((QI) (x) <= (QI) (y))
#define GTQI(x, y) ((QI) (x) > (QI) (y))
#define GEQI(x, y) ((QI) (x) >= (QI) (y))
#define LTUQI(x, y) ((UQI) (x) < (UQI) (y))
#define LEUQI(x, y) ((UQI) (x) <= (UQI) (y))
#define GTUQI(x, y) ((UQI) (x) > (UQI) (y))
#define GEUQI(x, y) ((UQI) (x) >= (UQI) (y))

#define ADDHI(x, y) ((x) + (y))
#define SUBHI(x, y) ((x) - (y))
#define MULHI(x, y) ((x) * (y))
#define DIVHI(x, y) ((HI) (x) / (HI) (y))
#define UDIVHI(x, y) ((UHI) (x) / (UHI) (y))
#define MODHI(x, y) ((HI) (x) % (HI) (y))
#define UMODHI(x, y) ((UHI) (x) % (UHI) (y))
#define SRAHI(x, y) ((HI) (x) >> (y))
#define SRLHI(x, y) ((UHI) (x) >> (y))
#define SHLHI(x, y) ((UHI) (x) << (y))
extern HI RORHI PARAMS ((HI, int));
extern HI ROLHI PARAMS ((HI, int));
#define ANDHI(x, y) ((x) & (y))
#define ORHI(x, y) ((x) | (y))
#define XORHI(x, y) ((x) ^ (y))
#define ANDIFHI(x, y) ((HI) (x) && (HI) (y))
#define ORIFHI(x, y) ((HI) (x) || (HI) (y))
#define NEGHI(x) (- (x))
#define NOTHI(x) (! (HI) (x))
#define INVHI(x) (~ (x))
#define EQHI(x, y) ((HI) (x) == (HI) (y))
#define NEHI(x, y) ((HI) (x) != (HI) (y))
#define LTHI(x, y) ((HI) (x) < (HI) (y))
#define LEHI(x, y) ((HI) (x) <= (HI) (y))
#define GTHI(x, y) ((HI) (x) > (HI) (y))
#define GEHI(x, y) ((HI) (x) >= (HI) (y))
#define LTUHI(x, y) ((UHI) (x) < (UHI) (y))
#define LEUHI(x, y) ((UHI) (x) <= (UHI) (y))
#define GTUHI(x, y) ((UHI) (x) > (UHI) (y))
#define GEUHI(x, y) ((UHI) (x) >= (UHI) (y))

#define ADDSI(x, y) ((x) + (y))
#define SUBSI(x, y) ((x) - (y))
#define MULSI(x, y) ((x) * (y))
#define DIVSI(x, y) ((SI) (x) / (SI) (y))
#define UDIVSI(x, y) ((USI) (x) / (USI) (y))
#define MODSI(x, y) ((SI) (x) % (SI) (y))
#define UMODSI(x, y) ((USI) (x) % (USI) (y))
#define SRASI(x, y) ((SI) (x) >> (y))
#define SRLSI(x, y) ((USI) (x) >> (y))
#define SHLSI(x, y) ((USI) (x) << (y))
extern SI RORSI PARAMS ((SI, int));
extern SI ROLSI PARAMS ((SI, int));
#define ANDSI(x, y) ((x) & (y))
#define ORSI(x, y) ((x) | (y))
#define XORSI(x, y) ((x) ^ (y))
#define ANDIFSI(x, y) ((SI) (x) && (SI) (y))
#define ORIFSI(x, y) ((SI) (x) || (SI) (y))
#define NEGSI(x) (- (x))
#define NOTSI(x) (! (SI) (x))
#define INVSI(x) (~ (x))
#define EQSI(x, y) ((SI) (x) == (SI) (y))
#define NESI(x, y) ((SI) (x) != (SI) (y))
#define LTSI(x, y) ((SI) (x) < (SI) (y))
#define LESI(x, y) ((SI) (x) <= (SI) (y))
#define GTSI(x, y) ((SI) (x) > (SI) (y))
#define GESI(x, y) ((SI) (x) >= (SI) (y))
#define LTUSI(x, y) ((USI) (x) < (USI) (y))
#define LEUSI(x, y) ((USI) (x) <= (USI) (y))
#define GTUSI(x, y) ((USI) (x) > (USI) (y))
#define GEUSI(x, y) ((USI) (x) >= (USI) (y))

#ifdef DI_FN_SUPPORT
extern DI ADDDI PARAMS ((DI, DI));
extern DI SUBDI PARAMS ((DI, DI));
extern DI MULDI PARAMS ((DI, DI));
extern DI DIVDI PARAMS ((DI, DI));
extern UDI UDIVDI PARAMS ((UDI, UDI));
extern DI MODDI PARAMS ((DI, DI));
extern UDI UMODDI PARAMS ((UDI, UDI));
extern DI SRADI PARAMS ((DI, int));
extern UDI SRLDI PARAMS ((UDI, int));
extern UDI SHLDI PARAMS ((UDI, int));
extern DI RORDI PARAMS ((DI, int));
extern DI ROLDI PARAMS ((DI, int));
extern DI ANDDI PARAMS ((DI, DI));
extern DI ORDI PARAMS ((DI, DI));
extern DI XORDI PARAMS ((DI, DI));
extern int ANDIFDI PARAMS ((DI, DI));
extern int ORIFDI PARAMS ((DI, DI));
extern DI NEGDI PARAMS ((DI));
extern int NOTDI PARAMS ((DI));
extern DI INVDI PARAMS ((DI));
extern int EQDI PARAMS ((DI, DI));
extern int NEDI PARAMS ((DI, DI));
extern int LTDI PARAMS ((DI, DI));
extern int LEDI PARAMS ((DI, DI));
extern int GTDI PARAMS ((DI, DI));
extern int GEDI PARAMS ((DI, DI));
extern int LTUDI PARAMS ((UDI, UDI));
extern int LEUDI PARAMS ((UDI, UDI));
extern int GTUDI PARAMS ((UDI, UDI));
extern int GEUDI PARAMS ((UDI, UDI));
#else /* ! DI_FN_SUPPORT */
#define ADDDI(x, y) ((x) + (y))
#define SUBDI(x, y) ((x) - (y))
#define MULDI(x, y) ((x) * (y))
#define DIVDI(x, y) ((DI) (x) / (DI) (y))
#define UDIVDI(x, y) ((UDI) (x) / (UDI) (y))
#define MODDI(x, y) ((DI) (x) % (DI) (y))
#define UMODDI(x, y) ((UDI) (x) % (UDI) (y))
#define SRADI(x, y) ((DI) (x) >> (y))
#define SRLDI(x, y) ((UDI) (x) >> (y))
#define SHLDI(x, y) ((UDI) (x) << (y))
extern DI RORDI PARAMS ((DI, int));
extern DI ROLDI PARAMS ((DI, int));
#define ANDDI(x, y) ((x) & (y))
#define ORDI(x, y) ((x) | (y))
#define XORDI(x, y) ((x) ^ (y))
#define ANDIFDI(x, y) ((DI) (x) && (DI) (y))
#define ORIFDI(x, y) ((DI) (x) || (DI) (y))
#define NEGDI(x) (- (x))
#define NOTDI(x) (! (DI) (x))
#define INVDI(x) (~ (x))
#define EQDI(x, y) ((DI) (x) == (DI) (y))
#define NEDI(x, y) ((DI) (x) != (DI) (y))
#define LTDI(x, y) ((DI) (x) < (DI) (y))
#define LEDI(x, y) ((DI) (x) <= (DI) (y))
#define GTDI(x, y) ((DI) (x) > (DI) (y))
#define GEDI(x, y) ((DI) (x) >= (DI) (y))
#define LTUDI(x, y) ((UDI) (x) < (UDI) (y))
#define LEUDI(x, y) ((UDI) (x) <= (UDI) (y))
#define GTUDI(x, y) ((UDI) (x) > (UDI) (y))
#define GEUDI(x, y) ((UDI) (x) >= (UDI) (y))
#endif /* DI_FN_SUPPORT */

#ifdef SF_FN_SUPPORT
extern SF ADDSF PARAMS ((SF, SF));
extern SF SUBSF PARAMS ((SF, SF));
extern SF NEGSF PARAMS ((SF));
extern SF MULSF PARAMS ((SF, SF));
extern SF DIVSF PARAMS ((SF, SF));
extern int EQSF PARAMS ((SF, SF));
extern int NESF PARAMS ((SF, SF));
extern int LTSF PARAMS ((SF, SF));
extern int LESF PARAMS ((SF, SF));
extern int GTSF PARAMS ((SF, SF));
extern int GESF PARAMS ((SF, SF));
extern SF ABSSF PARAMS ((SF));
extern SF SQRTSF PARAMS ((SF));
extern SF COSSF PARAMS ((SF));
extern SF SINSF PARAMS ((SF));
#else /* ! SF_FN_SUPPORT */
#define ADDSF(x, y) ((x) + (y))
#define SUBSF(x, y) ((x) - (y))
#define NEGSF(x) (- (x))
#define MULSF(x, y) ((x) * (y))
#define DIVSF(x, y) ((x) / (y))
#define EQSF(x, y) ((SF) (x) == (SF) (y))
#define NESF(x, y) ((SF) (x) != (SF) (y))
#define LTSF(x, y) ((SF) (x) < (SF) (y))
#define LESF(x, y) ((SF) (x) <= (SF) (y))
#define GTSF(x, y) ((SF) (x) > (SF) (y))
#define GESF(x, y) ((SF) (x) >= (SF) (y))
extern SF ABSSF PARAMS ((SF));
extern SF SQRTSF PARAMS ((SF));
extern SF COSSF PARAMS ((SF));
extern SF SINSF PARAMS ((SF));
#endif /* SF_FN_SUPPORT */

#ifdef DF_FN_SUPPORT
extern DF ADDDF PARAMS ((DF, DF));
extern DF SUBDF PARAMS ((DF, DF));
extern DF NEGDF PARAMS ((DF));
extern DF MULDF PARAMS ((DF, DF));
extern DF DIVDF PARAMS ((DF, DF));
extern int EQDF PARAMS ((DF, DF));
extern int NEDF PARAMS ((DF, DF));
extern int LTDF PARAMS ((DF, DF));
extern int LEDF PARAMS ((DF, DF));
extern int GTDF PARAMS ((DF, DF));
extern int GEDF PARAMS ((DF, DF));
extern DF ABSDF PARAMS ((DF));
extern DF SQRTDF PARAMS ((DF));
extern DF COSDF PARAMS ((DF));
extern DF SINDF PARAMS ((DF));
#else /* ! DF_FN_SUPPORT */
#define ADDDF(x, y) ((x) + (y))
#define SUBDF(x, y) ((x) - (y))
#define NEGDF(x) (- (x))
#define MULDF(x, y) ((x) * (y))
#define DIVDF(x, y) ((x) / (y))
#define EQDF(x, y) ((DF) (x) == (DF) (y))
#define NEDF(x, y) ((DF) (x) != (DF) (y))
#define LTDF(x, y) ((DF) (x) < (DF) (y))
#define LEDF(x, y) ((DF) (x) <= (DF) (y))
#define GTDF(x, y) ((DF) (x) > (DF) (y))
#define GEDF(x, y) ((DF) (x) >= (DF) (y))
extern DF ABSDF PARAMS ((DF));
extern DF SQRTDF PARAMS ((DF));
extern DF COSDF PARAMS ((DF));
extern DF SINDF PARAMS ((DF));
#endif /* DF_FN_SUPPORT */

#ifdef XF_FN_SUPPORT
extern XF ADDXF PARAMS ((XF, XF));
extern XF SUBXF PARAMS ((XF, XF));
extern XF NEGXF PARAMS ((XF));
extern XF MULXF PARAMS ((XF, XF));
extern XF DIVXF PARAMS ((XF, XF));
extern int EQXF PARAMS ((XF, XF));
extern int NEXF PARAMS ((XF, XF));
extern int LTXF PARAMS ((XF, XF));
extern int LEXF PARAMS ((XF, XF));
extern int GTXF PARAMS ((XF, XF));
extern int GEXF PARAMS ((XF, XF));
extern XF ABSXF PARAMS ((XF));
extern XF SQRTXF PARAMS ((XF));
extern XF COSXF PARAMS ((XF));
extern XF SINXF PARAMS ((XF));
#else /* ! XF_FN_SUPPORT */
#define ADDXF(x, y) ((x) + (y))
#define SUBXF(x, y) ((x) - (y))
#define NEGXF(x) (- (x))
#define MULXF(x, y) ((x) * (y))
#define DIVXF(x, y) ((x) / (y))
#define EQXF(x, y) ((XF) (x) == (XF) (y))
#define NEXF(x, y) ((XF) (x) != (XF) (y))
#define LTXF(x, y) ((XF) (x) < (XF) (y))
#define LEXF(x, y) ((XF) (x) <= (XF) (y))
#define GTXF(x, y) ((XF) (x) > (XF) (y))
#define GEXF(x, y) ((XF) (x) >= (XF) (y))
extern XF ABSXF PARAMS ((XF));
extern XF SQRTXF PARAMS ((XF));
extern XF COSXF PARAMS ((XF));
extern XF SINXF PARAMS ((XF));
#endif /* XF_FN_SUPPORT */

#ifdef TF_FN_SUPPORT
extern TF ADDTF PARAMS ((TF, TF));
extern TF SUBTF PARAMS ((TF, TF));
extern TF NEGTF PARAMS ((TF));
extern TF MULTF PARAMS ((TF, TF));
extern TF DIVTF PARAMS ((TF, TF));
extern int EQTF PARAMS ((TF, TF));
extern int NETF PARAMS ((TF, TF));
extern int LTTF PARAMS ((TF, TF));
extern int LETF PARAMS ((TF, TF));
extern int GTTF PARAMS ((TF, TF));
extern int GETF PARAMS ((TF, TF));
extern TF ABSTF PARAMS ((TF));
extern TF SQRTTF PARAMS ((TF));
extern TF COSTF PARAMS ((TF));
extern TF SINTF PARAMS ((TF));
#else /* ! TF_FN_SUPPORT */
#define ADDTF(x, y) ((x) + (y))
#define SUBTF(x, y) ((x) - (y))
#define NEGTF(x) (- (x))
#define MULTF(x, y) ((x) * (y))
#define DIVTF(x, y) ((x) / (y))
#define EQTF(x, y) ((TF) (x) == (TF) (y))
#define NETF(x, y) ((TF) (x) != (TF) (y))
#define LTTF(x, y) ((TF) (x) < (TF) (y))
#define LETF(x, y) ((TF) (x) <= (TF) (y))
#define GTTF(x, y) ((TF) (x) > (TF) (y))
#define GETF(x, y) ((TF) (x) >= (TF) (y))
extern TF ABSTF PARAMS ((TF));
extern TF SQRTTF PARAMS ((TF));
extern TF COSTF PARAMS ((TF));
extern TF SINTF PARAMS ((TF));
#endif /* TF_FN_SUPPORT */


#define EXTBIQI(x) ((QI) (BI) (x))
#define EXTBIHI(x) ((HI) (BI) (x))
#define EXTBISI(x) ((SI) (BI) (x))
#if defined (DI_FN_SUPPORT)
extern DI EXTBIDI PARAMS ((BI));
#else
#define EXTBIDI(x) ((DI) (BI) (x))
#endif
#define EXTQIHI(x) ((HI) (QI) (x))
#define EXTQISI(x) ((SI) (QI) (x))
#if defined (DI_FN_SUPPORT)
extern DI EXTQIDI PARAMS ((QI));
#else
#define EXTQIDI(x) ((DI) (QI) (x))
#endif
#define EXTHISI(x) ((SI) (HI) (x))
#if defined (DI_FN_SUPPORT)
extern DI EXTHIDI PARAMS ((HI));
#else
#define EXTHIDI(x) ((DI) (HI) (x))
#endif
#if defined (DI_FN_SUPPORT)
extern DI EXTSIDI PARAMS ((SI));
#else
#define EXTSIDI(x) ((DI) (SI) (x))
#endif
#if defined (SF_FN_SUPPORT) || defined (DF_FN_SUPPORT)
extern DF EXTSFDF PARAMS ((SF));
#else
#define EXTSFDF(x) ((DF) (SF) (x))
#endif
#if defined (SF_FN_SUPPORT) || defined (XF_FN_SUPPORT)
extern XF EXTSFXF PARAMS ((SF));
#else
#define EXTSFXF(x) ((XF) (SF) (x))
#endif
#if defined (SF_FN_SUPPORT) || defined (TF_FN_SUPPORT)
extern TF EXTSFTF PARAMS ((SF));
#else
#define EXTSFTF(x) ((TF) (SF) (x))
#endif
#if defined (DF_FN_SUPPORT) || defined (XF_FN_SUPPORT)
extern XF EXTDFXF PARAMS ((DF));
#else
#define EXTDFXF(x) ((XF) (DF) (x))
#endif
#if defined (DF_FN_SUPPORT) || defined (TF_FN_SUPPORT)
extern TF EXTDFTF PARAMS ((DF));
#else
#define EXTDFTF(x) ((TF) (DF) (x))
#endif
#if defined (XF_FN_SUPPORT) || defined (TF_FN_SUPPORT)
extern TF EXTXFTF PARAMS ((XF));
#else
#define EXTXFTF(x) ((TF) (XF) (x))
#endif
#define ZEXTBIQI(x) ((QI) (UBI) (x))
#define ZEXTBIHI(x) ((HI) (UBI) (x))
#define ZEXTBISI(x) ((SI) (UBI) (x))
#if defined (DI_FN_SUPPORT)
extern DI ZEXTBIDI PARAMS ((BI));
#else
#define ZEXTBIDI(x) ((DI) (UBI) (x))
#endif
#define ZEXTQIHI(x) ((HI) (UQI) (x))
#define ZEXTQISI(x) ((SI) (UQI) (x))
#if defined (DI_FN_SUPPORT)
extern DI ZEXTQIDI PARAMS ((QI));
#else
#define ZEXTQIDI(x) ((DI) (UQI) (x))
#endif
#define ZEXTHISI(x) ((SI) (UHI) (x))
#if defined (DI_FN_SUPPORT)
extern DI ZEXTHIDI PARAMS ((HI));
#else
#define ZEXTHIDI(x) ((DI) (UHI) (x))
#endif
#if defined (DI_FN_SUPPORT)
extern DI ZEXTSIDI PARAMS ((SI));
#else
#define ZEXTSIDI(x) ((DI) (USI) (x))
#endif
#define TRUNCQIBI(x) ((BI) (QI) (x))
#define TRUNCHIBI(x) ((BI) (HI) (x))
#define TRUNCHIQI(x) ((QI) (HI) (x))
#define TRUNCSIBI(x) ((BI) (SI) (x))
#define TRUNCSIQI(x) ((QI) (SI) (x))
#define TRUNCSIHI(x) ((HI) (SI) (x))
#if defined (DI_FN_SUPPORT)
extern BI TRUNCDIBI PARAMS ((DI));
#else
#define TRUNCDIBI(x) ((BI) (DI) (x))
#endif
#if defined (DI_FN_SUPPORT)
extern QI TRUNCDIQI PARAMS ((DI));
#else
#define TRUNCDIQI(x) ((QI) (DI) (x))
#endif
#if defined (DI_FN_SUPPORT)
extern HI TRUNCDIHI PARAMS ((DI));
#else
#define TRUNCDIHI(x) ((HI) (DI) (x))
#endif
#if defined (DI_FN_SUPPORT)
extern SI TRUNCDISI PARAMS ((DI));
#else
#define TRUNCDISI(x) ((SI) (DI) (x))
#endif
#if defined (DF_FN_SUPPORT) || defined (SF_FN_SUPPORT)
extern SF TRUNCDFSF PARAMS ((DF));
#else
#define TRUNCDFSF(x) ((SF) (DF) (x))
#endif
#if defined (XF_FN_SUPPORT) || defined (SF_FN_SUPPORT)
extern SF TRUNCXFSF PARAMS ((XF));
#else
#define TRUNCXFSF(x) ((SF) (XF) (x))
#endif
#if defined (XF_FN_SUPPORT) || defined (DF_FN_SUPPORT)
extern DF TRUNCXFDF PARAMS ((XF));
#else
#define TRUNCXFDF(x) ((DF) (XF) (x))
#endif
#if defined (TF_FN_SUPPORT) || defined (SF_FN_SUPPORT)
extern SF TRUNCTFSF PARAMS ((TF));
#else
#define TRUNCTFSF(x) ((SF) (TF) (x))
#endif
#if defined (TF_FN_SUPPORT) || defined (DF_FN_SUPPORT)
extern DF TRUNCTFDF PARAMS ((TF));
#else
#define TRUNCTFDF(x) ((DF) (TF) (x))
#endif
#if defined (TF_FN_SUPPORT) || defined (XF_FN_SUPPORT)
extern XF TRUNCTFXF PARAMS ((TF));
#else
#define TRUNCTFXF(x) ((XF) (TF) (x))
#endif
#if defined (SF_FN_SUPPORT)
extern SF FLOATBISF PARAMS ((BI));
#else
#define FLOATBISF(x) ((SF) (BI) (x))
#endif
#if defined (DF_FN_SUPPORT)
extern DF FLOATBIDF PARAMS ((BI));
#else
#define FLOATBIDF(x) ((DF) (BI) (x))
#endif
#if defined (XF_FN_SUPPORT)
extern XF FLOATBIXF PARAMS ((BI));
#else
#define FLOATBIXF(x) ((XF) (BI) (x))
#endif
#if defined (TF_FN_SUPPORT)
extern TF FLOATBITF PARAMS ((BI));
#else
#define FLOATBITF(x) ((TF) (BI) (x))
#endif
#if defined (SF_FN_SUPPORT)
extern SF FLOATQISF PARAMS ((QI));
#else
#define FLOATQISF(x) ((SF) (QI) (x))
#endif
#if defined (DF_FN_SUPPORT)
extern DF FLOATQIDF PARAMS ((QI));
#else
#define FLOATQIDF(x) ((DF) (QI) (x))
#endif
#if defined (XF_FN_SUPPORT)
extern XF FLOATQIXF PARAMS ((QI));
#else
#define FLOATQIXF(x) ((XF) (QI) (x))
#endif
#if defined (TF_FN_SUPPORT)
extern TF FLOATQITF PARAMS ((QI));
#else
#define FLOATQITF(x) ((TF) (QI) (x))
#endif
#if defined (SF_FN_SUPPORT)
extern SF FLOATHISF PARAMS ((HI));
#else
#define FLOATHISF(x) ((SF) (HI) (x))
#endif
#if defined (DF_FN_SUPPORT)
extern DF FLOATHIDF PARAMS ((HI));
#else
#define FLOATHIDF(x) ((DF) (HI) (x))
#endif
#if defined (XF_FN_SUPPORT)
extern XF FLOATHIXF PARAMS ((HI));
#else
#define FLOATHIXF(x) ((XF) (HI) (x))
#endif
#if defined (TF_FN_SUPPORT)
extern TF FLOATHITF PARAMS ((HI));
#else
#define FLOATHITF(x) ((TF) (HI) (x))
#endif
#if defined (SF_FN_SUPPORT)
extern SF FLOATSISF PARAMS ((SI));
#else
#define FLOATSISF(x) ((SF) (SI) (x))
#endif
#if defined (DF_FN_SUPPORT)
extern DF FLOATSIDF PARAMS ((SI));
#else
#define FLOATSIDF(x) ((DF) (SI) (x))
#endif
#if defined (XF_FN_SUPPORT)
extern XF FLOATSIXF PARAMS ((SI));
#else
#define FLOATSIXF(x) ((XF) (SI) (x))
#endif
#if defined (TF_FN_SUPPORT)
extern TF FLOATSITF PARAMS ((SI));
#else
#define FLOATSITF(x) ((TF) (SI) (x))
#endif
#if defined (DI_FN_SUPPORT) || defined (SF_FN_SUPPORT)
extern SF FLOATDISF PARAMS ((DI));
#else
#define FLOATDISF(x) ((SF) (DI) (x))
#endif
#if defined (DI_FN_SUPPORT) || defined (DF_FN_SUPPORT)
extern DF FLOATDIDF PARAMS ((DI));
#else
#define FLOATDIDF(x) ((DF) (DI) (x))
#endif
#if defined (DI_FN_SUPPORT) || defined (XF_FN_SUPPORT)
extern XF FLOATDIXF PARAMS ((DI));
#else
#define FLOATDIXF(x) ((XF) (DI) (x))
#endif
#if defined (DI_FN_SUPPORT) || defined (TF_FN_SUPPORT)
extern TF FLOATDITF PARAMS ((DI));
#else
#define FLOATDITF(x) ((TF) (DI) (x))
#endif
#if defined (SF_FN_SUPPORT)
extern SF UFLOATBISF PARAMS ((BI));
#else
#define UFLOATBISF(x) ((SF) (UBI) (x))
#endif
#if defined (DF_FN_SUPPORT)
extern DF UFLOATBIDF PARAMS ((BI));
#else
#define UFLOATBIDF(x) ((DF) (UBI) (x))
#endif
#if defined (XF_FN_SUPPORT)
extern XF UFLOATBIXF PARAMS ((BI));
#else
#define UFLOATBIXF(x) ((XF) (UBI) (x))
#endif
#if defined (TF_FN_SUPPORT)
extern TF UFLOATBITF PARAMS ((BI));
#else
#define UFLOATBITF(x) ((TF) (UBI) (x))
#endif
#if defined (SF_FN_SUPPORT)
extern SF UFLOATQISF PARAMS ((QI));
#else
#define UFLOATQISF(x) ((SF) (UQI) (x))
#endif
#if defined (DF_FN_SUPPORT)
extern DF UFLOATQIDF PARAMS ((QI));
#else
#define UFLOATQIDF(x) ((DF) (UQI) (x))
#endif
#if defined (XF_FN_SUPPORT)
extern XF UFLOATQIXF PARAMS ((QI));
#else
#define UFLOATQIXF(x) ((XF) (UQI) (x))
#endif
#if defined (TF_FN_SUPPORT)
extern TF UFLOATQITF PARAMS ((QI));
#else
#define UFLOATQITF(x) ((TF) (UQI) (x))
#endif
#if defined (SF_FN_SUPPORT)
extern SF UFLOATHISF PARAMS ((HI));
#else
#define UFLOATHISF(x) ((SF) (UHI) (x))
#endif
#if defined (DF_FN_SUPPORT)
extern DF UFLOATHIDF PARAMS ((HI));
#else
#define UFLOATHIDF(x) ((DF) (UHI) (x))
#endif
#if defined (XF_FN_SUPPORT)
extern XF UFLOATHIXF PARAMS ((HI));
#else
#define UFLOATHIXF(x) ((XF) (UHI) (x))
#endif
#if defined (TF_FN_SUPPORT)
extern TF UFLOATHITF PARAMS ((HI));
#else
#define UFLOATHITF(x) ((TF) (UHI) (x))
#endif
#if defined (SF_FN_SUPPORT)
extern SF UFLOATSISF PARAMS ((SI));
#else
#define UFLOATSISF(x) ((SF) (USI) (x))
#endif
#if defined (DF_FN_SUPPORT)
extern DF UFLOATSIDF PARAMS ((SI));
#else
#define UFLOATSIDF(x) ((DF) (USI) (x))
#endif
#if defined (XF_FN_SUPPORT)
extern XF UFLOATSIXF PARAMS ((SI));
#else
#define UFLOATSIXF(x) ((XF) (USI) (x))
#endif
#if defined (TF_FN_SUPPORT)
extern TF UFLOATSITF PARAMS ((SI));
#else
#define UFLOATSITF(x) ((TF) (USI) (x))
#endif
#if defined (DI_FN_SUPPORT) || defined (SF_FN_SUPPORT)
extern SF UFLOATDISF PARAMS ((DI));
#else
#define UFLOATDISF(x) ((SF) (UDI) (x))
#endif
#if defined (DI_FN_SUPPORT) || defined (DF_FN_SUPPORT)
extern DF UFLOATDIDF PARAMS ((DI));
#else
#define UFLOATDIDF(x) ((DF) (UDI) (x))
#endif
#if defined (DI_FN_SUPPORT) || defined (XF_FN_SUPPORT)
extern XF UFLOATDIXF PARAMS ((DI));
#else
#define UFLOATDIXF(x) ((XF) (UDI) (x))
#endif
#if defined (DI_FN_SUPPORT) || defined (TF_FN_SUPPORT)
extern TF UFLOATDITF PARAMS ((DI));
#else
#define UFLOATDITF(x) ((TF) (UDI) (x))
#endif
#if defined (SF_FN_SUPPORT)
extern BI FIXSFBI PARAMS ((SF));
#else
#define FIXSFBI(x) ((BI) (SF) (x))
#endif
#if defined (SF_FN_SUPPORT)
extern QI FIXSFQI PARAMS ((SF));
#else
#define FIXSFQI(x) ((QI) (SF) (x))
#endif
#if defined (SF_FN_SUPPORT)
extern HI FIXSFHI PARAMS ((SF));
#else
#define FIXSFHI(x) ((HI) (SF) (x))
#endif
#if defined (SF_FN_SUPPORT)
extern SI FIXSFSI PARAMS ((SF));
#else
#define FIXSFSI(x) ((SI) (SF) (x))
#endif
#if defined (SF_FN_SUPPORT) || defined (DI_FN_SUPPORT)
extern DI FIXSFDI PARAMS ((SF));
#else
#define FIXSFDI(x) ((DI) (SF) (x))
#endif
#if defined (DF_FN_SUPPORT)
extern BI FIXDFBI PARAMS ((DF));
#else
#define FIXDFBI(x) ((BI) (DF) (x))
#endif
#if defined (DF_FN_SUPPORT)
extern QI FIXDFQI PARAMS ((DF));
#else
#define FIXDFQI(x) ((QI) (DF) (x))
#endif
#if defined (DF_FN_SUPPORT)
extern HI FIXDFHI PARAMS ((DF));
#else
#define FIXDFHI(x) ((HI) (DF) (x))
#endif
#if defined (DF_FN_SUPPORT)
extern SI FIXDFSI PARAMS ((DF));
#else
#define FIXDFSI(x) ((SI) (DF) (x))
#endif
#if defined (DF_FN_SUPPORT) || defined (DI_FN_SUPPORT)
extern DI FIXDFDI PARAMS ((DF));
#else
#define FIXDFDI(x) ((DI) (DF) (x))
#endif
#if defined (XF_FN_SUPPORT)
extern BI FIXXFBI PARAMS ((XF));
#else
#define FIXXFBI(x) ((BI) (XF) (x))
#endif
#if defined (XF_FN_SUPPORT)
extern QI FIXXFQI PARAMS ((XF));
#else
#define FIXXFQI(x) ((QI) (XF) (x))
#endif
#if defined (XF_FN_SUPPORT)
extern HI FIXXFHI PARAMS ((XF));
#else
#define FIXXFHI(x) ((HI) (XF) (x))
#endif
#if defined (XF_FN_SUPPORT)
extern SI FIXXFSI PARAMS ((XF));
#else
#define FIXXFSI(x) ((SI) (XF) (x))
#endif
#if defined (XF_FN_SUPPORT) || defined (DI_FN_SUPPORT)
extern DI FIXXFDI PARAMS ((XF));
#else
#define FIXXFDI(x) ((DI) (XF) (x))
#endif
#if defined (TF_FN_SUPPORT)
extern BI FIXTFBI PARAMS ((TF));
#else
#define FIXTFBI(x) ((BI) (TF) (x))
#endif
#if defined (TF_FN_SUPPORT)
extern QI FIXTFQI PARAMS ((TF));
#else
#define FIXTFQI(x) ((QI) (TF) (x))
#endif
#if defined (TF_FN_SUPPORT)
extern HI FIXTFHI PARAMS ((TF));
#else
#define FIXTFHI(x) ((HI) (TF) (x))
#endif
#if defined (TF_FN_SUPPORT)
extern SI FIXTFSI PARAMS ((TF));
#else
#define FIXTFSI(x) ((SI) (TF) (x))
#endif
#if defined (TF_FN_SUPPORT) || defined (DI_FN_SUPPORT)
extern DI FIXTFDI PARAMS ((TF));
#else
#define FIXTFDI(x) ((DI) (TF) (x))
#endif
#if defined (SF_FN_SUPPORT)
extern BI UFIXSFBI PARAMS ((SF));
#else
#define UFIXSFBI(x) ((UBI) (SF) (x))
#endif
#if defined (SF_FN_SUPPORT)
extern QI UFIXSFQI PARAMS ((SF));
#else
#define UFIXSFQI(x) ((UQI) (SF) (x))
#endif
#if defined (SF_FN_SUPPORT)
extern HI UFIXSFHI PARAMS ((SF));
#else
#define UFIXSFHI(x) ((UHI) (SF) (x))
#endif
#if defined (SF_FN_SUPPORT)
extern SI UFIXSFSI PARAMS ((SF));
#else
#define UFIXSFSI(x) ((USI) (SF) (x))
#endif
#if defined (SF_FN_SUPPORT) || defined (DI_FN_SUPPORT)
extern DI UFIXSFDI PARAMS ((SF));
#else
#define UFIXSFDI(x) ((UDI) (SF) (x))
#endif
#if defined (DF_FN_SUPPORT)
extern BI UFIXDFBI PARAMS ((DF));
#else
#define UFIXDFBI(x) ((UBI) (DF) (x))
#endif
#if defined (DF_FN_SUPPORT)
extern QI UFIXDFQI PARAMS ((DF));
#else
#define UFIXDFQI(x) ((UQI) (DF) (x))
#endif
#if defined (DF_FN_SUPPORT)
extern HI UFIXDFHI PARAMS ((DF));
#else
#define UFIXDFHI(x) ((UHI) (DF) (x))
#endif
#if defined (DF_FN_SUPPORT)
extern SI UFIXDFSI PARAMS ((DF));
#else
#define UFIXDFSI(x) ((USI) (DF) (x))
#endif
#if defined (DF_FN_SUPPORT) || defined (DI_FN_SUPPORT)
extern DI UFIXDFDI PARAMS ((DF));
#else
#define UFIXDFDI(x) ((UDI) (DF) (x))
#endif
#if defined (XF_FN_SUPPORT)
extern BI UFIXXFBI PARAMS ((XF));
#else
#define UFIXXFBI(x) ((UBI) (XF) (x))
#endif
#if defined (XF_FN_SUPPORT)
extern QI UFIXXFQI PARAMS ((XF));
#else
#define UFIXXFQI(x) ((UQI) (XF) (x))
#endif
#if defined (XF_FN_SUPPORT)
extern HI UFIXXFHI PARAMS ((XF));
#else
#define UFIXXFHI(x) ((UHI) (XF) (x))
#endif
#if defined (XF_FN_SUPPORT)
extern SI UFIXXFSI PARAMS ((XF));
#else
#define UFIXXFSI(x) ((USI) (XF) (x))
#endif
#if defined (XF_FN_SUPPORT) || defined (DI_FN_SUPPORT)
extern DI UFIXXFDI PARAMS ((XF));
#else
#define UFIXXFDI(x) ((UDI) (XF) (x))
#endif
#if defined (TF_FN_SUPPORT)
extern BI UFIXTFBI PARAMS ((TF));
#else
#define UFIXTFBI(x) ((UBI) (TF) (x))
#endif
#if defined (TF_FN_SUPPORT)
extern QI UFIXTFQI PARAMS ((TF));
#else
#define UFIXTFQI(x) ((UQI) (TF) (x))
#endif
#if defined (TF_FN_SUPPORT)
extern HI UFIXTFHI PARAMS ((TF));
#else
#define UFIXTFHI(x) ((UHI) (TF) (x))
#endif
#if defined (TF_FN_SUPPORT)
extern SI UFIXTFSI PARAMS ((TF));
#else
#define UFIXTFSI(x) ((USI) (TF) (x))
#endif
#if defined (TF_FN_SUPPORT) || defined (DI_FN_SUPPORT)
extern DI UFIXTFDI PARAMS ((TF));
#else
#define UFIXTFDI(x) ((UDI) (TF) (x))
#endif

/* Semantic support utilities.  */

#ifdef __GNUC__

#ifdef SEMOPS_DEFINE_INLINE
#define SEMOPS_INLINE
#else
#define SEMOPS_INLINE extern inline
#endif

SEMOPS_INLINE SI
ADDCSI (SI a, SI b, UBI c)
{
  SI res = ADDSI (a, ADDSI (b, c));
  return res;
}

SEMOPS_INLINE UBI
ADDCFSI (SI a, SI b, UBI c)
{
  SI tmp = ADDSI (a, ADDSI (b, c));
  BI res = (USI) tmp < (USI) a || (USI) tmp < (USI) b;
  return res;
}

SEMOPS_INLINE UBI
ADDOFSI (SI a, SI b, UBI c)
{
  SI tmp = ADDSI (a, ADDSI (b, c));
  BI res = (((a < 0) == (b < 0))
	    && ((a < 0) != (tmp < 0)));
  return res;
}

SEMOPS_INLINE SI
SUBCSI (SI a, SI b, UBI c)
{
  SI res = SUBSI (a, ADDSI (b, c));
  return res;
}

SEMOPS_INLINE UBI
SUBCFSI (SI a, SI b, UBI c)
{
  BI res = ((USI) a < (USI) b) || ((a == b) && c);
  return res;
}

SEMOPS_INLINE UBI
SUBOFSI (SI a, SI b, UBI c)
{
  SI tmp = SUBSI (a, ADDSI (b, c));
  BI res = (((a < 0) != (b < 0))
	    && ((a < 0) != (tmp < 0)));
  return res;
}

#else

SI ADDCSI (SI, SI, UBI);
UBI ADDCFSI (SI, SI, UBI);
UBI ADDOFSI (SI, SI, UBI);
SI SUBCSI (SI, SI, UBI);
UBI SUBCFSI (SI, SI, UBI);
UBI SUBOFSI (SI, SI, UBI);

#endif

/* DI mode support if "long long" doesn't exist.
   At one point CGEN supported K&R C compilers, and ANSI C compilers without
   "long long".  One can argue the various merits of keeping this in or
   throwing it out.  I went to the trouble of adding it so for the time being
   I'm leaving it in.  */

#ifdef DI_FN_SUPPORT

DI make_struct_di (SI, SI);
/* FIXME: needed? */
DI CONVHIDI (HI);
DI CONVSIDI (SI);
SI CONVDISI (DI);

#endif /* DI_FN_SUPPORT */

#endif /* CGEN_SEM_OPS_H */
