/**
  @file Win32Window_di8.cpp

  @maintainer Morgan McGuire, matrix@graphics3d.com 
  @created 	  2004-10-07
  @edited  	  2004-10-07
    
*/

#ifndef G3D_WIN32
    #error This is a Win32-only file
#endif

#ifdef WIN32WINDOW_INCLUDE_DI8

// dinput.h replacements
namespace Win32Window_DI8 {

    //DirectInput8 defines
    #define DIRECTINPUT_VERSION 0x0800
    #include <initguid.h>

    /*
        These define the structures required to use IDirectInput8
        and IDirectInputDevice8.  The only ones left not commented
        out are the ones used.  Akward ordering is left in-case other
        structures are needed later.
    */

    // DIDEVICEINSTANCEA
    typedef struct DIDEVICEINSTANCEA {
        DWORD   dwSize;
        GUID    guidInstance;
        GUID    guidProduct;
        DWORD   dwDevType;
        CHAR    tszInstanceName[MAX_PATH];
        CHAR    tszProductName[MAX_PATH];
    #if(DIRECTINPUT_VERSION >= 0x0500)
        GUID    guidFFDriver;
        WORD    wUsagePage;
        WORD    wUsage;
    #endif // DIRECTINPUT_VERSION >= 0x0500
    } DIDEVICEINSTANCEA, *LPDIDEVICEINSTANCEA;

    typedef const DIDEVICEINSTANCEA *LPCDIDEVICEINSTANCEA;
    typedef BOOL (FAR PASCAL * LPDIENUMDEVICESCALLBACKA)(LPCDIDEVICEINSTANCEA, LPVOID);

    // DIDEVICEOBJECTINSTANCEA
//    typedef struct DIDEVICEOBJECTINSTANCEA {
//        DWORD   dwSize;
//        GUID    guidType;
//        DWORD   dwOfs;
//        DWORD   dwType;
//        DWORD   dwFlags;
//        CHAR    tszName[MAX_PATH];
//    #if(DIRECTINPUT_VERSION >= 0x0500)
//        DWORD   dwFFMaxForce;
//        DWORD   dwFFForceResolution;
//        WORD    wCollectionNumber;
//        WORD    wDesignatorIndex;
//        WORD    wUsagePage;
//        WORD    wUsage;
//        DWORD   dwDimension;
//        WORD    wExponent;
//        WORD    wReportId;
//    #endif /* DIRECTINPUT_VERSION >= 0x0500 */
//    } DIDEVICEOBJECTINSTANCEA, *LPDIDEVICEOBJECTINSTANCEA;
//    typedef DIDEVICEOBJECTINSTANCEA DIDEVICEOBJECTINSTANCE;
//    typedef LPDIDEVICEOBJECTINSTANCEA LPDIDEVICEOBJECTINSTANCE;
//    typedef const DIDEVICEOBJECTINSTANCEA *LPCDIDEVICEOBJECTINSTANCEA;

    // LPDIENUMDEVICEOBJECTSCALLBACKA
//    typedef BOOL (FAR PASCAL * LPDIENUMDEVICEOBJECTSCALLBACKA)(LPCDIDEVICEOBJECTINSTANCEA, LPVOID);
    

    // DIDEVCAPS
    typedef struct DIDEVCAPS {
        DWORD   dwSize;
        DWORD   dwFlags;
        DWORD   dwDevType;
        DWORD   dwAxes;
        DWORD   dwButtons;
        DWORD   dwPOVs;
    #if(DIRECTINPUT_VERSION >= 0x0500)
        DWORD   dwFFSamplePeriod;
        DWORD   dwFFMinTimeResolution;
        DWORD   dwFirmwareRevision;
        DWORD   dwHardwareRevision;
        DWORD   dwFFDriverVersion;
    #endif // DIRECTINPUT_VERSION >= 0x0500
    } DIDEVCAPS, *LPDIDEVCAPS;


