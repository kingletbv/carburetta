/* generated by gen_unicode_data.c */

#ifndef UNICODE_DATA_H
#define UNICODE_DATA_H

#ifdef __cplusplus
extern "C" {
#endif

enum UnicodeCategory {
  UCCG_INVALID,  /* Invalid unicode codepoint */
  UCCG_LU,
  UCCG_LL,
  UCCG_LT,
  UCCG_LM,
  UCCG_LO,
  UCCG_MN,
  UCCG_MC,
  UCCG_ME,
  UCCG_ND,
  UCCG_NL,
  UCCG_NO,
  UCCG_PC,
  UCCG_PD,
  UCCG_PS,
  UCCG_PE,
  UCCG_PI,
  UCCG_PF,
  UCCG_PO,
  UCCG_SM,
  UCCG_SC,
  UCCG_SK,
  UCCG_SO,
  UCCG_ZS,
  UCCG_ZL,
  UCCG_ZP,
  UCCG_CC,
  UCCG_CF,
  UCCG_CS,
  UCCG_CO,
  UCCG_CN,
};
/* index by codepoint/256; value is -1 if the range is described by a more
** detailed map, found at unicode_precise_category_map_lookup[codepoint/256].
** If all codepoints in the /256 range have the same category, it is listed
** in unicode_granular_category_map directly.
** If all codepoints in the /256 range have the same category, then
** unicode_precise_category_map_lookup[codepoint/256] will be NULL.
*/
extern signed char unicode_granular_category_map[];
extern signed char *unicode_precise_category_map_lookup[];

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* UNICODE_DATA_H */
