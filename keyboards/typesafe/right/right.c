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

#include "right.h"
#include "pointing_device.h"
#include "transactions.h"

extern const pointing_device_driver_t pointing_device_driver;

static bool scroll_pressed_x;
static bool scroll_pressed_y;
static bool mouse_buttons_dirty;
static int8_t scroll_h;
static int8_t scroll_v;
static joy_sync_t state;
static uint32_t last_sync = 0;

#define UP KC_LALT
#define DOWN KC_LSFT
#define LEFT KC_LGUI
#define RIGHT KC_LCTL

void register_joy(bool old, bool dir, uint16_t keycode) {
    if (old == dir) {
        return;
    }
    if (dir) {
        register_code(keycode);
    } else {
        unregister_code(keycode);
    }
}

void reset_joy(void) {
    register_joy(state.up, false, UP);
    register_joy(state.down, false, DOWN);
    register_joy(state.left, false, LEFT);
    register_joy(state.right, false, RIGHT);
    state.up = false;
    state.down = false;
    state.left = false;
    state.right = false;
}

void matrix_scan_user() {
   if (!is_keyboard_master() || timer_elapsed32(last_sync) <= 100) return;
        
    joy_sync_t d = {false, false, false, false};
    if(transaction_rpc_exec(JOY_SYNC, sizeof(0), NULL, sizeof(d), &d)) {
        last_sync = timer_read32();
        register_joy(state.up, d.up, UP);
        register_joy(state.down, d.down, DOWN);
        register_joy(state.left, d.left, LEFT);
        register_joy(state.right, d.right, RIGHT);
        state = d;
    } else {
        reset_joy();
    }
}

void pointing_device_init_kb(void){
    if(!is_keyboard_master()) return;

    config_oddball_t kb_config;
    kb_config.raw = eeconfig_read_kb();

    if(!kb_config.cpi) {
        kb_config.cpi = CPI_2;
        eeconfig_update_kb(kb_config.raw);
    }

    pointing_device_set_cpi(kb_config.cpi);
}

report_mouse_t pointing_device_task_kb(report_mouse_t mouse_report) {
    if (!is_keyboard_master()) return mouse_report;

    if (abs(mouse_report.x) > TS_POINTER_LAYER_TRIGGER_THRESHOLD || abs(mouse_report.y) > TS_POINTER_LAYER_TRIGGER_THRESHOLD) {
        layer_on(2);
    }

    int8_t clamped_x = mouse_report.x, clamped_y = mouse_report.y;
    mouse_report.x = 0;
    mouse_report.y = 0;

    if (scroll_pressed_x || scroll_pressed_y) {
        // accumulate scroll
        scroll_h += clamped_x;
        scroll_v += clamped_y;

        int8_t scaled_scroll_h = scroll_h / SCROLL_DIVIDER;
        int8_t scaled_scroll_v = scroll_v / SCROLL_DIVIDER;

        // clear accumulated scroll on assignment

        if (scroll_pressed_x && scaled_scroll_h != 0) {
            mouse_report.h = -scaled_scroll_h;
            scroll_h       = 0;
        }

        if (scroll_pressed_y && scaled_scroll_v != 0) {
            mouse_report.v = -scaled_scroll_v;
            scroll_v       = 0;
        }
    } else {
        mouse_report.x = -clamped_x;
        mouse_report.y = clamped_y;
    }

    return mouse_report;
}

static void on_cpi_button(uint16_t cpi, keyrecord_t *record) {

    if(!record->event.pressed)
        return;

    pointing_device_set_cpi(cpi);

    config_oddball_t kb_config;
    kb_config.cpi = cpi;
    eeconfig_update_kb(kb_config.raw);
}

static void on_mouse_button(uint8_t mouse_button, keyrecord_t *record) {

    report_mouse_t report = pointing_device_get_report();

    if(record->event.pressed)
        report.buttons |= mouse_button;
    else
        report.buttons &= ~mouse_button;

    pointing_device_set_report(report);
    mouse_buttons_dirty = true;
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {

    if(!process_record_user(keycode, record))
        return false;

    // handle mouse drag and scroll

    switch (keycode) {
        case KC_BTN1:
            on_mouse_button(MOUSE_BTN1, record);
            return false;

        case KC_BTN2:
            on_mouse_button(MOUSE_BTN2, record);
            return false;

        case KC_BTN3:
            on_mouse_button(MOUSE_BTN3, record);
            return false;

        case KC_BTN4:
            on_mouse_button(MOUSE_BTN4, record);
            return false;

        case KC_BTN5:
            on_mouse_button(MOUSE_BTN5, record);
            return false;

        case KC_SCROLL:
            scroll_pressed_x = scroll_pressed_y = record->event.pressed;
            return false;

        case KC_SCROLX:
            scroll_pressed_x = record->event.pressed;
            return false;

        case KC_SCROLY:
            scroll_pressed_y = record->event.pressed;
            return false;

        case KC_CPI_1:
            on_cpi_button(CPI_1, record);
            return false;

        case KC_CPI_2:
            on_cpi_button(CPI_2, record);
            return false;

        case KC_CPI_3:
            on_cpi_button(CPI_3, record);
            return false;

        default:
            return true;
    }
}