    // DIPROPHEADER
//    typedef struct DIPROPHEADER {
//        DWORD   dwSize;
//        DWORD   dwHeaderSize;
//        DWORD   dwObj;
//        DWORD   dwHow;
//    } DIPROPHEADER, *LPDIPROPHEADER;
//    typedef const DIPROPHEADER *LPCDIPROPHEADER;


    // DIDEVICEOBJECTDATA
//    typedef struct DIDEVICEOBJECTDATA {
//        DWORD       dwOfs;
//        DWORD       dwData;
//        DWORD       dwTimeStamp;
//        DWORD       dwSequence;
//    #if(DIRECTINPUT_VERSION >= 0x0800)
//        UINT_PTR    uAppData;
//    #endif // DIRECTINPUT_VERSION >= 0x0800
//    } DIDEVICEOBJECTDATA, *LPDIDEVICEOBJECTDATA;
//    typedef const DIDEVICEOBJECTDATA *LPCDIDEVICEOBJECTDATA;


    // DIOBJECTDATAFORMAT
    typedef struct _DIOBJECTDATAFORMAT {
        const GUID *pguid;
        DWORD   dwOfs;
        DWORD   dwType;
        DWORD   dwFlags;
    } DIOBJECTDATAFORMAT, *LPDIOBJECTDATAFORMAT;
    typedef const DIOBJECTDATAFORMAT *LPCDIOBJECTDATAFORMAT;


    // DIDATAFORMAT
    typedef struct _DIDATAFORMAT {
        DWORD   dwSize;
        DWORD   dwObjSize;
        DWORD   dwFlags;
        DWORD   dwDataSize;
        DWORD   dwNumObjs;
        LPDIOBJECTDATAFORMAT rgodf;
    } DIDATAFORMAT, *LPDIDATAFORMAT;
    typedef const DIDATAFORMAT *LPCDIDATAFORMAT;


    // DIENVELOPE
//    typedef struct DIENVELOPE {
//        DWORD dwSize;                   /* sizeof(DIENVELOPE)   */
//        DWORD dwAttackLevel;
//        DWORD dwAttackTime;             /* Microseconds         */
//        DWORD dwFadeLevel;
//        DWORD dwFadeTime;               /* Microseconds         */
//    } DIENVELOPE, *LPDIENVELOPE;
//    typedef const DIENVELOPE *LPCDIENVELOPE;


    // DIEFFECT
//    typedef struct DIEFFECT {
//        DWORD dwSize;                   /* sizeof(DIEFFECT)     */
//        DWORD dwFlags;                  /* DIEFF_*              */
//        DWORD dwDuration;               /* Microseconds         */
//        DWORD dwSamplePeriod;           /* Microseconds         */
//        DWORD dwGain;
//        DWORD dwTriggerButton;          /* or DIEB_NOTRIGGER    */
//        DWORD dwTriggerRepeatInterval;  /* Microseconds         */
//        DWORD cAxes;                    /* Number of axes       */
//        LPDWORD rgdwAxes;               /* Array of axes        */
//        LPLONG rglDirection;            /* Array of directions  */
//        LPDIENVELOPE lpEnvelope;        /* Optional             */
//        DWORD cbTypeSpecificParams;     /* Size of params       */
//        LPVOID lpvTypeSpecificParams;   /* Pointer to params    */
//    #if(DIRECTINPUT_VERSION >= 0x0600)
//        DWORD  dwStartDelay;            /* Microseconds         */
//    #endif /* DIRECTINPUT_VERSION >= 0x0600 */
//    } DIEFFECT, *LPDIEFFECT;
//    typedef DIEFFECT DIEFFECT_DX6;
//    typedef LPDIEFFECT LPDIEFFECT_DX6;
//    typedef const DIEFFECT *LPCDIEFFECT;


    // DIEFFESCAPE
//    typedef struct DIEFFESCAPE {
//        DWORD   dwSize;
//        DWORD   dwCommand;
//        LPVOID  lpvInBuffer;
//        DWORD   cbInBuffer;
//        LPVOID  lpvOutBuffer;
//        DWORD   cbOutBuffer;
//    } DIEFFESCAPE, *LPDIEFFESCAPE;


