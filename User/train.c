/*
 * train.c
 *
 * Training mode: text generation, state machine, scoring.
 */

#include "train.h"
#include "morse_send.h"
#include "string.h"

/* ── simple LCG random ─────────────────────────────────── */
static uint32_t train_rand_seed = 0xDEADBEEF;

static void Train_Srand (uint32_t seed) {
    train_rand_seed = seed;
}

static uint32_t Train_Rand (void) {
    train_rand_seed = train_rand_seed * 1103515245 + 12345;
    return (train_rand_seed >> 16) & 0x7FFF;
}

/* ── default init ──────────────────────────────────────── */
void Train_InitDefaults (void) {
    if (train_info.methon >= TRAIN_METHOD_COUNT)
        train_info.methon = TRAIN_METHOD_KOCH;
    if (train_info.lesson < 1 || train_info.lesson > 40)
        train_info.lesson = 1;
    /* chars_per_group: 0=RAND(random 5-7), 2-7=fixed.
       BSS=0 matches RAND, so on cold boot it looks like RAND.
       We force default 5; user reselects RAND via menu each session. */
    if (train_info.chars_per_group < 2 || train_info.chars_per_group > 7)
        train_info.chars_per_group = 5;
    if (train_info.group_gap_spaces < 1 || train_info.group_gap_spaces > 5)
        train_info.group_gap_spaces = 1;
    if (train_info.train_duration_min < 1 || train_info.train_duration_min > 5)
        train_info.train_duration_min = 1;
    /* config.wpm: EEPROM may contain stale data from old struct layouts
       that coincidentally passes initial_startup check.
       Guard: < 5 or > MAX_WPM → force default 20. */
    if (config.wpm < 5 || config.wpm > MAX_WPM)
        config.wpm = DEF_WPM;
}

/* ── training control ──────────────────────────────────── */
void Train_Start (void) {
    Train_GenerateText ();

    if (train_text_len == 0)
        return;

    /* Save & override beeper/mode for training audio */
    train_beeper_saved = config.beeper;
    train_mode_saved   = config.mode;
    config.beeper = 1;
    config.mode   = 0;

    /* Reset cursor edit state */
    cursor_edit_mode = 0;
    cursor_pos = 0;

    /* Copy generated text to dedicated train_buf (ISR reads it in PLAYING) */
    memcpy (train_buf, train_generated_text, train_text_len);
    train_buf_size = train_text_len;
    train_buf[train_buf_size] = '\0';

    /* Sync TIM2 with current WPM (may have changed in SETTING) */
    TIM2_ChangBase (3999,
        (60 * SystemCoreClock / (config.wpm * 50 * 1000) - 1));

    train_phase = TRAIN_PHASE_READY;
    disp_training = 1;
    sendCount = 0;

    /* starSending() deferred to Train_Poll after 1s pre-roll */
}

void Train_Exit (void) {
    endSending ();

    /* Restore beeper and mode */
    config.beeper = train_beeper_saved;
    config.mode   = train_mode_saved;

    /* Clear input buffer */
    memset (inputBuff, 0, BUFFSIZE);
    inputBuffSize = 0;
    sendCount = 0;

    /* Reset cursor edit state */
    cursor_edit_mode = 0;
    cursor_pos = 0;

    train_phase = TRAIN_PHASE_IDLE;
    disp_training = 0;
    disp_train_menu = 1;
}

/* ── poll: 1s pre-roll → start audio → detect end ─────── */
void Train_Poll (void) {
    if (!disp_training) return;

    if (train_phase == TRAIN_PHASE_READY) {
        static uint32_t ready_ms = 0;
        if (ready_ms == 0) {
            ready_ms = app_tick_ms;
            return;
        }
        if (app_tick_ms - ready_ms >= 1000) {
            ready_ms = 0;
            TIM_SetCounter (TIM2, 0);
            starSending ();
            train_phase = TRAIN_PHASE_RUNNING;
        }
        return;
    }

    /* RUNNING: audio plays + user types.  Poll does nothing;
       end-of-audio is detected by stge==0 (displayed as "Done"). */
}

