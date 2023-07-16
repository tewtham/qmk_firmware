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

#include "left.h"
#include "joystick.h"
#include "transactions.h"

joystick_config_t joystick_axes[JOYSTICK_AXIS_COUNT] = {
   JOYSTICK_AXIS_IN(F0, 1023, 512, 0),
   JOYSTICK_AXIS_IN(F1, 1023, 512, 0)
};

#define UP    (y > JOYSTICK_ZERO)
#define RIGHT (x > JOYSTICK_ZERO)
#define DOWN  (y < -JOYSTICK_ZERO)
#define LEFT  (x < -JOYSTICK_ZERO)

void joy_sync_handler(uint8_t in_buflen, const void* in_data, uint8_t out_buflen, void* out_data) {
    int16_t y = joystick_read_axis(0);
    int16_t x = -joystick_read_axis(1);
    joy_sync_t *data = (joy_sync_t*)out_data;
    data->up = UP;
    data->down = DOWN;
    data->left = LEFT;
    data->right = RIGHT;
}

void keyboard_post_init_user(void) {
    transaction_register_rpc(JOY_SYNC, joy_sync_handler);
}