    // IDirectInputEffect
//    #undef INTERFACE
//    #define INTERFACE IDirectInputEffect

//    DECLARE_INTERFACE_(IDirectInputEffect, IUnknown)
//    {
//        /*** IUnknown methods ***/
//        STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE;
//        STDMETHOD_(ULONG,AddRef)(THIS) PURE;
//        STDMETHOD_(ULONG,Release)(THIS) PURE;

//        /*** IDirectInputEffect methods ***/
//        STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD,REFGUID) PURE;
//        STDMETHOD(GetEffectGuid)(THIS_ LPGUID) PURE;
//        STDMETHOD(GetParameters)(THIS_ LPDIEFFECT,DWORD) PURE;
//        STDMETHOD(SetParameters)(THIS_ LPCDIEFFECT,DWORD) PURE;
//        STDMETHOD(Start)(THIS_ DWORD,DWORD) PURE;
//        STDMETHOD(Stop)(THIS) PURE;
//        STDMETHOD(GetEffectStatus)(THIS_ LPDWORD) PURE;
//        STDMETHOD(Download)(THIS) PURE;
//        STDMETHOD(Unload)(THIS) PURE;
//        STDMETHOD(Escape)(THIS_ LPDIEFFESCAPE) PURE;
//    };
//    typedef struct IDirectInputEffect *LPDIRECTINPUTEFFECT;


    // DIEFFECTINFOA
//    typedef struct DIEFFECTINFOA {
//        DWORD   dwSize;
//        GUID    guid;
//        DWORD   dwEffType;
//        DWORD   dwStaticParams;
//        DWORD   dwDynamicParams;
//        CHAR    tszName[MAX_PATH];
//    } DIEFFECTINFOA, *LPDIEFFECTINFOA;
//    typedef DIEFFECTINFOA DIEFFECTINFO;
//    typedef LPDIEFFECTINFOA LPDIEFFECTINFO;
//    typedef const DIEFFECTINFOA *LPCDIEFFECTINFOA;
//    typedef const DIEFFECTINFO  *LPCDIEFFECTINFO;


    // DIENUMEFFECTSCALLBACKA
//    typedef BOOL (FAR PASCAL * LPDIENUMEFFECTSCALLBACKA)(LPCDIEFFECTINFOA, LPVOID);

    // DIENUMCREATEEFFECTOBJECTSCALLBACK
//    typedef BOOL (FAR PASCAL * LPDIENUMCREATEDEFFECTOBJECTSCALLBACK)(LPDIRECTINPUTEFFECT, LPVOID);


    // DIFILEEFFECT
//    typedef struct DIFILEEFFECT{
//        DWORD       dwSize;
//        GUID        GuidEffect;
//        LPCDIEFFECT lpDiEffect;
//        CHAR        szFriendlyName[MAX_PATH];
//    }DIFILEEFFECT, *LPDIFILEEFFECT;
//    typedef const DIFILEEFFECT *LPCDIFILEEFFECT;
//    typedef BOOL (FAR PASCAL * LPDIENUMEFFECTSINFILECALLBACK)(LPCDIFILEEFFECT , LPVOID);


    // DIACTIONA
//    typedef struct _DIACTIONA {
//                    UINT_PTR    uAppData;
//                    DWORD       dwSemantic;
//        OPTIONAL    DWORD       dwFlags;
//        OPTIONAL    union {
//                        LPCSTR      lptszActionName;
//                        UINT        uResIdString;
//                    };
//        OPTIONAL    GUID        guidInstance;
//        OPTIONAL    DWORD       dwObjID;
//        OPTIONAL    DWORD       dwHow;
//    } DIACTIONA, *LPDIACTIONA ;