/* ── key handling during training ─────────────────────── */
void Train_HandleKey (uint8_t key_value) {
    if (train_phase == TRAIN_PHASE_READY) {
        /* Only Esc cancels during pre-roll */
        if (key_value == 27) {
            Train_Exit ();
        }
        return;
    }

    if (train_phase == TRAIN_PHASE_RUNNING) {
        /* ── Insert key toggles cursor edit mode ─────────── */
        if (key_value == 30) {
            cursor_edit_mode = 1 - cursor_edit_mode;
            if (cursor_edit_mode) {
                cursor_pos = inputBuffSize;
            }
            return;
        }

        /* ── Cursor edit mode ──────────────────────────── */
        if (cursor_edit_mode) {
            /* Direction arrows: move cursor */
            if (key_value == 28) {
                /* Left */
                if (cursor_pos > 0) cursor_pos--;
                return;
            } else if (key_value == 29) {
                /* Right */
                if (cursor_pos < inputBuffSize) cursor_pos++;
                return;
            } else if (key_value == 0x1) {
                /* Down → jump to end */
                cursor_pos = inputBuffSize;
                return;
            } else if (key_value == 0x2) {
                /* Up → jump to start */
                cursor_pos = 0;
                return;
            }

            /* Printable character: insert at cursor */
            if (Morse_CanEncodeChar (key_value)) {
                if (inputBuffSize < INPUTZONE_SIZE - 1) {
                    uint16_t i;
                    for (i = inputBuffSize; i > cursor_pos; i--) {
                        inputBuff[i] = inputBuff[i - 1];
                    }
                    inputBuff[cursor_pos] = key_value;
                    inputBuffSize++;
                    cursor_pos++;
                    inputBuff[inputBuffSize] = '\0';
                }
                return;
            }

            /* Backspace: delete char before cursor */
            if (key_value == 127) {
                if (cursor_pos > 0 && inputBuffSize > 0) {
                    uint16_t i;
                    for (i = cursor_pos - 1; i < inputBuffSize - 1; i++) {
                        inputBuff[i] = inputBuff[i + 1];
                    }
                    inputBuffSize--;
                    cursor_pos--;
                    inputBuff[inputBuffSize] = '\0';
                }
                return;
            }

            /* Delete forward: delete char at cursor */
            if (key_value == 126) {
                if (cursor_pos < inputBuffSize) {
                    uint16_t i;
                    for (i = cursor_pos; i < inputBuffSize - 1; i++) {
                        inputBuff[i] = inputBuff[i + 1];
                    }
                    inputBuffSize--;
                    inputBuff[inputBuffSize] = '\0';
                }
                return;
            }

            /* Enter: exit edit mode and submit */
            if (key_value == 31) {
                cursor_edit_mode = 0;
                cursor_pos = 0;
                train_score_scroll = 0;
                inputBuffSize  = Train_NormalizeSpaces (
                    inputBuff, inputBuffSize, inputBuff);
                train_text_len = Train_NormalizeSpaces (
                    train_generated_text, train_text_len, train_generated_text);
                Train_Align (train_generated_text, train_text_len,
                             inputBuff, inputBuffSize);
                train_phase = TRAIN_PHASE_SCORING;
                return;
            }

            /* Escape: exit edit mode */
            if (key_value == 27) {
                cursor_edit_mode = 0;
                cursor_pos = 0;
                return;
            }

            return;
        }

        /* ── Normal (non-edit) mode ────────────────────── */
        /* Printable characters: append to input buffer */
        if (Morse_CanEncodeChar (key_value)) {
            if (inputBuffSize < INPUTZONE_SIZE - 1) {
                inputBuff[inputBuffSize++] = key_value;
                inputBuff[inputBuffSize] = '\0';
            }
        } else if (key_value == 127) {
            /* Backspace */
            if (inputBuffSize > 0)
                inputBuffSize--;
            inputBuff[inputBuffSize] = '\0';
        } else if (key_value == 31) {
            /* Enter → normalize spaces then go to scoring */
            train_score_scroll = 0;
            inputBuffSize  = Train_NormalizeSpaces (
                inputBuff, inputBuffSize, inputBuff);
            train_text_len = Train_NormalizeSpaces (
                train_generated_text, train_text_len, train_generated_text);
            Train_Align (train_generated_text, train_text_len,
                         inputBuff, inputBuffSize);
            train_phase = TRAIN_PHASE_SCORING;
        } else if (key_value == 27) {
            /* Esc → cancel */
            Train_Exit ();
        }
        return;
    }

    if (train_phase == TRAIN_PHASE_SCORING) {
        if (key_value == 0x1) {
            /* Down: scroll down */
            train_score_scroll++;
        } else if (key_value == 0x2) {
            /* Up: scroll up */
            if (train_score_scroll > 0)
                train_score_scroll--;
        } else if (key_value == 31 || key_value == 27) {
            /* Enter / Esc → exit scoring */
            Train_Exit ();
        }
    }
}

