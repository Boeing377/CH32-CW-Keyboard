/*
 * train.h
 *
 * Training mode state machine and text generation.
 */

#ifndef USER_TRAIN_H_
#define USER_TRAIN_H_

#include "global.h"

/* Initialize training defaults after EEPROM read */
void Train_InitDefaults (void);

/* Generate training text based on current train_info settings */
void Train_GenerateText (void);

/* Enter training mode: generate text, start audio, set display */
void Train_Start (void);

/* Exit training mode, return to menu */
void Train_Exit (void);

/* Called every main loop tick: check for phase transitions */
void Train_Poll (void);

/* Handle a key press during training mode */
void Train_HandleKey (uint8_t key_value);

/* Get max setting item index for current method */
uint8_t Train_GetSettingItemMax (void);

/* Collapse consecutive spaces to single space, return new length */
uint16_t Train_NormalizeSpaces (const char *src, uint16_t src_len, char *dst);

/* Greedy sequence alignment: produces aligned answer (in train_buf),
   aligned user (in train_align_user), match flags (train_align_match).
   Sets train_align_len. */
void Train_Align (const char *answer, uint16_t a_len,
                  const char *user,   uint16_t u_len);

#endif /* USER_TRAIN_H_ */
