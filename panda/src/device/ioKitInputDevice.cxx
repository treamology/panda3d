/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file ioKitInputDevice.cxx
 * @author rdb
 * @date 2017-12-21
 */

#include "ioKitInputDevice.h"

#if defined(__APPLE__) && !defined(CPPPARSER)

#include <IOKit/hid/IOHIDElement.h>

#include "keyboardButton.h"
#include "gamepadButton.h"
#include "mouseButton.h"

static void removal_callback(void *ctx, IOReturn result, void *sender) {
  IOKitInputDevice *input_device = (IOKitInputDevice *)ctx;
  nassertv(input_device != nullptr);
  input_device->on_remove();
}

/**
 * Protected constructor.
 */
IOKitInputDevice::
IOKitInputDevice(IOHIDDeviceRef device) :
  _device(device),
  _hat_element(nullptr),
  _pointer_x(nullptr),
  _pointer_y(nullptr),
  _scroll_wheel(nullptr),
  _pointer_x_timestamp(0),
  _pointer_y_timestamp(0),
  _scroll_wheel_timestamp(0) {
  nassertv(device);

  char buffer[4096];

  CFStringRef name = (CFStringRef)IOHIDDeviceGetProperty(device, CFSTR(kIOHIDProductKey));
  if (name) {
    CFStringGetCString(name, buffer, sizeof(buffer), kCFStringEncodingUTF8);
    _name = buffer;
  }

  CFStringRef mfg = (CFStringRef)IOHIDDeviceGetProperty(device, CFSTR(kIOHIDManufacturerKey));
  if (mfg) {
    CFStringGetCString(mfg, buffer, sizeof(buffer), kCFStringEncodingUTF8);
    _manufacturer = buffer;
  }

  CFStringRef serial = (CFStringRef)IOHIDDeviceGetProperty(device, CFSTR(kIOHIDSerialNumberKey));
  if (serial) {
    CFStringGetCString(serial, buffer, sizeof(buffer), kCFStringEncodingUTF8);
    _serial_number = buffer;
  }

  CFNumberRef vendor = (CFNumberRef)IOHIDDeviceGetProperty(device, CFSTR(kIOHIDVendorIDKey));
  if (vendor) {
    int32_t value = 0;
    CFNumberGetValue(vendor, kCFNumberSInt32Type, &value);
    _vendor_id = (unsigned short)value;
  }

  CFNumberRef product = (CFNumberRef)IOHIDDeviceGetProperty(device, CFSTR(kIOHIDProductIDKey));
  if (product) {
    int32_t value = 0;
    CFNumberGetValue(product, kCFNumberSInt32Type, &value);
    _product_id = (unsigned short)value;
  }

  if (IOHIDDeviceConformsTo(device, kHIDPage_GenericDesktop, kHIDUsage_GD_Mouse)) {
    _device_class = DC_mouse;
  } else if (IOHIDDeviceConformsTo(device, kHIDPage_GenericDesktop, kHIDUsage_GD_Keyboard)) {
    _device_class = DC_keyboard;
  } else if (IOHIDDeviceConformsTo(device, kHIDPage_GenericDesktop, kHIDUsage_GD_GamePad)) {
    _device_class = DC_gamepad;
  } else if (IOHIDDeviceConformsTo(device, kHIDPage_Simulation, kHIDUsage_Sim_FlightStick)) {
    _device_class = DC_flight_stick;
  } else if (IOHIDDeviceConformsTo(device, kHIDPage_Simulation, kHIDUsage_GD_Joystick)) {
    _device_class = DC_flight_stick;
  } else if (_vendor_id == 0x044f && _product_id == 0xb108) {
    // T.Flight Hotas X
    _device_class = DC_flight_stick;
  }

  CFArrayRef elements = IOHIDDeviceCopyMatchingElements(device, NULL, 0);
  CFIndex count = CFArrayGetCount(elements);
  for (CFIndex i = 0; i < count; ++i) {
    IOHIDElementRef element = (IOHIDElementRef)CFArrayGetValueAtIndex(elements, i);
    parse_element(element);
  }
  CFRelease(elements);

  if (_hat_element != nullptr) {
    _hat_x_axis = _controls.size();
    add_control(C_hat_x, -1, 1, true);
    add_control(C_hat_y, -1, 1, true);
  }

  if (_pointer_x != nullptr && _pointer_y != nullptr) {
    _flags |= IDF_has_pointer;
  }

  _is_connected = true;
  IOHIDDeviceRegisterRemovalCallback(device, removal_callback, this);
}

