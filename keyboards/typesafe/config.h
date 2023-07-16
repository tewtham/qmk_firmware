#pragma once

/* key matrix size */
#define MATRIX_ROWS 10
#define MATRIX_COLS 6

/* Split Keyboard specific options */
#define USE_I2C
#define SPLIT_USB_DETECT
#define MASTER_RIGHT

/* Debounce reduces chatter (unintended double-presses) - set 0 if debouncing is not needed */
#define DEBOUNCE 5

/* Mechanical locking support. Use KC_LCAP, KC_LNUM or KC_LSCR instead in keymap */
#define LOCKING_SUPPORT_ENABLE
/* Locking resynchronize hack */
#define LOCKING_RESYNC_ENABLE
#define SPLIT_TRANSACTION_IDS_KB JOY_SYNC
