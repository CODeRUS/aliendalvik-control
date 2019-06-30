/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef INPUT_H
#define INPUT_H

#include "bitset.h"
#include "errors.h"

class Parcel;

namespace android {

enum {
    AMOTION_EVENT_AXIS_X = 0,
    AMOTION_EVENT_AXIS_Y = 1,
    AMOTION_EVENT_AXIS_PRESSURE = 2,
    AMOTION_EVENT_AXIS_SIZE = 3,
    AMOTION_EVENT_AXIS_TOUCH_MAJOR = 4,
    AMOTION_EVENT_AXIS_TOUCH_MINOR = 5,
    AMOTION_EVENT_AXIS_TOOL_MAJOR = 6,
    AMOTION_EVENT_AXIS_TOOL_MINOR = 7,
    AMOTION_EVENT_AXIS_ORIENTATION = 8,
    AMOTION_EVENT_AXIS_VSCROLL = 9,
    AMOTION_EVENT_AXIS_HSCROLL = 10,
};

/*
 * Pointer coordinate data.
 */
struct PointerCoords {
    enum { MAX_AXES = 30 }; // 30 so that sizeof(PointerCoords) == 128
    // Bitfield of axes that are present in this structure.
    uint64_t bits __attribute__((aligned(8)));
    // Values of axes that are stored in this structure packed in order by axis id
    // for each axis that is present in the structure according to 'bits'.
    float values[MAX_AXES];
    inline void clear() {
        BitSet64::clear(bits);
    }
    bool isEmpty() const {
        return BitSet64::isEmpty(bits);
    }
    float getAxisValue(int32_t axis) const;
    status_t setAxisValue(int32_t axis, float value);
    void scale(float scale);
    void applyOffset(float xOffset, float yOffset);
    inline float getX() const {
        return getAxisValue(AMOTION_EVENT_AXIS_X);
    }
    inline float getY() const {
        return getAxisValue(AMOTION_EVENT_AXIS_Y);
    }
    status_t readFromParcel(Parcel* parcel);
    status_t writeToParcel(Parcel* parcel) const;
    bool operator==(const PointerCoords& other) const;
    inline bool operator!=(const PointerCoords& other) const {
        return !(*this == other);
    }
    void copyFrom(const PointerCoords& other);
private:
    void tooManyAxes(int axis);
};

} // namespace android

#endif // INPUT_H
