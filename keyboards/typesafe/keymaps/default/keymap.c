/* Copyright 2020 Alexander Tulloh
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H

enum layer_names {
    _QWERTY,
    _LOWER,
    _RAISE
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

  [_QWERTY] = LAYOUT(
     KC_ESC ,   KC_Q,    KC_W,    KC_E,    KC_R,       KC_T,          KC_Y,     KC_U,     KC_I,    KC_O,    KC_P,    KC_BSLS,
     KC_MINS,   KC_A,    KC_S,    KC_D,    KC_F,       KC_G,          KC_H,     KC_J,     KC_K,    KC_L,    KC_SCLN, KC_QUOT,
     KC_TAB ,   KC_Z,    KC_X,    KC_C,    KC_V,       KC_B,          KC_N,     KC_M,     KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,
     KC_LGUI,   KC_TILD, KC_EQUAL,KC_LALT,                                                KC_RBRC, KC_LBRC, KC_LCTL, KC_LALT,                           
                                  KC_SPC, KC_DEL,   KC_BSPC,          KC_END, KC_ENT, LYR
  ),

  [_LOWER] = LAYOUT(
     KC_MINS,   KC_EXLM, KC_AT,   KC_HASH,  KC_DLR, KC_PERC,       KC_CIRC,  KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_UNDS,
     KC_SCROLL, KC_1,    KC_2,    KC_3,     KC_4,   KC_5,          KC_6,     KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS,
     _______,   _______, _______, _______, _______, _______,       _______, _______, _______, _______, _______,  _______,
     KC_LGUI,   _______, _______, _______,                                           _______, _______, _______,  _______,
                                  _______, _______, _______,       _______, _______,  LYR
  ),

  [_RAISE] = LAYOUT(
     _______,   KC_SCROLL, _______, KC_UP,   _______,  _______,       KC_SCROLX, _______, _______, _______, _______,  _______,
     KC_SCROLL, KC_SCROLL, KC_LEFT, KC_DOWN, KC_RIGHT, _______,       KC_SCROLY, KC_BTN1, KC_BTN3, KC_BTN2, _______,  _______,
     _______,   _______,   _______, _______, _______,  _______,       _______,   _______, _______, _______, _______,  _______,
     _______,   _______,   _______, _______,                                            _______, _______, _______,  _______,
                                  _______, _______, _______,          _______,    _______, LYR
  )
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
   case LYR:
      layer_state_set(_QWERTY);
      if (record->event.pressed) {
         layer_on(_LOWER);
      }
      return false; // Skip all further processing of this key
   default:
      return true; // Process all other keycodes normally
  }
}