    // DIACTIONFORMATA
//    typedef struct _DIACTIONFORMATA {
//                    DWORD       dwSize;
//                    DWORD       dwActionSize;
//                    DWORD       dwDataSize;
//                    DWORD       dwNumActions;
//                    LPDIACTIONA rgoAction;
//                    GUID        guidActionMap;
//                    DWORD       dwGenre;
//                    DWORD       dwBufferSize;
//        OPTIONAL    LONG        lAxisMin;
//        OPTIONAL    LONG        lAxisMax;
//        OPTIONAL    HINSTANCE   hInstString;
//                    FILETIME    ftTimeStamp;
//                    DWORD       dwCRC;
//                    CHAR        tszActionMap[MAX_PATH];
//    } DIACTIONFORMATA, *LPDIACTIONFORMATA;


    // DIDEVICEIMAGEINFOA
//    typedef struct _DIDEVICEIMAGEINFOA {
//        CHAR        tszImagePath[MAX_PATH];
//        DWORD       dwFlags; 
//        // These are valid if DIDIFT_OVERLAY is present in dwFlags.
//        DWORD       dwViewID;      
//        RECT        rcOverlay;             
//        DWORD       dwObjID;
//        DWORD       dwcValidPts;
//        POINT       rgptCalloutLine[5];  
//        RECT        rcCalloutRect;  
//        DWORD       dwTextAlign;     
//    } DIDEVICEIMAGEINFOA, *LPDIDEVICEIMAGEINFOA;


    // DIDEVICEIMAGEINFOHEADER
//    typedef struct _DIDEVICEIMAGEINFOHEADERA {
//        DWORD       dwSize;
//        DWORD       dwSizeImageInfo;
//        DWORD       dwcViews;
//        DWORD       dwcButtons;
//        DWORD       dwcAxes;
//        DWORD       dwcPOVs;
//        DWORD       dwBufferSize;
//        DWORD       dwBufferUsed;
//        LPDIDEVICEIMAGEINFOA lprgImageInfoArray;
//    } DIDEVICEIMAGEINFOHEADERA, *LPDIDEVICEIMAGEINFOHEADERA;


    /*
        IDirectInputDevice8A

        This defines the DirectInput8 interface needed to 
        access a direct input device.  
    */
    #undef INTERFACE
    #define INTERFACE IDirectInputDevice8A