/* ── setting item count ────────────────────────────────── */
uint8_t Train_GetSettingItemMax (void) {
    if (train_info.methon == TRAIN_METHOD_FREE)
        return TRAIN_SETTING_ITEMS_FREE - 1;  /* 0..1 */
    return TRAIN_SETTING_ITEMS_KOCH - 1;       /* 0..3 */
}

/* ── space normalization: collapse consecutive spaces ──── */
uint16_t Train_NormalizeSpaces (const char *src, uint16_t src_len, char *dst) {
    uint16_t d = 0;
    uint16_t s;
    for (s = 0; s < src_len; s++) {
        if (src[s] == ' ' && s + 1 < src_len && src[s + 1] == ' ')
            continue;
        dst[d++] = src[s];
    }
    dst[d] = '\0';
    return d;
}

/* ── greedy sequence alignment ─────────────────────────── */
static uint8_t Train_CharEq (uint8_t c1, uint8_t c2) {
    if (c1 >= 'a' && c1 <= 'z') c1 -= 32;
    if (c2 >= 'a' && c2 <= 'z') c2 -= 32;
    return c1 == c2;
}

/* ── align one group pair, append result to aligned buffers ─ */
/* ── align one group pair: positional compare, pad at end ─ */
static uint16_t Train_AlignGroup (const char *ag, uint16_t ag_len,
                                  const char *ug, uint16_t ug_len,
                                  char *align_a, char *align_u,
                                  uint8_t *m, uint16_t d) {
    uint16_t common = (ag_len < ug_len) ? ag_len : ug_len;
    uint16_t i;

    /* Compare common prefix positionally (substitution, no lookahead) */
    for (i = 0; i < common && d < 254; i++, d++) {
        align_a[d] = ag[i];
        align_u[d] = ug[i];
        m[d] = Train_CharEq (ag[i], ug[i]) ? 1 : 0;
    }

    /* Pad shorter side at the end with '-' */
    while (i < ag_len && d < 254) {
        align_a[d] = ag[i++];
        align_u[d] = '-';
        m[d] = 0;
        d++;
    }
    while (i < ug_len && d < 254) {
        align_a[d] = '-';
        align_u[d] = ug[i++];
        m[d] = 0;
        d++;
    }
    return d;
}

/* ── per-group sequence alignment ──────────────────────── */
void Train_Align (const char *answer, uint16_t a_len,
                  const char *user,   uint16_t u_len) {
    char *align_a = train_buf;       /* reuse train_buf for aligned answer */
    char *align_u = train_align_user;
    uint8_t *m    = train_align_match;
    uint16_t d = 0;
    uint16_t as = 0, us = 0;  /* scan positions */

    while ((as < a_len || us < u_len) && d < 254) {
        /* Extract one group from each side */
        uint16_t ag_start = as, ug_start = us;
        while (as < a_len && answer[as] != ' ') as++;
        while (us < u_len && user[us]   != ' ') us++;
        uint16_t ag_len = as - ag_start;
        uint16_t ug_len = us - ug_start;

        /* Handle empty groups (one side has more groups) */
        if (ag_len == 0 && ug_len == 0) break;

        /* Align this group pair */
        d = Train_AlignGroup (answer + ag_start, ag_len,
                              user   + ug_start, ug_len,
                              align_a, align_u, m, d);

        /* Add space between groups (not after last) */
        if ((as < a_len || us < u_len) && d < 254) {
            align_a[d] = ' ';
            align_u[d] = ' ';
            m[d] = 1;  /* space always matches */
            d++;
        }

        /* Skip the space delimiter */
        if (as < a_len) as++;
        if (us < u_len) us++;
    }

    align_a[d] = '\0';
    align_u[d] = '\0';
    train_align_len = d;
}

