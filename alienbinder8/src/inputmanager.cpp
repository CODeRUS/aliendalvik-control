#include "inputmanager.h"
#include "parcel.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QTimer>

#include "input.h"

#define IM_SERVICE_NAME "input"
#define IM_INTERFACE_NAME "android.hardware.input.IInputManager"

static InputManager *s_instance = nullptr;

InputManager::InputManager(QObject *parent, const char *loggingCategoryName)
    : BinderInterfaceAbstract(IM_SERVICE_NAME,
                              IM_INTERFACE_NAME,
                              parent,
                              loggingCategoryName)
{

}

InputManager::~InputManager()
{
    if (s_instance == this) {
        s_instance = nullptr;
    }
}

InputManager *InputManager::GetInstance()
{
    if (!s_instance) {
        s_instance = new InputManager(qApp);
    }
    return s_instance;
}

void InputManager::sendTap(int posx, int posy, quint64 uptime)
{
    InputManager::injectTapEvent(0, posx, posy, 1.0f, uptime + 10);
    InputManager::injectTapEvent(1, posx, posy, 0.0f, uptime + 20);
}

void InputManager::injectTapEvent(int action, float posx, float posy, float pressure, quint64 uptime)
{
    InputManager *manager = InputManager::GetInstance();
    qCDebug(manager->logging) << Q_FUNC_INFO << action << posx << posy << pressure << uptime;

    QSharedPointer<Parcel> parcel = manager->createTransaction();
    parcel->writeInt(1); // have event
    parcel->writeInt(1); // PARCEL_TOKEN_MOTION_EVENT

    parcel->writeInt(1); // pointerCount
    parcel->writeInt(1); // sampleCount
    parcel->writeInt(1); // mDeviceId
    parcel->writeInt(0x1002); // mSource
    parcel->writeInt(action); // mAction
    parcel->writeInt(0); // mActionButton
    parcel->writeInt(0); // mFlags
    parcel->writeInt(0); // mEdgeFlags
    parcel->writeInt(0); // mMetaState
    parcel->writeInt(0); // mButtonState
    parcel->writeFloat(0.0f); // mXOffset
    parcel->writeFloat(0.0f); // mYOffset
    parcel->writeFloat(1.0f); // mXPrecision
    parcel->writeFloat(1.0f); // mYPrecision
    parcel->writeInt64(uptime * 1000000); // mDownTime
    parcel->writeInt(0); // properties.id
    parcel->writeInt(0); // properties.toolType
    parcel->writeInt64(uptime * 1000000); // mSampleEventTimes

    android::PointerCoords pointerCoords;
    pointerCoords.clear();

    pointerCoords.setAxisValue(android::AMOTION_EVENT_AXIS_X, posx);
    pointerCoords.setAxisValue(android::AMOTION_EVENT_AXIS_Y, posy);
    pointerCoords.setAxisValue(android::AMOTION_EVENT_AXIS_PRESSURE, pressure);
    pointerCoords.setAxisValue(android::AMOTION_EVENT_AXIS_SIZE, 1.0f);

    pointerCoords.writeToParcel(parcel.data());

//    enum { MAX_AXES = 30 }; // 30 so that sizeof(PointerCoords) == 128
//    // Bitfield of axes that are present in this structure.
//    uint64_t bits __attribute__((aligned(8)));
//    // Values of axes that are stored in this structure packed in order by axis id
//    // for each axis that is present in the structure according to 'bits'.
//    float values[MAX_AXES];

//    const float size = 1.0f;
//    float inputValues[] = {posx, posy, pressure, size};

//    BitSet64::clear(bits);

//    for (int axis = 0; axis < 4; axis++) {
//        float value = inputValues[axis];
//        uint32_t index = BitSet64::getIndexOfBit(bits, axis);
//        if (!BitSet64::hasBit(bits, axis)) {
//            if (value == 0) {
//                continue; // axes with value 0 do not need to be stored
//            }
//            uint32_t count = BitSet64::count(bits);
//            if (count >= MAX_AXES) {
//                continue;
//            }
//            BitSet64::markBit(bits, axis);
//            for (uint32_t i = count; i > index; i--) {
//                values[i] = values[i - 1];
//            }
//        }
//        values[index] = value;
//    }
//    parcel->writeInt64(bits); // values bits

//    uint32_t count = BitSet64::count(bits);
//    for (uint32_t i = 0; i < count; i++) {
//        parcel->writeFloat(values[i]);
//    }

    parcel->writeInt(2); // mode

    int status = 0;
    QSharedPointer<Parcel> out = manager->sendTransaction(TRANSACTION_injectInputEvent, parcel, &status);
    qCDebug(manager->logging) << Q_FUNC_INFO << "Status:" << status;
    if (status < 0) {
        return;
    }
    const int exception = out->readInt();
    if (exception != 0) {
        qCCritical(manager->logging) << Q_FUNC_INFO << "Exception:" << exception;
        return;
    }
    const int result = out->readInt();
    qCDebug(manager->logging) << Q_FUNC_INFO << "Result:" << result;
}