/**
 *
 */
IOKitInputDevice::
~IOKitInputDevice() {
}

/**
 * The nonstatic version of on_remove_device.
 */
void IOKitInputDevice::
on_remove() {
  {
    LightMutexHolder holder(_lock);
    if (!_is_connected) {
      return;
    }
    _is_connected = false;
  }

  if (device_cat.is_debug()) {
    device_cat.debug()
      << "Removed input device " << *this << "\n";
  }

  IOHIDDeviceClose(_device, kIOHIDOptionsTypeNone);

  InputDeviceManager *mgr = InputDeviceManager::get_global_ptr();
  nassertv(mgr != nullptr);
  mgr->remove_device(this);
}

/**
 *
 */
void IOKitInputDevice::
parse_element(IOHIDElementRef element) {
  ButtonHandle handle = ButtonHandle::none();
  ControlAxis axis = C_none;
  uint32_t page = IOHIDElementGetUsagePage(element);
  uint32_t usage = IOHIDElementGetUsage(element);

  switch (IOHIDElementGetType(element)) {
  case kIOHIDElementTypeInput_Misc:
    switch (page) {
    case kHIDPage_GenericDesktop:
      switch (usage) {
      case kHIDUsage_GD_X:
        if (_device_class == DC_gamepad) {
          axis = C_left_x;
        } else if (_device_class == DC_mouse) {
          _pointer_x = element;
          return;
        } else {
          axis = C_x;
        }
        break;
      case kHIDUsage_GD_Y:
        if (_device_class == DC_gamepad) {
          axis = C_left_y;
        } else if (_device_class == DC_mouse) {
          _pointer_y = element;
          return;
        } else {
          axis = C_y;
        }
        break;
      case kHIDUsage_GD_Z:
        if (_device_class == DC_gamepad) {
          axis = C_left_trigger;
        } else {
          axis = C_throttle;
        }
        break;
      case kHIDUsage_GD_Rx:
        axis = C_right_x;
        break;
      case kHIDUsage_GD_Ry:
        axis = C_right_y;
        break;
      case kHIDUsage_GD_Rz:
        if (_device_class == DC_gamepad) {
          axis = C_right_trigger;
        } else {
          axis = C_twist;
        }
        break;
      case kHIDUsage_GD_Slider:
        axis = C_rudder;
        break;
      case kHIDUsage_GD_Dial:
        break;
      case kHIDUsage_GD_Wheel:
        _scroll_wheel = element;
        return;
      case kHIDUsage_GD_Hatswitch:
        _hat_element = element;
        return;
      case kHIDUsage_GD_DPadUp:
        handle = GamepadButton::dpad_up();
        break;
      case kHIDUsage_GD_DPadDown:
        handle = GamepadButton::dpad_down();
        break;
      case kHIDUsage_GD_DPadRight:
        handle = GamepadButton::dpad_right();
        break;
      case kHIDUsage_GD_DPadLeft:
        handle = GamepadButton::dpad_left();
        break;
      case 0xffffffffu:
        return;
      default:
        break;
      }
      break;

    case kHIDPage_Simulation:
      switch (usage) {
      case kHIDUsage_Sim_Rudder:
        axis = C_rudder;
        break;
      case kHIDUsage_Sim_Throttle:
        axis = C_throttle;
        break;
      case kHIDUsage_Sim_Accelerator:
        axis = C_accelerator;
        break;
      case kHIDUsage_Sim_Brake:
        axis = C_brake;
        break;
      }
      break;
    }
    if (axis != C_none) {
      int min = IOHIDElementGetLogicalMin(element);
      int max = IOHIDElementGetLogicalMax(element);
      if (_vendor_id == 0x044f && _product_id == 0xb108 && axis == C_throttle) {
        // T.Flight Hotas X throttle is reversed and can go backwards.
        add_control(axis, max, min, true);
      } else if (axis == C_y || axis == C_left_y || axis == C_right_y) {
        // We'd like to reverse the Y axis to match the XInput behavior.
        add_control(axis, max, min);
      } else {
        add_control(axis, min, max);
      }

      _analog_elements.push_back(element);
    }
    break;

  case kIOHIDElementTypeInput_Button:
    switch (page) {
    case kHIDPage_GenericDesktop:
      switch (usage) {
      case kHIDUsage_GD_DPadUp:
        handle = GamepadButton::dpad_up();
        break;
      case kHIDUsage_GD_DPadDown:
        handle = GamepadButton::dpad_down();
        break;
      case kHIDUsage_GD_DPadRight:
        handle = GamepadButton::dpad_right();
        break;
      case kHIDUsage_GD_DPadLeft:
        handle = GamepadButton::dpad_left();
        break;
      default:
        break;
      }
      break;

    case kHIDPage_KeyboardOrKeypad:
      switch (usage) {
      case kHIDUsage_KeyboardA:
      case kHIDUsage_KeyboardB:
      case kHIDUsage_KeyboardC:
      case kHIDUsage_KeyboardD:
      case kHIDUsage_KeyboardE:
      case kHIDUsage_KeyboardF:
      case kHIDUsage_KeyboardG:
      case kHIDUsage_KeyboardH:
      case kHIDUsage_KeyboardI:
      case kHIDUsage_KeyboardJ:
      case kHIDUsage_KeyboardK:
      case kHIDUsage_KeyboardL:
      case kHIDUsage_KeyboardM:
      case kHIDUsage_KeyboardN:
      case kHIDUsage_KeyboardO:
      case kHIDUsage_KeyboardP:
      case kHIDUsage_KeyboardQ:
      case kHIDUsage_KeyboardR:
      case kHIDUsage_KeyboardS:
      case kHIDUsage_KeyboardT:
      case kHIDUsage_KeyboardU:
      case kHIDUsage_KeyboardV:
      case kHIDUsage_KeyboardW:
      case kHIDUsage_KeyboardX:
      case kHIDUsage_KeyboardY:
      case kHIDUsage_KeyboardZ:
        handle = KeyboardButton::ascii_key('a' + (usage - kHIDUsage_KeyboardA));
        break;
      case kHIDUsage_Keyboard1:
        handle = KeyboardButton::ascii_key('1');
        break;
      case kHIDUsage_Keyboard2:
        handle = KeyboardButton::ascii_key('2');
        break;
      case kHIDUsage_Keyboard3:
        handle = KeyboardButton::ascii_key('3');
        break;
      case kHIDUsage_Keyboard4:
        handle = KeyboardButton::ascii_key('4');
        break;
      case kHIDUsage_Keyboard5:
        handle = KeyboardButton::ascii_key('5');
        break;
      case kHIDUsage_Keyboard6:
        handle = KeyboardButton::ascii_key('6');
        break;
      case kHIDUsage_Keyboard7:
        handle = KeyboardButton::ascii_key('7');
        break;
      case kHIDUsage_Keyboard8:
        handle = KeyboardButton::ascii_key('8');
        break;
      case kHIDUsage_Keyboard9:
        handle = KeyboardButton::ascii_key('9');
        break;
      case kHIDUsage_Keyboard0:
        handle = KeyboardButton::ascii_key('0');
        break;
      case kHIDUsage_KeyboardReturnOrEnter:
        handle = KeyboardButton::enter();
        break;
      case kHIDUsage_KeyboardEscape:
        handle = KeyboardButton::escape();
        break;
      case kHIDUsage_KeyboardDeleteOrBackspace:
        handle = KeyboardButton::backspace();
        break;
      case kHIDUsage_KeyboardTab:
        handle = KeyboardButton::tab();
        break;
      case kHIDUsage_KeyboardSpacebar:
        handle = KeyboardButton::ascii_key(' ');
        break;
      case kHIDUsage_KeyboardHyphen:
        handle = KeyboardButton::ascii_key('-');
        break;
      case kHIDUsage_KeyboardEqualSign:
        handle = KeyboardButton::ascii_key('=');
        break;
      case kHIDUsage_KeyboardOpenBracket:
        handle = KeyboardButton::ascii_key('[');
        break;
      case kHIDUsage_KeyboardCloseBracket:
        handle = KeyboardButton::ascii_key(']');
        break;
      case kHIDUsage_KeyboardBackslash:
        handle = KeyboardButton::ascii_key('\\');
        break;
      case kHIDUsage_KeyboardNonUSPound:
        handle = KeyboardButton::ascii_key('$');
        break;
      case kHIDUsage_KeyboardSemicolon:
        handle = KeyboardButton::ascii_key(';');
        break;
      case kHIDUsage_KeyboardQuote:
        handle = KeyboardButton::ascii_key('\'');
        break;
      case kHIDUsage_KeyboardGraveAccentAndTilde:
        handle = KeyboardButton::ascii_key('`');
        break;
      case kHIDUsage_KeyboardComma:
        handle = KeyboardButton::ascii_key(',');
        break;
      case kHIDUsage_KeyboardPeriod:
        handle = KeyboardButton::ascii_key('.');
        break;
      case kHIDUsage_KeyboardSlash:
        handle = KeyboardButton::ascii_key('/');
        break;
      case kHIDUsage_KeyboardCapsLock:
        handle = KeyboardButton::caps_lock();
        break;
      case kHIDUsage_KeyboardF1:
        handle = KeyboardButton::f1();
        break;
      case kHIDUsage_KeyboardF2:
        handle = KeyboardButton::f2();
        break;
      case kHIDUsage_KeyboardF3:
        handle = KeyboardButton::f3();
        break;
      case kHIDUsage_KeyboardF4:
        handle = KeyboardButton::f4();
        break;
      case kHIDUsage_KeyboardF5:
        handle = KeyboardButton::f5();
        break;
      case kHIDUsage_KeyboardF6:
        handle = KeyboardButton::f6();
        break;
      case kHIDUsage_KeyboardF7:
        handle = KeyboardButton::f7();
        break;
      case kHIDUsage_KeyboardF8:
        handle = KeyboardButton::f8();
        break;
      case kHIDUsage_KeyboardF9:
        handle = KeyboardButton::f9();
        break;
      case kHIDUsage_KeyboardF10:
        handle = KeyboardButton::f10();
        break;
      case kHIDUsage_KeyboardF11:
        handle = KeyboardButton::f11();
        break;
      case kHIDUsage_KeyboardF12:
        handle = KeyboardButton::f12();
        break;
      case kHIDUsage_KeyboardPrintScreen:
        handle = KeyboardButton::print_screen();
        break;
      case kHIDUsage_KeyboardScrollLock:
        handle = KeyboardButton::scroll_lock();
        break;
      case kHIDUsage_KeyboardPause:
        handle = KeyboardButton::pause();
        break;
      case kHIDUsage_KeyboardInsert:
        handle = KeyboardButton::insert();
        break;
      case kHIDUsage_KeyboardHome:
        handle = KeyboardButton::home();
        break;
      case kHIDUsage_KeyboardPageUp:
        handle = KeyboardButton::page_up();
        break;
      case kHIDUsage_KeyboardDeleteForward:
        handle = KeyboardButton::del();
        break;
      case kHIDUsage_KeyboardEnd:
        handle = KeyboardButton::end();
        break;
      case kHIDUsage_KeyboardPageDown:
        handle = KeyboardButton::page_down();
        break;
      case kHIDUsage_KeyboardRightArrow:
        handle = KeyboardButton::right();
        break;
      case kHIDUsage_KeyboardLeftArrow:
        handle = KeyboardButton::left();
        break;
      case kHIDUsage_KeyboardDownArrow:
        handle = KeyboardButton::down();
        break;
      case kHIDUsage_KeyboardUpArrow:
        handle = KeyboardButton::up();
        break;
      case kHIDUsage_KeypadNumLock:
        handle = KeyboardButton::num_lock();
        break;
      case kHIDUsage_KeypadSlash:
        handle = KeyboardButton::ascii_key('/');
        break;
      case kHIDUsage_KeypadAsterisk:
        handle = KeyboardButton::ascii_key('*');
        break;
      case kHIDUsage_KeypadHyphen:
        handle = KeyboardButton::ascii_key('-');
        break;
      case kHIDUsage_KeypadPlus:
        handle = KeyboardButton::ascii_key('+');
        break;
      case kHIDUsage_KeypadEnter:
        handle = KeyboardButton::enter();
        break;
      case kHIDUsage_Keypad1:
        handle = KeyboardButton::ascii_key('1');
        break;
      case kHIDUsage_Keypad2:
        handle = KeyboardButton::ascii_key('2');
        break;
      case kHIDUsage_Keypad3:
        handle = KeyboardButton::ascii_key('3');
        break;
      case kHIDUsage_Keypad4:
        handle = KeyboardButton::ascii_key('4');
        break;
      case kHIDUsage_Keypad5:
        handle = KeyboardButton::ascii_key('5');
        break;
      case kHIDUsage_Keypad6:
        handle = KeyboardButton::ascii_key('6');
        break;
      case kHIDUsage_Keypad7:
        handle = KeyboardButton::ascii_key('7');
        break;
      case kHIDUsage_Keypad8:
        handle = KeyboardButton::ascii_key('8');
        break;
      case kHIDUsage_Keypad9:
        handle = KeyboardButton::ascii_key('9');
        break;
      case kHIDUsage_Keypad0:
        handle = KeyboardButton::ascii_key('0');
        break;
      case kHIDUsage_KeypadPeriod:
        handle = KeyboardButton::ascii_key('.');
        break;
      case kHIDUsage_KeyboardNonUSBackslash:
        handle = KeyboardButton::ascii_key('\\');
        break;
      case kHIDUsage_KeypadEqualSign:
        handle = KeyboardButton::ascii_key('=');
        break;
      case kHIDUsage_KeyboardF13:
        handle = KeyboardButton::f13();
        break;
      case kHIDUsage_KeyboardF14:
        handle = KeyboardButton::f14();
        break;
      case kHIDUsage_KeyboardF15:
        handle = KeyboardButton::f15();
        break;
      case kHIDUsage_KeyboardF16:
        handle = KeyboardButton::f16();
        break;
      case kHIDUsage_KeyboardExecute:
        break;
      case kHIDUsage_KeyboardHelp:
        handle = KeyboardButton::help();
        break;
      case kHIDUsage_KeyboardMenu:
        handle = KeyboardButton::menu();
        break;
      case kHIDUsage_KeypadComma:
        handle = KeyboardButton::ascii_key(',');
        break;
      case kHIDUsage_KeypadEqualSignAS400:
        handle = KeyboardButton::ascii_key('=');
        break;
      case kHIDUsage_KeyboardReturn:
        handle = KeyboardButton::enter();
        break;
      case kHIDUsage_KeyboardLeftControl:
        handle = KeyboardButton::lcontrol();
        break;
      case kHIDUsage_KeyboardLeftShift:
        handle = KeyboardButton::lshift();
        break;
      case kHIDUsage_KeyboardLeftAlt:
        handle = KeyboardButton::lalt();
        break;
      case kHIDUsage_KeyboardLeftGUI:
        handle = KeyboardButton::lmeta();
        break;
      case kHIDUsage_KeyboardRightControl:
        handle = KeyboardButton::rcontrol();
        break;
      case kHIDUsage_KeyboardRightShift:
        handle = KeyboardButton::rshift();
        break;
      case kHIDUsage_KeyboardRightAlt:
        handle = KeyboardButton::ralt();
        break;
      case kHIDUsage_KeyboardRightGUI:
        handle = KeyboardButton::rmeta();
        break;
      default:
        break;
      }
      break;

    case kHIDPage_Button:
      if (_device_class == DC_gamepad) {
        // These seem to be the button mappings exposed by the 360Controller
        // driver I don't know if other drivers do the same thing at all.
        static const ButtonHandle gamepad_buttons[] = {
          ButtonHandle::none(),
          GamepadButton::action_a(),
          GamepadButton::action_b(),
          GamepadButton::action_x(),
          GamepadButton::action_y(),
          GamepadButton::lshoulder(),
          GamepadButton::rshoulder(),
          GamepadButton::lstick(),
          GamepadButton::rstick(),
          GamepadButton::start(),
          GamepadButton::back(),
          GamepadButton::guide(),
          GamepadButton::dpad_up(),
          GamepadButton::dpad_down(),
          GamepadButton::dpad_left(),
          GamepadButton::dpad_right(),
        };
        if (usage < sizeof(gamepad_buttons) / sizeof(ButtonHandle)) {
          handle = gamepad_buttons[usage];
        }
      } else if (_device_class == DC_mouse) {
        // In Panda, wheel and right button are flipped around...
        int button = (usage == 2 || usage == 3) ? (4 - usage) : (usage - 1);
        handle = MouseButton::button(button);
      }
      break;
    }
    _buttons.push_back(ButtonState(handle));
    _button_elements.push_back(element);
    break;

  case kIOHIDElementTypeInput_Axis:
    break;

  case kIOHIDElementTypeInput_ScanCodes:
    break;

  case kIOHIDElementTypeOutput:
    break;

  case kIOHIDElementTypeFeature:
    break;

  case kIOHIDElementTypeCollection:
    {
      // This doesn't seem to be necessary and instead leads to duplication of
      // axes and buttons.
      /*
      CFArrayRef children = IOHIDElementGetChildren(element);
      CFIndex count = CFArrayGetCount(children);
      for (CFIndex i = 0; i < count; ++i) {
        IOHIDElementRef element = (IOHIDElementRef)CFArrayGetValueAtIndex(children, i);
        parse_element(element, depth + 2);
      }*/
    }
    break;
  }
}