/* ── text generation ───────────────────────────────────── */
/* Build a character pool: for KOCH = KochAplhaTable[0..lesson],
   for SEQU = A-Z 0-9 up to lesson+1 chars, for FREE = A-Z 0-9 all */
static uint8_t Train_BuildPool (char *pool) {
    uint8_t size = 0;

    if (train_info.methon == TRAIN_METHOD_KOCH) {
        /* Lesson N uses first N+1 Koch chars (lesson 1 = K,M) */
        uint8_t n = train_info.lesson + 1;
        if (n > 40) n = 40;
        size = n;
        memcpy (pool, KochAplhaTable, size);
    } else if (train_info.methon == TRAIN_METHOD_SEQU) {
        /* Sequential: all chars in alphabetic order (A-Z then 0-9) */
        uint8_t i;
        for (i = 0; i < 26; i++) pool[size++] = 'A' + i;
        for (i = 0; i < 10; i++) pool[size++] = '0' + i;
        /* Lesson N uses first N+1 chars (lesson 1 = A,B) */
        if (train_info.lesson + 1 < size)
            size = train_info.lesson + 1;
    } else {
        /* FREE: A-Z + 0-9 */
        uint8_t i;
        for (i = 0; i < 26; i++) pool[size++] = 'A' + i;
        for (i = 0; i < 10; i++) pool[size++] = '0' + i;
    }

    return size;
}

void Train_GenerateText (void) {
    char pool[41];
    uint8_t pool_size = Train_BuildPool (pool);
    uint8_t cpg_base = train_info.chars_per_group;
    uint8_t gap   = train_info.group_gap_spaces;
    uint8_t wpm   = (uint8_t)config.wpm;
    uint8_t dur   = train_info.train_duration_min;

    /* ── Dot-unit-based character count ─────────────────
       Standard PARIS word = 50 dot units.
       Average A-Z char ≈ 11.5 dot units (incl letter_break=3).
       Each gap space = 4 dot units (word_break 7 - letter_break 3).
       
       Target dot units  = WPM × 50 × duration_min
       Group cost (Q1 fixed-pt): cpg × 23  +  gap × 8
       Groups = target × 2 / group_cost
       Total chars = groups × cpg
    ─────────────────────────────────────────────────── */
    uint32_t target_dot2 = (uint32_t)wpm * 100 * dur;  /* ×2 for Q1 */
    uint16_t total_chars = 0;

    {
        /* Use default cpg=5 for estimation if RAND mode */
        uint8_t est_cpg = (cpg_base == 0) ? 5 : cpg_base;
        uint32_t group_cost = (uint32_t)est_cpg * 23 + (uint32_t)gap * 8;
        uint16_t num_groups = 0;

        if (group_cost > 0) {
            num_groups = (uint16_t)(target_dot2 / group_cost);
            if (num_groups < 1) num_groups = 1;
        }
        total_chars = (uint16_t)num_groups * est_cpg;
    }

    /* Seed random from app_tick_ms */
    Train_Srand (app_tick_ms);

    uint16_t pos = 0;
    uint16_t chars_left = total_chars;

    while (pos + 7 + gap < INPUTZONE_SIZE - 1) {

        /* Determine group size: RAND(0) → random 5-7 */
        uint8_t cpg;
        if (cpg_base == 0) {
            cpg = 5 + (uint8_t)(Train_Rand () % 3);  /* 5, 6, or 7 */
        } else {
            cpg = cpg_base;
        }

        if (chars_left < cpg)
            break;

        uint8_t i;
        for (i = 0; i < cpg; i++) {
            train_generated_text[pos++] = pool[Train_Rand () % pool_size];
        }
        chars_left -= cpg;

        /* Add inter-group gap */
        for (i = 0; i < gap && pos < INPUTZONE_SIZE - 1; i++) {
            train_generated_text[pos++] = ' ';
        }
    }

    /* Trim trailing spaces */
    while (pos > 0 && train_generated_text[pos - 1] == ' ')
        pos--;

    train_generated_text[pos] = '\0';
    train_text_len = pos;
}
