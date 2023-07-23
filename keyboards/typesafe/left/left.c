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
#include "matrix.h"

joystick_config_t joystick_axes[JOYSTICK_AXIS_COUNT] = {
   JOYSTICK_AXIS_IN(F0, 1023, 450, 0),
   JOYSTICK_AXIS_IN(F1, 1023, 450, 0)
};

#define UP    (y > JOYSTICK_ZERO)
#define RIGHT (x > JOYSTICK_ZERO)
#define DOWN  (y < -JOYSTICK_ZERO)
#define LEFT  (x < -JOYSTICK_ZERO)

uint8_t read_joystick(void) {
    int16_t y = joystick_read_axis(0);
    int16_t x = -joystick_read_axis(1);

    // small twist
    double r = sqrt(pow(x, 2.0) + pow(y, 2.0));
    double a = atan2(y, x) + 0.6;
    x = floor(r * cos(a));

    y = floor(r * sin(a));
    matrix_row_t joystick = 0;
    matrix_row_t row_shifter = MATRIX_ROW_SHIFTER;
    joystick |= UP ? row_shifter : 0;
    row_shifter <<= 1;
    joystick |= DOWN ? row_shifter : 0;
    row_shifter <<= 1;
    joystick |= LEFT ? row_shifter : 0;
    row_shifter <<= 1;
    joystick |= RIGHT ? row_shifter : 0;
    return joystick;
}

static pin_t row_pins[5] = {F7, F6, F5, F4, D3};
static pin_t col_pins[6] = {B6, D2, C6, D7, E6, B4};

static inline void setPinOutput_writeLow(pin_t pin) {
    ATOMIC_BLOCK_FORCEON {
        setPinOutput(pin);
        writePinLow(pin);
    }
}

static inline void setPinInputHigh_atomic(pin_t pin) {
    ATOMIC_BLOCK_FORCEON {
        setPinInputHigh(pin);
    }
}

static bool select_row(uint8_t row) {
    pin_t pin = row_pins[row];
    if (pin != NO_PIN) {
        setPinOutput_writeLow(pin);
        return true;
    }
    return false;
}

static void unselect_row(uint8_t row) {
    pin_t pin = row_pins[row];
    if (pin != NO_PIN) {
        setPinInputHigh_atomic(pin);
    }
}

#define MATRIX_INPUT_PRESSED_STATE 0
static inline uint8_t readMatrixPin(pin_t pin) {
    if (pin != NO_PIN) {
        return (readPin(pin) == MATRIX_INPUT_PRESSED_STATE) ? 0 : 1;
    } else {
        return 1;
    }
}

void matrix_read_cols_on_row(matrix_row_t current_matrix[], uint8_t current_row) {
    // Start with a clear matrix row
    matrix_row_t current_row_value = 0;

    if (current_row == 5) {
        current_matrix[current_row] = read_joystick();
        return;
    }

    if (!select_row(current_row)) { // Select row
        return;                     // skip NO_PIN row
    }
    matrix_output_select_delay();

    // For each col...
    matrix_row_t row_shifter = MATRIX_ROW_SHIFTER;
    for (uint8_t col_index = 0; col_index < MATRIX_COLS; col_index++, row_shifter <<= 1) {
        uint8_t pin_state = readMatrixPin(col_pins[col_index]);

        // Populate the matrix row with the state of the col pin
        current_row_value |= pin_state ? 0 : row_shifter;
    }

    // Unselect row
    unselect_row(current_row);
    matrix_output_unselect_delay(current_row, current_row_value != 0); // wait for all Col signals to go HIGH

    // Update the matrix
    current_matrix[current_row] = current_row_value;
}