    DECLARE_INTERFACE_(IDirectInputDevice8A, IUnknown)
    {
        //
        STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE;
        STDMETHOD_(ULONG,AddRef)(THIS) PURE;
        STDMETHOD_(ULONG,Release)(THIS) PURE;

        //
/*
        STDMETHOD(GetCapabilities)(THIS_ LPDIDEVCAPS) PURE;
        STDMETHOD(EnumObjects)(THIS_ LPDIENUMDEVICEOBJECTSCALLBACKA,LPVOID,DWORD) PURE;
        STDMETHOD(GetProperty)(THIS_ REFGUID,LPDIPROPHEADER) PURE;
        STDMETHOD(SetProperty)(THIS_ REFGUID,LPCDIPROPHEADER) PURE;
        STDMETHOD(Acquire)(THIS) PURE;
        STDMETHOD(Unacquire)(THIS) PURE;
        STDMETHOD(GetDeviceState)(THIS_ DWORD,LPVOID) PURE;
        STDMETHOD(GetDeviceData)(THIS_ DWORD,LPDIDEVICEOBJECTDATA,LPDWORD,DWORD) PURE;
        STDMETHOD(SetDataFormat)(THIS_ LPCDIDATAFORMAT) PURE;
        STDMETHOD(SetEventNotification)(THIS_ HANDLE) PURE;
        STDMETHOD(SetCooperativeLevel)(THIS_ HWND,DWORD) PURE;
        STDMETHOD(GetObjectInfo)(THIS_ LPDIDEVICEOBJECTINSTANCEA,DWORD,DWORD) PURE;
        STDMETHOD(GetDeviceInfo)(THIS_ LPDIDEVICEINSTANCEA) PURE;
        STDMETHOD(RunControlPanel)(THIS_ HWND,DWORD) PURE;
        STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD,REFGUID) PURE;
        STDMETHOD(CreateEffect)(THIS_ REFGUID,LPCDIEFFECT,LPDIRECTINPUTEFFECT *,LPUNKNOWN) PURE;
        STDMETHOD(EnumEffects)(THIS_ LPDIENUMEFFECTSCALLBACKA,LPVOID,DWORD) PURE;
        STDMETHOD(GetEffectInfo)(THIS_ LPDIEFFECTINFOA,REFGUID) PURE;
        STDMETHOD(GetForceFeedbackState)(THIS_ LPDWORD) PURE;
        STDMETHOD(SendForceFeedbackCommand)(THIS_ DWORD) PURE;
        STDMETHOD(EnumCreatedEffectObjects)(THIS_ LPDIENUMCREATEDEFFECTOBJECTSCALLBACK,LPVOID,DWORD) PURE;
        STDMETHOD(Escape)(THIS_ LPDIEFFESCAPE) PURE;
        STDMETHOD(Poll)(THIS) PURE;
        STDMETHOD(SendDeviceData)(THIS_ DWORD,LPCDIDEVICEOBJECTDATA,LPDWORD,DWORD) PURE;
        STDMETHOD(EnumEffectsInFile)(THIS_ LPCSTR,LPDIENUMEFFECTSINFILECALLBACK,LPVOID,DWORD) PURE;
        STDMETHOD(WriteEffectToFile)(THIS_ LPCSTR,DWORD,LPDIFILEEFFECT,DWORD) PURE;
        STDMETHOD(BuildActionMap)(THIS_ LPDIACTIONFORMATA,LPCSTR,DWORD) PURE;
        STDMETHOD(SetActionMap)(THIS_ LPDIACTIONFORMATA,LPCSTR,DWORD) PURE;
        STDMETHOD(GetImageInfo)(THIS_ LPDIDEVICEIMAGEINFOHEADERA) PURE;
*/
        STDMETHOD(GetCapabilities)(THIS_ LPDIDEVCAPS) PURE;
        STDMETHOD(EnumObjects)(THIS_ LPVOID,LPVOID,DWORD) PURE;
        STDMETHOD(GetProperty)(THIS_ REFGUID,LPVOID) PURE;
        STDMETHOD(SetProperty)(THIS_ REFGUID,LPVOID) PURE;
        STDMETHOD(Acquire)(THIS) PURE;
        STDMETHOD(Unacquire)(THIS) PURE;
        STDMETHOD(GetDeviceState)(THIS_ DWORD,LPVOID) PURE;
        STDMETHOD(GetDeviceData)(THIS_ DWORD,LPVOID,LPDWORD,DWORD) PURE;
        STDMETHOD(SetDataFormat)(THIS_ LPCDIDATAFORMAT) PURE;
        STDMETHOD(SetEventNotification)(THIS_ HANDLE) PURE;
        STDMETHOD(SetCooperativeLevel)(THIS_ HWND,DWORD) PURE;
        STDMETHOD(GetObjectInfo)(THIS_ LPVOID,DWORD,DWORD) PURE;
        STDMETHOD(GetDeviceInfo)(THIS_ LPVOID) PURE;
        STDMETHOD(RunControlPanel)(THIS_ HWND,DWORD) PURE;
        STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD,REFGUID) PURE;
        STDMETHOD(CreateEffect)(THIS_ REFGUID,LPVOID,LPVOID *,LPUNKNOWN) PURE;
        STDMETHOD(EnumEffects)(THIS_ LPVOID,LPVOID,DWORD) PURE;
        STDMETHOD(GetEffectInfo)(THIS_ LPVOID,REFGUID) PURE;
        STDMETHOD(GetForceFeedbackState)(THIS_ LPDWORD) PURE;
        STDMETHOD(SendForceFeedbackCommand)(THIS_ DWORD) PURE;
        STDMETHOD(EnumCreatedEffectObjects)(THIS_ LPVOID,LPVOID,DWORD) PURE;
        STDMETHOD(Escape)(THIS_ LPVOID) PURE;
        STDMETHOD(Poll)(THIS) PURE;
        STDMETHOD(SendDeviceData)(THIS_ DWORD,LPVOID,LPDWORD,DWORD) PURE;
        STDMETHOD(EnumEffectsInFile)(THIS_ LPCSTR,LPVOID,LPVOID,DWORD) PURE;
        STDMETHOD(WriteEffectToFile)(THIS_ LPCSTR,DWORD,LPVOID,DWORD) PURE;
        STDMETHOD(BuildActionMap)(THIS_ LPVOID,LPCSTR,DWORD) PURE;
        STDMETHOD(SetActionMap)(THIS_ LPVOID,LPCSTR,DWORD) PURE;
        STDMETHOD(GetImageInfo)(THIS_ LPVOID) PURE;
    };
    typedef struct IDirectInputDevice8A *LPDIRECTINPUTDEVICE8A;


    /*
        IDirectInput8A

        This defines the DirectInput8 interface needed to 
        find and create direct input device interfaces.  
    */
    #undef INTERFACE
    #define INTERFACE IDirectInput8A

    DECLARE_INTERFACE_(IDirectInput8A, IUnknown)
    {
        /*** IUnknown methods ***/
        STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE;
        STDMETHOD_(ULONG,AddRef)(THIS) PURE;
        STDMETHOD_(ULONG,Release)(THIS) PURE;

        /*** IDirectInput8A methods ***/
        STDMETHOD(CreateDevice)(THIS_ REFGUID,LPDIRECTINPUTDEVICE8A *,LPUNKNOWN) PURE;
        STDMETHOD(EnumDevices)(THIS_ DWORD,LPDIENUMDEVICESCALLBACKA,LPVOID,DWORD) PURE;
        STDMETHOD(GetDeviceStatus)(THIS_ REFGUID) PURE;
        STDMETHOD(RunControlPanel)(THIS_ HWND,DWORD) PURE;
        STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD) PURE;
        STDMETHOD(FindDevice)(THIS_ REFGUID,LPCSTR,LPGUID) PURE;
        STDMETHOD(EnumDevicesBySemantics)(THIS_ LPCSTR,LPVOID,LPVOID,LPVOID,DWORD) PURE;
        STDMETHOD(ConfigureDevices)(THIS_ LPVOID,LPVOID,DWORD,LPVOID) PURE;
    };

    /*
        dinput.h defines

        These are the defines, turned into const uint32, that
        are normally found in dinput.h . Only the ones needed
        in the IDirectInput8A and IDirectInputDevice8A interface
        methods are defined.
    */

    const uint32 DIEDFL_ATTACHEDONLY   = 0x00000001;
    const uint32 DI8DEVCLASS_GAMECTRL  = 4;
    const uint32 DISCL_EXCLUSIVE       = 0x00000001;
    const uint32 DISCL_NONEXCLUSIVE    = 0x00000002;
    const uint32 DISCL_FOREGROUND      = 0x00000004;
    const uint32 DISCL_BACKGROUND      = 0x00000008;
    const uint32 DISCL_NOWINKEY        = 0x00000010;

    const uint32 DIDFT_RELAXIS         = 0x00000001;
    const uint32 DIDFT_ABSAXIS         = 0x00000002;
    const uint32 DIDFT_AXIS            = 0x00000003;
    const uint32 DIDFT_BUTTON          = 0x0000000C;
    const uint32 DIDFT_POV             = 0x00000010;
    const uint32 DIDFT_ANYINSTANCE     = 0x00FFFF00;

    const uint32 DIDF_ABSAXIS          = 0x00000001;
    const uint32 DIDF_RELAXIS          = 0x00000002;
    const uint32 DIDFT_OPTIONAL        = 0x80000000;

    DEFINE_GUID(IID_IDirectInput8A,    0xBF798030,0x483A,0x4DA2,0xAA,0x99,0x5D,0x64,0xED,0x36,0x97,0x00);
    DEFINE_GUID(IID_IDirectInputDevice8A,0x54D41080,0xDC15,0x4833,0xA4,0x1B,0x74,0x8F,0x73,0xA3,0x81,0x79);
    DEFINE_GUID(GUID_Joystick   ,0x6F1D2B70,0xD5A0,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);

    DEFINE_GUID(GUID_XAxis,   0xA36D02E0,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
    DEFINE_GUID(GUID_YAxis,   0xA36D02E1,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
    DEFINE_GUID(GUID_ZAxis,   0xA36D02E2,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
    DEFINE_GUID(GUID_RxAxis,  0xA36D02F4,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
    DEFINE_GUID(GUID_RyAxis,  0xA36D02F5,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
    DEFINE_GUID(GUID_RzAxis,  0xA36D02E3,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
    DEFINE_GUID(GUID_Slider,  0xA36D02E4,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);

    DEFINE_GUID(GUID_Button,  0xA36D02F0,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
    DEFINE_GUID(GUID_Key,     0x55728220,0xD33C,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);

    DEFINE_GUID(GUID_POV,     0xA36D02F2,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);

    /*
        These declarations mimic the structures
        exported in dinput8.lib and are used
        in GetDeviceState calls for joysticks
    */

    struct G3DJOYDATA { 
        LONG lX;
        LONG lY;
        LONG lZ;
        LONG lRx;
        LONG lRy;
        LONG lRz;
        LONG rglSlider[2];
        DWORD rgdwPOV[4];
        BYTE rgbButtons[32];
    }; 

    DIOBJECTDATAFORMAT rgodf[ ] = { 
      { &GUID_XAxis, FIELD_OFFSET(G3DJOYDATA, lX),
        DIDFT_AXIS | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE, 0, }, 
      { &GUID_YAxis, FIELD_OFFSET(G3DJOYDATA, lY), 
        DIDFT_AXIS | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE, 0, }, 
      { &GUID_ZAxis, FIELD_OFFSET(G3DJOYDATA, lZ), 
        DIDFT_AXIS | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE, 0, }, 
      { &GUID_RxAxis, FIELD_OFFSET(G3DJOYDATA, lRx), 
        DIDFT_AXIS | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE, 0, }, 
      { &GUID_RyAxis, FIELD_OFFSET(G3DJOYDATA, lRy), 
        DIDFT_AXIS | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE, 0, }, 
      { &GUID_RzAxis, FIELD_OFFSET(G3DJOYDATA, lRz), 
        DIDFT_AXIS | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE, 0, }, 
      { &GUID_Slider, FIELD_OFFSET(G3DJOYDATA, rglSlider[0]), 
        DIDFT_AXIS | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE, 0, }, 
      { &GUID_Slider, FIELD_OFFSET(G3DJOYDATA, rglSlider[1]), 
        DIDFT_AXIS | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE, 0, }, 
      { &GUID_POV, FIELD_OFFSET(G3DJOYDATA, rgdwPOV[0]), 
        DIDFT_POV | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE, 0, }, 
      { &GUID_POV, FIELD_OFFSET(G3DJOYDATA, rgdwPOV[1]), 
        DIDFT_POV | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE, 0, }, 
      { &GUID_POV, FIELD_OFFSET(G3DJOYDATA, rgdwPOV[2]), 
        DIDFT_POV | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE, 0, }, 
      { &GUID_POV, FIELD_OFFSET(G3DJOYDATA, rgdwPOV[3]), 
        DIDFT_POV | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE, 0, }, 
      { &GUID_Button, FIELD_OFFSET(G3DJOYDATA, rgbButtons[0]),
        DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE, 0, }, 
      { &GUID_Button, FIELD_OFFSET(G3DJOYDATA, rgbButtons[1]),
        DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE, 0, }, 
      { &GUID_Button, FIELD_OFFSET(G3DJOYDATA, rgbButtons[2]),
        DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE, 0, }, 
      { &GUID_Button, FIELD_OFFSET(G3DJOYDATA, rgbButtons[3]),
        DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE, 0, }, 
      { &GUID_Button, FIELD_OFFSET(G3DJOYDATA, rgbButtons[4]),
        DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE, 0, }, 
      { &GUID_Button, FIELD_OFFSET(G3DJOYDATA, rgbButtons[5]),
        DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE, 0, }, 
      { &GUID_Button, FIELD_OFFSET(G3DJOYDATA, rgbButtons[6]),
        DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE, 0, }, 
      { &GUID_Button, FIELD_OFFSET(G3DJOYDATA, rgbButtons[7]),
        DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE, 0, }, 
      { &GUID_Button, FIELD_OFFSET(G3DJOYDATA, rgbButtons[8]),
        DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE, 0, }, 
      { &GUID_Button, FIELD_OFFSET(G3DJOYDATA, rgbButtons[9]),
        DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE, 0, }, 
      { &GUID_Button, FIELD_OFFSET(G3DJOYDATA, rgbButtons[10]),
        DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE, 0, }, 
      { &GUID_Button, FIELD_OFFSET(G3DJOYDATA, rgbButtons[11]),
        DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE, 0, }, 
      { &GUID_Button, FIELD_OFFSET(G3DJOYDATA, rgbButtons[12]),
        DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE, 0, }, 
      { &GUID_Button, FIELD_OFFSET(G3DJOYDATA, rgbButtons[13]),
        DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE, 0, }, 
      { &GUID_Button, FIELD_OFFSET(G3DJOYDATA, rgbButtons[14]),
        DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE, 0, }, 
      { &GUID_Button, FIELD_OFFSET(G3DJOYDATA, rgbButtons[15]),
        DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE, 0, }, 
      { &GUID_Button, FIELD_OFFSET(G3DJOYDATA, rgbButtons[16]),
        DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE, 0, }, 
      { &GUID_Button, FIELD_OFFSET(G3DJOYDATA, rgbButtons[17]),
        DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE, 0, }, 
      { &GUID_Button, FIELD_OFFSET(G3DJOYDATA, rgbButtons[18]),
        DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE, 0, }, 
      { &GUID_Button, FIELD_OFFSET(G3DJOYDATA, rgbButtons[19]),
        DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE, 0, }, 
      { &GUID_Button, FIELD_OFFSET(G3DJOYDATA, rgbButtons[20]),
        DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE, 0, }, 
      { &GUID_Button, FIELD_OFFSET(G3DJOYDATA, rgbButtons[21]),
        DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE, 0, }, 
      { &GUID_Button, FIELD_OFFSET(G3DJOYDATA, rgbButtons[22]),
        DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE, 0, }, 
      { &GUID_Button, FIELD_OFFSET(G3DJOYDATA, rgbButtons[23]),
        DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE, 0, }, 
      { &GUID_Button, FIELD_OFFSET(G3DJOYDATA, rgbButtons[24]),
        DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE, 0, }, 
      { &GUID_Button, FIELD_OFFSET(G3DJOYDATA, rgbButtons[25]),
        DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE, 0, }, 
      { &GUID_Button, FIELD_OFFSET(G3DJOYDATA, rgbButtons[26]),
        DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE, 0, }, 
      { &GUID_Button, FIELD_OFFSET(G3DJOYDATA, rgbButtons[27]),
        DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE, 0, }, 
      { &GUID_Button, FIELD_OFFSET(G3DJOYDATA, rgbButtons[28]),
        DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE, 0, }, 
      { &GUID_Button, FIELD_OFFSET(G3DJOYDATA, rgbButtons[29]),
        DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE, 0, }, 
      { &GUID_Button, FIELD_OFFSET(G3DJOYDATA, rgbButtons[30]),
        DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE, 0, }, 
      { &GUID_Button, FIELD_OFFSET(G3DJOYDATA, rgbButtons[31]),
        DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE, 0, }, 
    }; 
    const uint32 numJoyObjects = (sizeof(rgodf) / sizeof(rgodf[0])); 

    const DIDATAFORMAT G3DJOYDF = { 
        sizeof(DIDATAFORMAT),       // Size of this structure 
        sizeof(DIOBJECTDATAFORMAT), // Size of object data format 
        DIDF_ABSAXIS,               // Absolute axis coordinates 
        sizeof(G3DJOYDATA),         // Size of device data 
        numJoyObjects,              // Number of objects 
        rgodf,                      // And here they are 
    }; 

} //namespace Win32Window_DI8



#endif //WIN32WINDOW_INCLUDE_DI8