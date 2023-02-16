#ifndef __SIMPLE_TEXT_INPUT_EX_PROTOCL_H__
#define  __SIMPLE_TEXT_INPUT_EX_PROTOCL_H__

#include <UefiBaseType.h>
#include <Protocol/SimpleTextInput.h>

#define EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL_GUID \
  { \
    0xdd9e7534, 0x7762, 0x4698, {0x8c, 0x14, 0xf5, 0x85, 0x17, 0xa6, 0x25, 0xaa} \
  }

struct _EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL;

typedef UINT8 EFI_KEY_TOGGLE_STATE;

typedef struct {
    UINT32 KeyShiftState; // Shiftキーの押下状態
    EFI_KEY_TOGGLE_STATE KeyToggleState; // ScrollLock, NumLock, CapsLockの状態
} EFI_KEY_STATE;

typedef struct {
    EFI_INPUT_KEY Key;
    EFI_KEY_STATE KeyState;
} EFI_KEY_DATA;

typedef 
EFI_STATUS
(EFIAPI *EFI_INPUT_RESET_EX) (
    IN struct _EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
    IN BOOLEAN ExtendedVerification
);

typedef 
EFI_STATUS
(EFIAPI *EFI_INPUT_READ_KEY_EX) (
    IN struct _EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
    OUT EFI_KEY_DATA *KeyData
);

typedef 
EFI_STATUS
(EFIAPI *EFI_SET_STATE) (
    IN struct _EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
    IN EFI_KEY_TOGGLE_STATE *KeyToggleState
);

typedef 
EFI_STATUS
(EFIAPI *EFI_KEY_NOTIFY_FUNCTION) (
    IN EFI_KEY_DATA *KeyData
);

/*
@param KeyData どのキーを押したら関数を呼び出すかを指定する
@prarm KeyNotificationFunction 関数へのポインタ
@param NotifyHandle uniqueなhandleが変える。登録解除用に使用する。
*/
typedef 
EFI_STATUS
(EFIAPI *EFI_REGISTER_KEYSTROKE_NOTIFY) (
    IN struct _EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
    IN EFI_KEY_DATA *KeyData,
    IN EFI_KEY_NOTIFY_FUNCTION KeyNotificationFunction,
    OUT VOID **NotifyHandle
);

typedef 
EFI_STATUS
(EFIAPI *EFI_UNREGISTER_KEYSTROKE_NOTIFY) (
    IN struct _EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
    IN VOID *NotificationHandle
);

typedef struct _EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL{
    EFI_INPUT_RESET_EX Reset;
    EFI_INPUT_READ_KEY_EX ReadKeyStrokeEx;
    EFI_EVENT WaitForKeyEx;
    EFI_SET_STATE SetState;
    EFI_REGISTER_KEYSTROKE_NOTIFY RegisterKeyNotify;
    EFI_UNREGISTER_KEYSTROKE_NOTIFY UnregisterKeyNotify;
} EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL;

extern EFI_GUID gEfiSimpleTextInputExProtocolGuid;

#endif