/**
 * Polls the input device for new activity, to ensure it contains the latest
 * events.  This will only have any effect for some types of input devices;
 * others may be updated automatically, and this method will be a no-op.
 */
void IOKitInputDevice::
do_poll() {
  for (size_t i = 0; i < _button_elements.size(); ++i) {
    IOHIDValueRef value_ref;
    if (!_button_elements[i]) continue;
    if (IOHIDDeviceGetValue(_device, _button_elements[i], &value_ref) == kIOReturnSuccess) {
      int value = IOHIDValueGetIntegerValue(value_ref);
      button_changed(i, value != 0);
    }
  }

  for (size_t i = 0; i < _analog_elements.size(); ++i) {
    IOHIDValueRef value_ref;
    if (IOHIDDeviceGetValue(_device, _analog_elements[i], &value_ref) == kIOReturnSuccess) {
      int value = IOHIDValueGetIntegerValue(value_ref);
      control_changed(i, value);
    }
  }

  if (_hat_element != nullptr) {
    IOHIDValueRef value_ref;
    if (IOHIDDeviceGetValue(_device, _hat_element, &value_ref) == kIOReturnSuccess) {
      int value = IOHIDValueGetIntegerValue(value_ref);
      int x = 0;
      int y = 0;
      switch (value) {
      case 0:
        x = 0;
        y = -1;
        break;
      case 1:
        x = 1;
        y = -1;
        break;
      case 2:
        x = 1;
        y = 0;
        break;
      case 3:
        x = 1;
        y = 1;
        break;
      case 4:
        x = 0;
        y = 1;
        break;
      case 5:
        x = -1;
        y = 1;
        break;
      case 6:
        x = -1;
        y = 0;
        break;
      case 7:
        x = -1;
        y = -1;
        break;
      default:
        break;
      }
      control_changed(_hat_x_axis, x);
      control_changed(_hat_x_axis + 1, y);
    }
  }

  int x = 0, y = 0;
  if (_pointer_x != nullptr) {
    IOHIDValueRef value_ref;
    if (IOHIDDeviceGetValue(_device, _pointer_x, &value_ref) == kIOReturnSuccess) {
      uint64_t timestamp = IOHIDValueGetTimeStamp(value_ref);
      if (timestamp != _pointer_x_timestamp) {
        x = IOHIDValueGetIntegerValue(value_ref);
        _pointer_x_timestamp = timestamp;
      }
    }
  }
  if (_pointer_y != nullptr) {
    IOHIDValueRef value_ref;
    if (IOHIDDeviceGetValue(_device, _pointer_y, &value_ref) == kIOReturnSuccess) {
      uint64_t timestamp = IOHIDValueGetTimeStamp(value_ref);
      if (timestamp != _pointer_y_timestamp) {
        y = IOHIDValueGetIntegerValue(value_ref);
        _pointer_y_timestamp = timestamp;
      }
    }
  }
  if (x != 0 || y != 0) {
    pointer_moved(x, y, ClockObject::get_global_clock()->get_frame_time());
  }

  // Do we have a scroll wheel axis?
  if (_scroll_wheel != nullptr) {
    IOHIDValueRef value_ref;
    if (IOHIDDeviceGetValue(_device, _scroll_wheel, &value_ref) == kIOReturnSuccess) {
      uint64_t timestamp = IOHIDValueGetTimeStamp(value_ref);
      if (timestamp != _scroll_wheel_timestamp) {
        int value = IOHIDValueGetIntegerValue(value_ref);
        if (value != 0) {
          // Just fire off a rapid down/up event.
          double time = ClockObject::get_global_clock()->get_frame_time();
          ButtonHandle handle = (value > 0) ? MouseButton::wheel_up() : MouseButton::wheel_down();
          _button_events->add_event(ButtonEvent(handle, ButtonEvent::T_down, time));
          _button_events->add_event(ButtonEvent(handle, ButtonEvent::T_up, time));
        }
        _scroll_wheel_timestamp = timestamp;
      }
    }
  }
}

#endif  // __APPLE__