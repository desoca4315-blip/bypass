#include "Offset/SDK.hpp"
using namespace SDK;
using namespace std;
#include <android/native_window_jni.h>
#include <android/log.h>
#include "Main/Tools.h"
#include "Main/Dobby/Dobby.h"
#include "Main/Includes.h"
#include "Main/StrEnc.h"
#include "Main/Vector3.hpp"
#include "Main/Vector2.hpp"
#include "Main/MemoryTools.h"
#include "Main/KittyMemory/MemoryPatch.h"
#include "Main/android_native_app_glue.h"
#include "Main/obfuscate.h"
#include "PUBGM.h"
#include <curl/curl.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <thread>
#include <chrono>
#include "Main/json.hpp"
#include "Main/oxorany.cpp"
#include "GlossHook/Gloss.h"
bool MEMORY;
bool bValid = false;
bool isLogin = false;
bool Expiry = true;
bool IpadView = false;
bool Canvas2 = false;
bool SmallCrossHair;
//bool IpadView = true;
static std::string EXP = "";
std::string g_Token, g_Auth;
using json = nlohmann::json;
android_app *g_App = 0;
ASTExtraPlayerCharacter *g_LocalPlayer=0;
ASTExtraPlayerController *g_PlayerController =0;

#define SLEEP_TIME 1000LL / 120LL
#define TSL_FONT_DEFAULT_SIZE 12

#define PI 3.14159265358979323846
#define RAD2DEG(x) ((float)(x) * (float)(180.f / PI))

bool ONLYESEPI;
bool AIMBOTX;
bool FIRE;
bool BULLETTRACK;
bool MAGICBULLET;
float aimAssistDistance = 70;
bool DEVICE = true;
bool memories;
static int TeamID;


uintptr_t GNames_Offset = 0x89750B0;
uintptr_t GUObject_Offset = 0xEB04D00;
uintptr_t GetActorArray = 0xA71EA80;
uintptr_t GNativeAndroidApp_Offset = 0xE8338A8;
uintptr_t Actors_Offset = 0xA0;

int screenWidth = -1, glWidth, screenHeight = -1, glHeight;
float density = -1;
uintptr_t UE4;

float screenSizeX = 0;
float screenSizeY = 0;
float FOVsize = 70;
float Speed_Aim = 15.4;
float recoilCompensationFactor = 0.8;
float Range = 70;
bool head;
bool AimHead = false;
bool AimBody = true;
int trackingType = 1;
int scopeAndFire = 0;
float FOVSizea;

enum EAimMode {
    AimBullet = 0,
    Pbullet = 1,
    AimBot = 2
};
enum EAimTarget {
    Chest = 0,
    Head = 1
};
enum EAimBy {
    FOV = 0,
    Distance = 1
};
enum EAimTrigger {
    Shooting = 0,
    None = 1,
    Scoping = 2,
    Both = 3,
    Any = 4
};
std::map<int, bool> itemConfig;

struct sConfig {
    bool Bypass;
    bool Enable;
    struct sPlayerESP {
        bool Line;
        bool Health;
        bool ItemEsp;
        bool Skeleton;
        bool Name;
        bool Distance;
        bool TeamID;
        bool Vehicle;
        bool Grenade;
        bool OneClickEsp;
        bool MessageBox;
    };
    sPlayerESP PlayerESP{0};

    struct sVehicleESP {
        bool ShowVehicle;
        bool ShowDistance;
    };
    sVehicleESP VehicleESP{0};

    struct sAimMenu {
        bool Enable;
        bool AimBot;
        EAimMode Mode;
        EAimBy AimBy;
        EAimTarget Target;
        EAimTrigger Trigger;
        bool RecoilComparison;
        float Recc;
        bool RecoilSet;
        bool Radius;
        float Line;
        bool Prediction;
        float Cross;
        float Crosss;
        bool IgnoreKnocked;
        bool VisCheck;
        bool IgnoreBot;
        float AimSmooth = 1.0f;
        float AimSet = 11.0f;
    };
    sAimMenu SilentAim{0};
    sAimMenu AimBot{0};
};
sConfig Config{0};
int GetIndex(int currentIndex, int arrayNum) {
    if (arrayNum <= 0) {
        return 0;
    }
    return (currentIndex + 1) % arrayNum;
}

int (*MessageBoxExt)(unsigned int, const char16_t *, const char16_t *);
 
std::u16string convertToUtf16(const std::string &utf8str)
{
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    return convert.from_bytes(utf8str);
}

std::string Fuck = "💫||√ WELCOME -: ALONE PVT MOD √||💫";
std::u16string f2 = convertToUtf16(Fuck);

std::string Fuck1 = "#1 Dynamic Always On Top";
std::u16string f3 = convertToUtf16(Fuck1);

std::string Op4 = "#1 WINNER DONE!\nFeedback: To @ALONE_OWNER_OP ❤";
std::u16string xxx4 = convertToUtf16(Op4);


std::string Filepath = "/sdcard/Android/obb/com.pubg.imobile/key.lic";
const char *Gamepackage = "com.pubg.imobile";

static bool isHead, isNeck, isPelvis, isLeftClavicle, isRightClavicle, 
            isLeftUpperArm, isLeftLowerArm, isLeftHand, isLeftThigh, 
            isLeftCalf, isLeftFoot, isRightUpperArm, isRightLowerArm, 
            isRightHand, isRightThigh, isRightCalf, isRightFoot, 
            isSpine1, isSpine2, isSpine3;
static int algorithm = 0;

struct sRegion
{
    uintptr_t start, end;
};

std::vector<sRegion> trapRegions;

bool isEqual(std::string s1, const char* check) {
    std::string s2(check);
    return (s1 == s2);
}

bool isObjectInvalid(UObject *obj)
{
    if (!Tools::IsPtrValid(obj))
    {
        return true;
    }
    if (!Tools::IsPtrValid(obj->ClassPrivate))
    {
        return true;
    }
    if (obj->InternalIndex <= 0)
    {
        return true;
    }
    if (obj->NamePrivate.ComparisonIndex <= 0)
    {
        return true;
    }
    if ((uintptr_t)(obj) % sizeof(uintptr_t) != 0x0 && (uintptr_t)(obj) % sizeof(uintptr_t) != 0x4)
    {
        return true;
    }
    if (std::any_of(trapRegions.begin(), trapRegions.end(), [obj](sRegion region) { return ((uintptr_t)obj) >= region.start && ((uintptr_t)obj) <= region.end; }) ||
        std::any_of(trapRegions.begin(), trapRegions.end(), [obj](sRegion region) { return ((uintptr_t)obj->ClassPrivate) >= region.start && ((uintptr_t)obj->ClassPrivate) <= region.end; }))
    {
        return true;
    }
    return false;
}

bool UrlLink;
int OpenURL(const char* url)
{
    JavaVM* java_vm = g_App->activity->vm;
    JNIEnv* java_env = NULL;

    jint jni_return = java_vm->GetEnv((void**)&java_env, JNI_VERSION_1_6);
    if (jni_return == JNI_ERR)
        return -1;

    jni_return = java_vm->AttachCurrentThread(&java_env, NULL);
    if (jni_return != JNI_OK)
        return -2;

    jclass native_activity_clazz = java_env->GetObjectClass(g_App->activity->clazz);
    if (native_activity_clazz == NULL)
        return -3;

    jmethodID method_id = java_env->GetMethodID(native_activity_clazz, "AndroidThunkJava_LaunchURL", "(Ljava/lang/String;)V");
    if (method_id == NULL)
        return -4;
        
    jstring retStr = java_env->NewStringUTF(url);
    java_env->CallVoidMethod(g_App->activity->clazz, method_id, retStr);

    jni_return = java_vm->DetachCurrentThread();
    if (jni_return != JNI_OK)
        return -5;

    return 0;
}



void DrawTrueTransparentRect(UCanvas* Canvas, FVector2D Position, float Width, float Height, FLinearColor Color)
{
 // K2_DrawTexture 
 Canvas->K2_DrawTexture(
 nullptr, // RenderTexture
 Position, // ScreenPosition
 FVector2D(Width, Height), // ScreenSize
 FVector2D(0, 0), // CoordinatePosition
 FVector2D(1, 1), // CoordinateSize
 Color, // RenderColor
 EBlendMode::BLEND_Translucent, // BlendMode
 0.0f, // Rotation
 FVector2D(0, 0) // RotPivot
 );
}

template<typename T>
void Write(uintptr_t addr, T value) {
WriteAddr((void *) addr, &value, sizeof(T));
}
void MemoryD_type(uintptr_t addr,int var){
WriteAddr(reinterpret_cast<void*>(addr),reinterpret_cast<void*>(&var),4);
}
void MemoryQ_type(uintptr_t addr,int var){
WriteAddr(reinterpret_cast<void*>(addr),reinterpret_cast<void*>(&var),32);
}

#include <chrono>

class FPSCounter {
private:
    unsigned int m_fps;
    unsigned int m_frameCount;
    std::chrono::steady_clock::time_point m_lastTime;

public:
    FPSCounter() {
        m_fps = 0;
        m_frameCount = 0;
        m_lastTime = std::chrono::steady_clock::now();
    }

    void update() {
        m_frameCount++;

        auto currentTime = std::chrono::steady_clock::now();
        std::chrono::duration<float> elapsed = currentTime - m_lastTime;

        if (elapsed.count() >= 1.0f) {
            m_fps = m_frameCount;
            m_frameCount = 0;
            m_lastTime = currentTime;
        }
    }

    unsigned int get() const {
        return m_fps;
    }
};


FPSCounter fps;
UWorld *GEWorld;
int GWorldNum = 0;
TUObjectArray gobjects;
UWorld *GetFullWorld()
{
    if(GWorldNum == 0) {
        gobjects = UObject::GUObjectArray->ObjObjects;
        for (int i=0; i< gobjects.Num(); i++)
            if (auto obj = gobjects.GetByIndex(i)) {
                if(obj->IsA(UEngine::StaticClass())) {
                    auto GEngine = (UEngine *) obj;
                    if(GEngine) {
                        auto ViewPort = GEngine->GameViewport;
                        if (ViewPort)
                        {
                            GEWorld = ViewPort->World;
                            GWorldNum = i;
                            if (!GEWorld || !GEWorld->NetDriver) {
                              g_LocalPlayer = nullptr;
                              g_PlayerController = nullptr;
                              return nullptr;
                              }
                            return ViewPort->World;
                        }
                    }
                }
            }
    }else {
        auto GEngine = (UEngine *) (gobjects.GetByIndex(GWorldNum));
        if(GEngine) {
            auto ViewPort = GEngine->GameViewport;
            if(ViewPort) {
                GEWorld = ViewPort->World;
                                            if (!GEWorld || !GEWorld->NetDriver) {
                              g_LocalPlayer = nullptr;
                              g_PlayerController = nullptr;
                              return nullptr;
                              }
                return ViewPort->World;
            }
        }
    }
    return 0;
}

static UEngine *GEngine = 0;
UWorld *GetWorld() {
    while (!GEngine) {
        GEngine = UObject::FindObject<UEngine>("UAEGameEngine Transient.UAEGameEngine_1"); // Auto 
        sleep(1);
    }
    if (GEngine) {
        auto ViewPort = GEngine->GameViewport;

        if (ViewPort) {
            return ViewPort->World;
        }
    }
    return 0;
}

static UGameViewportClient *GameViewport = 0;
UGameViewportClient *GetGameViewport() {
    while (!GameViewport) {
        GameViewport = UObject::FindObject<UGameViewportClient>("GameViewportClient Transient.UAEGameEngine_1.GameViewportClient_1");
        sleep(1);
    }
    if (GameViewport) {
        return GameViewport;
    }
    return 0;
}

std::vector<AActor *> GetActors() {
    auto World = GetFullWorld();
    if (!World)
    return std::vector<AActor *>();
    auto PersistentLevel = World->PersistentLevel;
    if (!PersistentLevel)
    return std::vector<AActor *>();
    struct GovnoArray {
    uintptr_t base;
    int32_t count;
    int32_t max;
    };
    static thread_local GovnoArray Actors{};
    Actors = *(((GovnoArray*(*)(uintptr_t))(UE4 + GetActorArray))(reinterpret_cast<uintptr_t>(PersistentLevel)));
    if (Actors.count <= 0) {
    return {};
    }
    std::vector<AActor *> actors;
    for (int i = 0; i < Actors.count; i++) {
    auto Actor = *(uintptr_t *) (Actors.base + (i * sizeof(uintptr_t)));
    if (Actor) {
    actors.push_back(reinterpret_cast<AActor *const>(Actor));
    }
    }
  return actors;
}

const char *GetVehicleName(ASTExtraVehicleBase *Vehicle) {
    switch (Vehicle->VehicleShapeType) {
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_Motorbike:
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_Motorbike_SideCart:
            return "Motorbike";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_Dacia:
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_HeavyDacia:
            return "Dacia";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_MiniBus:
            return "Mini Bus";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_PickUp:
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_PickUp01:
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_HeavyPickup:
            return "Pick Up";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_Buggy:
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_HeavyBuggy:
            return "Buggy";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_UAZ:
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_UAZ01:
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_UAZ02:
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_UAZ03:
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_HeavyUAZ:
            return "UAZ";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_PG117:
            return "PG117";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_Aquarail:
            return "Aquarail";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_Mirado:
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_Mirado01:
            return "Mirado";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_Rony:
            return "Rony";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_Scooter:
            return "Scooter";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_SnowMobile:
            return "Snow Mobile";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_TukTukTuk:
            return "Tuk Tuk";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_SnowBike:
            return "Snow Bike";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_Surfboard:
            return "Surf Board";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_Snowboard:
            return "Snow Board";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_Amphibious:
            return "Amphibious";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_LadaNiva:
            return "Lada Niva";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_UAV:
            return "UAV";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_MegaDrop:
            return "Mega Drop";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_Lamborghini:
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_Lamborghini01:
            return "Lamborghini";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_GoldMirado:
            return "Gold Mirado";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_BigFoot:
            return "Big Foot";
            break;
        case ESTExtraVehicleShapeType::ESTExtraVehicleShapeType__VST_HeavyUH60:
            return "UH60";
            break;
        default:
            return "Vehicle";
            break;
    }
    return "Vehicle";
}

std::string getClipboardText() {
        if (!g_App)
            return "";

        auto activity = g_App->activity;
        if (!activity)
            return "";

        auto vm = activity->vm;
        if (!vm)
            return "";

        auto object = activity->clazz;
        if (!object)
            return "";

        std::string result;

        JNIEnv *env;
        vm->AttachCurrentThread(&env, 0);
        {
            auto ContextClass = env->FindClass("android/content/Context");
            auto getSystemServiceMethod = env->GetMethodID(ContextClass, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");

            auto str = env->NewStringUTF("clipboard");
            auto clipboardManager = env->CallObjectMethod(object, getSystemServiceMethod, str);
            env->DeleteLocalRef(str);

            auto ClipboardManagerClass = env->FindClass("android/content/ClipboardManager");
            auto getText = env->GetMethodID(ClipboardManagerClass, "getText", "()Ljava/lang/CharSequence;");

            auto CharSequenceClass = env->FindClass("java/lang/CharSequence");
            auto toStringMethod = env->GetMethodID(CharSequenceClass, "toString", "()Ljava/lang/String;");

            auto text = env->CallObjectMethod(clipboardManager, getText);
            if (text) {
                str = (jstring) env->CallObjectMethod(text, toStringMethod);
                result = env->GetStringUTFChars(str, 0);
                env->DeleteLocalRef(str);
                env->DeleteLocalRef(text);
            }

            env->DeleteLocalRef(CharSequenceClass);
            env->DeleteLocalRef(ClipboardManagerClass);
            env->DeleteLocalRef(clipboardManager);
            env->DeleteLocalRef(ContextClass);
        }
        vm->DetachCurrentThread();

        return result;
    }

const char *GetAndroidID(JNIEnv *env, jobject context) {
    jclass contextClass = env->FindClass(/*android/content/Context*/ StrEnc("`L+&0^[S+-:J^$,r9q92(as", "\x01\x22\x4F\x54\x5F\x37\x3F\x7C\x48\x42\x54\x3E\x3B\x4A\x58\x5D\x7A\x1E\x57\x46\x4D\x19\x07", 23).c_str());
    jmethodID getContentResolverMethod = env->GetMethodID(contextClass, /*getContentResolver*/ StrEnc("E8X\\7r7ys_Q%JS+L+~", "\x22\x5D\x2C\x1F\x58\x1C\x43\x1C\x1D\x2B\x03\x40\x39\x3C\x47\x3A\x4E\x0C", 18).c_str(), /*()Landroid/content/ContentResolver;*/ StrEnc("8^QKmj< }5D:9q7f.BXkef]A*GYLNg}B!/L", "\x10\x77\x1D\x2A\x03\x0E\x4E\x4F\x14\x51\x6B\x59\x56\x1F\x43\x03\x40\x36\x77\x28\x0A\x08\x29\x24\x44\x33\x0B\x29\x3D\x08\x11\x34\x44\x5D\x77", 35).c_str());
    jclass settingSecureClass = env->FindClass(/*android/provider/Settings$Secure*/ StrEnc("T1yw^BCF^af&dB_@Raf}\\FS,zT~L(3Z\"", "\x35\x5F\x1D\x05\x31\x2B\x27\x69\x2E\x13\x09\x50\x0D\x26\x3A\x32\x7D\x32\x03\x09\x28\x2F\x3D\x4B\x09\x70\x2D\x29\x4B\x46\x28\x47", 32).c_str());
    jmethodID getStringMethod = env->GetStaticMethodID(settingSecureClass, /*getString*/ StrEnc("e<F*J5c0Y", "\x02\x59\x32\x79\x3E\x47\x0A\x5E\x3E", 9).c_str(), /*(Landroid/content/ContentResolver;Ljava/lang/String;)Ljava/lang/String;*/ StrEnc("$6*%R*!XO\"m18o,0S!*`uI$IW)l_/_knSdlRiO1T`2sH|Ouy__^}%Y)JsQ:-\"(2_^-$i{?H", "\x0C\x7A\x4B\x4B\x36\x58\x4E\x31\x2B\x0D\x0E\x5E\x56\x1B\x49\x5E\x27\x0E\x69\x0F\x1B\x3D\x41\x27\x23\x7B\x09\x2C\x40\x33\x1D\x0B\x21\x5F\x20\x38\x08\x39\x50\x7B\x0C\x53\x1D\x2F\x53\x1C\x01\x0B\x36\x31\x39\x46\x0C\x15\x43\x2B\x05\x30\x15\x41\x43\x46\x55\x70\x0D\x59\x56\x00\x15\x58\x73", 71).c_str());

    auto obj = env->CallObjectMethod(context, getContentResolverMethod);
    auto str = (jstring) env->CallStaticObjectMethod(settingSecureClass, getStringMethod, obj, env->NewStringUTF(/*android_id*/ StrEnc("ujHO)8OfOE", "\x14\x04\x2C\x3D\x46\x51\x2B\x39\x26\x21", 10).c_str()));
    return env->GetStringUTFChars(str, 0);
}

const char *GetDeviceModel(JNIEnv *env) {
    jclass buildClass = env->FindClass(/*android/os/Build*/ StrEnc("m5I{GKGWBP-VOxkA", "\x0C\x5B\x2D\x09\x28\x22\x23\x78\x2D\x23\x02\x14\x3A\x11\x07\x25", 16).c_str());
    jfieldID modelId = env->GetStaticFieldID(buildClass, /*MODEL*/ StrEnc("|}[q:", "\x31\x32\x1F\x34\x76", 5).c_str(), /*Ljava/lang/String;*/ StrEnc(".D:C:ETZ1O-Ib&^h.Y", "\x62\x2E\x5B\x35\x5B\x6A\x38\x3B\x5F\x28\x02\x1A\x16\x54\x37\x06\x49\x62", 18).c_str());

    auto str = (jstring) env->GetStaticObjectField(buildClass, modelId);
    return env->GetStringUTFChars(str, 0);
}

const char *GetDeviceBrand(JNIEnv *env) {
    jclass buildClass = env->FindClass(/*android/os/Build*/ StrEnc("0iW=2^>0zTRB!B90", "\x51\x07\x33\x4F\x5D\x37\x5A\x1F\x15\x27\x7D\x00\x54\x2B\x55\x54", 16).c_str());
    jfieldID modelId = env->GetStaticFieldID(buildClass, /*BRAND*/ StrEnc("@{[FP", "\x02\x29\x1A\x08\x14", 5).c_str(), /*Ljava/lang/String;*/ StrEnc(".D:C:ETZ1O-Ib&^h.Y", "\x62\x2E\x5B\x35\x5B\x6A\x38\x3B\x5F\x28\x02\x1A\x16\x54\x37\x06\x49\x62", 18).c_str());

    auto str = (jstring) env->GetStaticObjectField(buildClass, modelId);
    return env->GetStringUTFChars(str, 0);
}

const char *GetPackageName(JNIEnv *env, jobject context) {
    jclass contextClass = env->FindClass(/*android/content/Context*/ StrEnc("`L+&0^[S+-:J^$,r9q92(as", "\x01\x22\x4F\x54\x5F\x37\x3F\x7C\x48\x42\x54\x3E\x3B\x4A\x58\x5D\x7A\x1E\x57\x46\x4D\x19\x07", 23).c_str());
    jmethodID getPackageNameId = env->GetMethodID(contextClass, /*getPackageName*/ StrEnc("YN4DaP)!{wRGN}", "\x3E\x2B\x40\x14\x00\x33\x42\x40\x1C\x12\x1C\x26\x23\x18", 14).c_str(), /*()Ljava/lang/String;*/ StrEnc("VnpibEspM(b]<s#[9cQD", "\x7E\x47\x3C\x03\x03\x33\x12\x5F\x21\x49\x0C\x3A\x13\x20\x57\x29\x50\x0D\x36\x7F", 20).c_str());

    auto str = (jstring) env->CallObjectMethod(context, getPackageNameId);
    return env->GetStringUTFChars(str, 0);
}

const char *GetDeviceUniqueIdentifier(JNIEnv *env, const char *uuid) {
    jclass uuidClass = env->FindClass(/*java/util/UUID*/ StrEnc("B/TxJ=3BZ_]SFx", "\x28\x4E\x22\x19\x65\x48\x47\x2B\x36\x70\x08\x06\x0F\x3C", 14).c_str());

    auto len = strlen(uuid);

    jbyteArray myJByteArray = env->NewByteArray(len);
    env->SetByteArrayRegion(myJByteArray, 0, len, (jbyte *) uuid);

    jmethodID nameUUIDFromBytesMethod = env->GetStaticMethodID(uuidClass, /*nameUUIDFromBytes*/ StrEnc("P6LV|'0#A+zQmoat,", "\x3E\x57\x21\x33\x29\x72\x79\x67\x07\x59\x15\x3C\x2F\x16\x15\x11\x5F", 17).c_str(), /*([B)Ljava/util/UUID;*/ StrEnc("sW[\"Q[W3,7@H.vT0) xB", "\x5B\x0C\x19\x0B\x1D\x31\x36\x45\x4D\x18\x35\x3C\x47\x1A\x7B\x65\x7C\x69\x3C\x79", 20).c_str());
    jmethodID toStringMethod = env->GetMethodID(uuidClass, /*toString*/ StrEnc("2~5292eW", "\x46\x11\x66\x46\x4B\x5B\x0B\x30", 8).c_str(), /*()Ljava/lang/String;*/ StrEnc("P$BMc' #j?<:myTh_*h0", "\x78\x0D\x0E\x27\x02\x51\x41\x0C\x06\x5E\x52\x5D\x42\x2A\x20\x1A\x36\x44\x0F\x0B", 20).c_str());

    auto obj = env->CallStaticObjectMethod(uuidClass, nameUUIDFromBytesMethod, myJByteArray);
    auto str = (jstring) env->CallObjectMethod(obj, toStringMethod);
    return env->GetStringUTFChars(str, 0);
}

struct MemoryStruct {
    char *memory;
    size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *) userp;

    mem->memory = (char *) realloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory == NULL) {
        return 0;
    }

    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

    std::string Login(const char *userKey) {
    if (!g_App)
        return "Internal Error";

    auto activity = g_App->activity;
    if (!activity)
        return "Internal Error";

    auto vm = activity->vm;
    if (!vm)
        return "Internal Error";

    auto object = activity->clazz;
    if (!object)
        return "Internal Error";

    JNIEnv *env;
    vm->AttachCurrentThread(&env, 0);
    std::string hwid = userKey;
    hwid += GetAndroidID(env, object);
    hwid += GetDeviceModel(env);
    hwid += GetDeviceBrand(env);
    std::string UUID = GetDeviceUniqueIdentifier(env, hwid.c_str());
    vm->DetachCurrentThread();
    std::string errMsg;

    struct MemoryStruct chunk{};
    chunk.memory = (char *) malloc(1);
    chunk.size = 0;

    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    //======LINKURL=====//
       if (curl) {
        std::string BHATIA = OBFUSCATE ("https://alonecheat.vippanel.cfd/connect");
        curl_easy_setopt(curl, CURLOPT_URL ,BHATIA.c_str());

        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, StrEnc("!mLBO", "\x49\x19\x38\x32\x3C", 5).c_str());
  struct curl_slist *headers = NULL;
  headers = curl_slist_append(headers, StrEnc("@;Ls\\(KP4Qrop`b#d3094/r1cf<c<=H)AiiBG6i|Ta66s2[", "\x03\x54\x22\x07\x39\x46\x3F\x7D\x60\x28\x02\x0A\x4A\x40\x03\x53\x14\x5F\x59\x5A\x55\x5B\x1B\x5E\x0D\x49\x44\x4E\x4B\x4A\x3F\x04\x27\x06\x1B\x2F\x6A\x43\x1B\x10\x31\x0F\x55\x59\x17\x57\x3F", 47).c_str());
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  char data[4096];
        sprintf(data, StrEnc("qu2yXK,YkJyGD@ut0.u~Nb'5(:.:chK", "\x16\x14\x5F\x1C\x65\x1B\x79\x1B\x2C\x6C\x0C\x34\x21\x32\x2A\x1F\x55\x57\x48\x5B\x3D\x44\x54\x50\x5A\x53\x4F\x56\x5E\x4D\x38", 31).c_str(), userKey, UUID.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &chunk);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            try {
                json result = json::parse(chunk.memory);
                if (result[StrEnc("(>_LBm", "\x5B\x4A\x3E\x38\x37\x1E", 6).c_str()] == true) {
                    std::string token = result[StrEnc("fAVA", "\x02\x20\x22\x20", 4).c_str()][/*token*/ StrEnc("{>3Lr", "\x0F\x51\x58\x29\x1C", 5).c_str()].get<std::string>();
                    time_t rng = result[StrEnc("fAVA", "\x02\x20\x22\x20", 4).c_str()][/*rng*/ StrEnc("+n,", "\x59\x00\x4B", 3).c_str()].get<time_t>();
                    EXP = result["data"]["EXP"].get<std::string>();
                    if (rng + 30 > time(0)) {
                        g_Token = "1";
                        g_Auth = "1";
                        bValid = g_Token == g_Auth;
                        if (bValid) {
                            pthread_t ptid;
                        }
                    }
                } else {
                    errMsg = result[StrEnc("LW(3(c", "\x3E\x32\x49\x40\x47\x0D", 6).c_str()].get<std::string>();
                }
            } catch (json::exception &e) {
                errMsg = "{";
                errMsg += e.what();
                errMsg += "}\n{";
                errMsg += chunk.memory;
                errMsg += "}";
            }
        } else {
            errMsg = curl_easy_strerror(res);

        }
    }
    curl_easy_cleanup(curl);
    vm->DetachCurrentThread();

    return bValid ? "OK" : errMsg;
}   

TNameEntryArray *GetGNames()
{
    return ((TNameEntryArray * (*)()) (UE4 + GNames_Offset))();
}

template <class T>
void GetAllActors(std::vector<T *> &Actors)
{
    UGameplayStatics *gGameplayStatics = (UGameplayStatics *)gGameplayStatics->StaticClass();
    auto GWorld = GetFullWorld();
    if (GWorld)
    {
        TArray<AActor *> Actors2;
        gGameplayStatics->GetAllActorsOfClass((UObject *)GWorld, T::StaticClass(), &Actors2);
        for (int i = 0; i < Actors2.Num(); i++)
        {
            Actors.push_back((T *)Actors2[i]);
        }
    }
}
////==========================================================================================================//
FVector GetBoneLocationByName(ASTExtraPlayerCharacter *Actor, const char *BoneName)
{
    return Actor->GetBonePos(BoneName, FVector());
}

#define COLOR_TRANSPARENT FLinearColor(0, 0, 0, 1.f)
#define COLOR_IN FLinearColor(0, 0, 0, 150)
#define COLOR_BLACK FLinearColor(0, 0, 0, 1.f)
#define COLOR_WHITE FLinearColor(1.f, 1.f, 1.f, 1.f)
#define COLOR_RED FLinearColor(1.f, 0, 0, 1.f)
#define COLOR_CAR FLinearColor(1.0f, 0.20f, 0.62f, 1.f)
#define COLOR_GREEN FLinearColor(0.0f, 1.0f, .0f, 1.0f)
#define COLOR_ORANGE FLinearColor(1.f, 0.4f, 0, 1.f)
#define COLOR_ROSE FLinearColor(1.0f, 0.28f, 0.66f, 1.0f)
#define COLOR_YELLOW FLinearColor(1.f, 1.f, 0, 1.f)
#define COLOR_LIME FLinearColor(0, 1.f, 0, 1.f)
#define COLOR_BLUE FLinearColor(0.12f, 0.48f, 1.0f, 1.f)
#define COLOR_THISTLE FLinearColor(1.0f, 0.74f, 0.84f, 1.0f)
#define COLOR_PINK FLinearColor(1.0f, 0.25f, 0.65f, 1.0f)
#define COLOR_CYAN FLinearColor(0.20f, 0.72f, 1.0f, 1.0f)
#define COLOR_OUTLINE FLinearColor(0.0f, 0.0f, 0.0f, 0.25f)
#define Black FLinearColor(0, 0, 0, 1.f)
#define White FLinearColor(1.f, 1.f, 1.f, 1.f)
#define COLOR_AZUSA FLinearColor(0.0f, 0.0f, 0.0f, 0.0f)
#define Red   FLinearColor(1.f, 0, 0, 1.f)
#define Gray  FLinearColor(0, 1.f, 0, 1.f)
#define Blue  FLinearColor(0.12f, 0.48f, 1.0f, 1.f)
#define Pink   FLinearColor(1.0f, 0.20f, 0.62f, 1.f)
#define Red FLinearColor(1.f, 0, 0, 0.7f)
#define Green FLinearColor(0, 0.5f, 0, 1.f)
#define Orange FLinearColor(1.f, 0.4f, 0, 1.f)
#define Purple FLinearColor(0.67f, 0.57f, 0.81f, 1.0f)
#define Yellow FLinearColor(1.f, 1.f, 0, 1.f)

SDK::FVector SubtractVectors(SDK::FVector a, SDK::FVector b) {
    SDK::FVector result;
    result.X = a.X - b.X;
    result.Y = a.Y - b.Y;
    result.Z = a.Z - b.Z;
    return result;
}

SDK::FVector AddVectors(SDK::FVector a, SDK::FVector b) {
    SDK::FVector result;
    result.X = a.X + b.X;
    result.Y = a.Y + b.Y;
    result.Z = a.Z + b.Z;
    return result;
}

SDK::FVector MultiplyVectors(SDK::FVector a, SDK::FVector b) {
    SDK::FVector result;
    result.X = a.X * b.X;
    result.Y = a.Y * b.Y;
    result.Z = a.Z * b.Z;
    return result;
}

SDK::FVector DivideVectors(SDK::FVector a, SDK::FVector b) {
    SDK::FVector result;
    result.X = a.X / b.X;
    result.Y = a.Y / b.Y;
    result.Z = a.Z / b.Z;
    return result;
}

SDK::FVector MultiplyVectorFloat(SDK::FVector a, float scalar) {
    SDK::FVector result;
    result.X = a.X * scalar;
    result.Y = a.Y * scalar;
    result.Z = a.Z * scalar;
    return result;
}

FRotator ToRotator2(FVector local, FVector target) {
    FVector Lund = SubtractVectors(target, local);
    FRotator newViewAngle;
    newViewAngle.Pitch =
            -std::atan2(Lund.Z, std::sqrt(Lund.X * Lund.X + Lund.Y * Lund.Y)) * (180.f / M_PI);
    newViewAngle.Yaw = std::atan2(Lund.Y, Lund.X) * (180.f / M_PI);
    newViewAngle.Roll = 0.f;
    if (newViewAngle.Yaw < 0.f) {
        newViewAngle.Yaw += 360.f;
    }
    return newViewAngle;
}

void VectorAnglesRadar(Vector3 &forward, FVector &angles) {
    if (forward.X == 0.f && forward.Y == 0.f) {
        angles.X = forward.Z > 0.f ? -360.f : 360.f;
        angles.Y = 0.f;
    } else {
        angles.X = RAD2DEG(atan2(-forward.Z, forward.Magnitude(forward)));
        angles.Y = RAD2DEG(atan2(forward.Y, forward.X));
    }
    angles.Z = 360.f;
}



FRotator ToRotator(FVector local, FVector target) {
FVector rotation = UKismetMathLibrary::Subtract_VectorVector(local, target);

float hyp = sqrt(rotation.X * rotation.X + rotation.Y * rotation.Y);

FRotator newViewAngle = {0};
newViewAngle.Pitch = -atan(rotation.Z / hyp) * (180.f / (float) 3.14159265358979323846);
newViewAngle.Yaw = atan(rotation.Y / rotation.X) * (180.f / (float) 3.14159265358979323846);
newViewAngle.Roll = (float) 0.f;

if (rotation.X >= 0.f)
newViewAngle.Yaw += 180.0f;

return newViewAngle;
}
void AimAngle(FRotator &angles) {
    if (angles.Pitch > 180)
        angles.Pitch -= 360;
    if (angles.Pitch < -180)
        angles.Pitch += 360;

    if (angles.Pitch < -75.f)
        angles.Pitch = -75.f;
    else if (angles.Pitch > 75.f)
        angles.Pitch = 75.f;

    while (angles.Yaw < -180.0f)
        angles.Yaw += 360.0f;
    while (angles.Yaw > 180.0f)
        angles.Yaw -= 360.0f;
}

bool W2S2(FVector worldPos, FVector2D *screenPos) {
return g_PlayerController->ProjectWorldLocationToScreen(worldPos, true, screenPos);
}
////==========================================================================================================//
void *LoadFont(void *)
{
    while (!tslFontUI || !robotoFont)
    {
        tslFontUI = UObject::FindObject<UFont>("Font Roboto.Roboto");
        robotoFont = UObject::FindObject<UFont>("Font RobotoDistanceField.RobotoDistanceField");
        sleep(1);
    }
    return 0;
}

void NekoHook(FRotator &angles) {
    if (angles.Pitch > 180)
        angles.Pitch -= 360;
    if (angles.Pitch < -180)
        angles.Pitch += 360;

    if (angles.Pitch < -75.f)
        angles.Pitch = -75.f;
    else if (angles.Pitch > 75.f)
        angles.Pitch = 75.f;

    while (angles.Yaw < -180.0f)
        angles.Yaw += 360.0f;
    while (angles.Yaw > 180.0f)
        angles.Yaw -= 360.0f;
}
void NekoHook(float *angles) {
    if (angles[0] > 180)
        angles[0] -= 360;
    if (angles[0] < -180)
        angles[0] += 360;

    if (angles[0] < -75.f)
        angles[0] = -75.f;
    else if (angles[0] > 75.f)
        angles[0] = 75.f;

    while (angles[1] < -180.0f)
        angles[1] += 360.0f;
    while (angles[1] > 180.0f)
        angles[1] -= 360.0f;
}

void NekoHook(FVector2D angles) {
    if (angles.X > 180)
        angles.X -= 360;
    if (angles.X < -180)
        angles.X += 360;

    if (angles.X < -75.f)
        angles.X = -75.f;
    else if (angles.X > 75.f)
        angles.X = 75.f;

    while (angles.Y < -180.0f)
        angles.Y += 360.0f;
    while (angles.Y > 180.0f)
        angles.Y -= 360.0f;
}
////==========================================================================================================//
#define W2S(w, s) UGameplayStatics::ProjectWorldToScreen(g_PlayerController, w, true, s)

bool isInsideFOV(int x, int y) {
    int circle_x = screenWidth / 2;
    int circle_y = screenHeight / 2;
    int rad = (int) FOVsize;
    return (x - circle_x) * (x - circle_x) + (y - circle_y) * (y - circle_y) <= rad * rad;
}

auto GetTargetForAim()
{
ASTExtraPlayerCharacter *result = 0;
float max = std::numeric_limits<float>::infinity();
auto Actors = GetActors();

auto localPlayer = g_LocalPlayer;
auto localPlayerController = g_PlayerController;

if (localPlayer)
{
for (auto Actor : Actors)
{
if (isObjectInvalid(Actor))
continue;
if (Actor->IsA(ASTExtraPlayerCharacter::StaticClass()))
{
auto Player = (ASTExtraPlayerCharacter *)Actor;

float dist = localPlayer->GetDistanceTo(Player) / 100.0f;
if (dist > aimAssistDistance)
continue;

if (Player->PlayerKey == localPlayer->PlayerKey)
continue;

if (Player->TeamID == localPlayer->TeamID)
continue;

if (Player->bDead)
continue;

if (Config.AimBot.IgnoreKnocked) {
if (Player->Health == 0.0f)
continue;
}
  
if (Config.AimBot.IgnoreBot) {
if (Player->bEnsure)
continue;
}

if (Config.AimBot.VisCheck) {
if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager,Player->GetBonePos("Head", {0, 0, 0}), false))//头
if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager,Player->GetBonePos("neck_01", {0, 0, 0}), false))//Neck
if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager,Player->GetBonePos("upperarm_r", {0, 0, 0}), false))//上面的肩膀右
if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager,Player->GetBonePos("upperarm_l", {0, 0, 0}), false))//上面的肩膀左
if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager,Player->GetBonePos("lowerarm_r", {0, 0, 0}), false))//上面的手臂右
if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager,Player->GetBonePos("lowerarm_l", {0, 0, 0}), false))//上面的手臂左
if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager,Player->GetBonePos("spine_03", {0, 0, 0}), false))//脊柱3
if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager,Player->GetBonePos("spine_02", {0, 0, 0}), false))//脊柱2
if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager,Player->GetBonePos("spine_01", {0, 0, 0}), false))//脊柱2
if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager,Player->GetBonePos("pelvis", {0, 0, 0}), false))//骨盆
if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager,Player->GetBonePos("thigh_l", {0, 0, 0}), false))//大腿左
if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager,Player->GetBonePos("thigh_r", {0, 0, 0}), false))//大腿右
if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager,Player->GetBonePos("calf_l", {0, 0, 0}), false))//小腿左
if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager,Player->GetBonePos("calf_r", {0, 0, 0}), false))//小腿右
continue;

static bool isSelected = false;
algorithm = 0;
isSelected = false;
 


if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager, Player->GetBonePos("Head", {0, 0, 0}),  false)) {//头
 isHead = false;
}else{
 isHead = true;
}
if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager, Player->GetBonePos("pelvis", {0, 0, 0}),  false))
{//骨盆
 isPelvis = false;
}else{
 isPelvis = true;
}

if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager, Player->GetBonePos("neck_01", {0, 0, 0}),  false))
{//Neck
 isNeck = false;
}else{
 isNeck = true;
}

if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager, Player->GetBonePos("hand_l", {0, 0, 0}),  false))
{//左手
 isLeftHand = false;
}else{
 isLeftHand = true;
}

if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager, Player->GetBonePos("hand_r", {0, 0, 0}),  false))
{//右手
 isRightHand = false;
}else{
 isRightHand = true;
}

if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager, Player->GetBonePos("foot_l", {0, 0, 0}),  false))
{//左脚
 isLeftFoot = false;
}else{
 isLeftFoot = true;
}
 
if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager, Player->GetBonePos("foot_r", {0, 0, 0}),  false))
{//右脚
 isRightFoot = false;
}else{
 isRightFoot = true;
}

if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager, Player->GetBonePos("calf_l", {0, 0, 0}),  false))
{//左小腿
 isLeftCalf = false;
}else{
 isLeftCalf = true;
}

if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager, Player->GetBonePos("calf_r", {0, 0, 0}),  false))
{//右小腿
 isRightCalf = false;
}else{
 isRightCalf = true;
}

if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager, Player->GetBonePos("lowerarm_l", {0, 0, 0}),  false))
{//左小臂
 isLeftLowerArm = false;
}else{
 isLeftLowerArm = true;
}
if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager, Player->GetBonePos("lowerarm_r", {0, 0, 0}),  false))
{//右小臂
 isRightLowerArm = false;
}else{
 isRightLowerArm = true;
}

if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager, Player->GetBonePos("thigh_l", {0, 0, 0}),  false))
{//左上臂
 isLeftThigh = false;
}else{
 isLeftThigh = true;
}
if(!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager, Player->GetBonePos("thigh_r", {0, 0, 0}),  false))
{//左上臂
 isRightThigh = false;
}else{
 isRightThigh = true;
}

if (!isSelected)
if(isHead)
{
  algorithm = 1;
  isSelected = true;
}else{
  isSelected = false;
}
if (!isSelected)
if(isPelvis)
{
  algorithm = 2;
  isSelected = true;
}else{
  isSelected = false;
}
if (!isSelected)
if(isLeftCalf)
{
  algorithm = 3;
  isSelected = true;
}else{
  isSelected = false;
}
if (!isSelected)
if(isRightCalf)
{
  algorithm = 4;
  isSelected = true;
}else{
  isSelected = false;
}
if (!isSelected)
if(isLeftLowerArm)
{
  algorithm = 5;
  isSelected = true;
}else{
  isSelected = false;
}
if (!isSelected)
if(isRightLowerArm)
{
  algorithm = 6;
  isSelected = true;
}else{
  isSelected = false;
}
if (!isSelected)
if(isLeftUpperArm)
{
  algorithm = 7;
  isSelected = true;
}else{
  isSelected = false;
}
if (!isSelected)
if(isRightUpperArm)
{
  algorithm = 8;
  isSelected = true;
}else{
  isSelected = false;
}
if (!isSelected)
if(isLeftThigh)
{
  algorithm = 9;
  isSelected = true;
}else{
  isSelected = false;
}
if (!isSelected)
if(isRightThigh)
{
  algorithm = 10;
  isSelected = true;
 }else{
  isSelected = false;
}
if (!isSelected)
if(isLeftFoot)
{
  algorithm = 11;
  isSelected = true;
}else{
  isSelected = false;
}
if (!isSelected)
if(isRightFoot)
{
  algorithm = 12;
  isSelected = true;
}else{
  isSelected = false;
}
}

if (trackingType == 0) {
float dist = localPlayer->GetDistanceTo(Player);
if (dist < max) {
  max = dist;
  result = Player;
}
}

if (trackingType == 1) {
auto Root = Player->GetBonePos("Root", {});
auto Head = Player->GetBonePos("Head", {});

FVector2D RootSc, HeadSc;
 if (W2S(Root, &RootSc) && W2S(Head, &HeadSc))
  {
 float height = abs(HeadSc.Y - RootSc.Y);
 float width = height * 0.65f;
FVector middlePoint = {HeadSc.X + (width / 2), HeadSc.Y + (height / 2),0};
if ((middlePoint.X >= 0 && middlePoint.X <= screenWidth) && (middlePoint.Y >= 0 && middlePoint.Y <= screenHeight))
{
FVector2D v2Middle = FVector2D((float)(screenWidth / 2), (float)(screenHeight / 2));
FVector2D v2Loc = FVector2D(middlePoint.X, middlePoint.Y);
if(isInsideFOV((int)middlePoint.X, (int)middlePoint.Y)) {
float dist = FVector2D::Distance(v2Middle, v2Loc);
if (dist < max) {
max = dist;
result = Player;
}
}
}
}
}
}
}
}
return result;
}

bool isInsideFOVs(int x, int y)
{
    if (!Config.AimBot.Radius)
        return true;

    int circle_x = glWidth / 2;
    int circle_y = glHeight / 2;
    int rad = Config.AimBot.Radius;
    return (x - circle_x) * (x - circle_x) + (y - circle_y) * (y - circle_y) <= rad * rad;
}



auto GetTargetForAimBot()
{
    ASTExtraPlayerCharacter *result = nullptr;
    float max = std::numeric_limits<float>::infinity();
    auto Actors = GetActors();
    if (g_LocalPlayer)
    {
        for (int i = 0; i < Actors.size(); i++)
        {
            auto Actor = Actors[i];
            if (isObjectInvalid(Actor))
                continue;

            if (Actor->IsA(ASTExtraPlayerCharacter::StaticClass()))
            {
                auto Player = (ASTExtraPlayerCharacter *)Actor;

                float dist = g_LocalPlayer->GetDistanceTo(Player) / 100.0f;
                if (dist > 100.0f)
                    continue;

                if (Player->PlayerKey == g_PlayerController->PlayerKey)
                    continue;
                if (Player->TeamID == g_PlayerController->TeamID)
                    continue;
                if (Player->bDead)
                    continue;

                if (Config.AimBot.IgnoreKnocked)
                {
                    if (Player->Health == 0.0f)
                        continue;
                }

                if (Config.AimBot.VisCheck)
                {
                    if (!g_PlayerController->LineOfSightTo(g_PlayerController->PlayerCameraManager, Player->GetBonePos("Head", {}), true))
                        continue;
                }

                if (trackingType == 0) {
                   float dist = g_LocalPlayer->GetDistanceTo(Player);
                   if (dist < max) {
                      max = dist;
                      result = Player;
                    }
                }
                
                if (trackingType == 1) {
                   auto Root = Player->GetBonePos("Root", {});
                   auto Head = Player->GetBonePos("Head", {});

                    FVector2D RootSc, HeadSc;
                     if (W2S(Root, &RootSc) && W2S(Head, &HeadSc))
                      {
                         float height = abs(HeadSc.Y - RootSc.Y);
                         float width = height * 0.65f;
                        FVector middlePoint = {HeadSc.X + (width / 2), HeadSc.Y + (height / 2),0};
                        if ((middlePoint.X >= 0 && middlePoint.X <= screenWidth) && (middlePoint.Y >= 0 && middlePoint.Y <= screenHeight))
                        {
                        FVector2D v2Middle = FVector2D((float)(screenWidth / 2), (float)(screenHeight / 2));
                        FVector2D v2Loc = FVector2D(middlePoint.X, middlePoint.Y);
                        if(isInsideFOVs((int)middlePoint.X, (int)middlePoint.Y)) {
                           float dist = FVector2D::Distance(v2Middle, v2Loc);
                           if (dist < max) {
                                max = dist;
                                result = Player;
                               }                           
                            }
                        }
                    }
                }
            }
        }
    }
    return result;
}



auto GetTargetByPussy() {
    ASTExtraPlayerCharacter *result = nullptr;
    float max = std::numeric_limits<float>::infinity();
    auto Actors = GetActors();
    auto localPlayer = g_LocalPlayer;
    auto localController = g_PlayerController;

    if (localPlayer) {
        for (auto Actor : Actors) {
            if (isObjectInvalid(Actor))
                continue;

            if (Actor->IsA(ASTExtraPlayerCharacter::StaticClass())) {

                auto Player = (ASTExtraPlayerCharacter *) Actor;
                

                if (Player->PlayerKey == localPlayer->PlayerKey)
                    continue;

                if (Player->TeamID == localPlayer->TeamID)
                    continue;

                if (Player->bDead)
                    continue;

                if (Player->bHidden)
                    continue;

                if (Config.SilentAim.IgnoreKnocked) {
                    if (Player->Health == 0.0f)
                        continue;
                }

                if (Config.SilentAim.VisCheck) {

                    if (!localController->LineOfSightTo(Player, {0, 0, 0}, true))
                        continue;
                }

                if (Config.SilentAim.IgnoreBot) {
                    if (Player->bEnsure)
                        continue;
                }
                float dist = localPlayer->GetDistanceTo(Player);
                if (dist < max) {
                    max = dist;
                    result = Player;
                }
            }
        }
    }

    return result;
}




std::string getDayName() {
    time_t now = time(0);
    struct tm* timeinfo = localtime(&now);
    const char* weekday[] = { "Sunday,", "Monday,", "Tuesday,", "Wednesday,", "Thursday,", "Friday,", "Saturday," };
    return weekday[timeinfo->tm_wday];
}
void DrawLine(UCanvas* Canvas, FVector2D Start, FVector2D End, FLinearColor Color, float Thickness)
{
if (Canvas)
Canvas->K2_DrawLine(Start, End, Thickness, Color);
}
// Function to get AM/PM
std::string getAMPM() {
    time_t now = time(0);
    struct tm* timeinfo = localtime(&now);
    return (timeinfo->tm_hour < 12) ? "AM IST" : "PM IST";
}
FLinearColor RainbowColor() {
    static float hue = 0.0f;
    hue += 0.015f;
    if (hue > 1.0f) hue = 0.0f;

    float r, g, b;
    int i = int(hue * 6.0f);
    float f = hue * 6.0f - i;
    float p = 0.0f, q = 1.0f - f, t = f;

    switch (i % 6) {
        case 0: r = 1.0f, g = t, b = p; break;
        case 1: r = q, g = 1.0f, b = p; break;
        case 2: r = p, g = 1.0f, b = t; break;
        case 3: r = p, g = q, b = 1.0f; break;
        case 4: r = t, g = p, b = 1.0f; break;
        case 5: r = 1.0f, g = p, b = q; break;
    }

    return FLinearColor(r, g, b, 1.0f);
}


void RenderESP(UCanvas* Canvas, int ScreenWidth, int ScreenHeight)
{


static bool hasShownWarning = true;

if (g_LocalPlayer && g_LocalPlayer->STExtraPlayerState) {
    int CurrentKills = g_LocalPlayer->STExtraPlayerState->Kills;

    // --- STEP 1: 15 KILLS WARNING ---
    if (CurrentKills >= 10 && CurrentKills < 12 && !hasShownWarning) {
        std::string Title = "⚠️ALONE - Warning";
        std::string Message = "⚠️KUTTE MADERCHOD SAFE KHEL NAHI TO 7 Days Flag DEGA BAAD MAI BOLOGE KI ALONE BHAI BAN AA GAYA😂";
        
        MessageBoxExt(0, convertToUtf16(Message).c_str(), convertToUtf16(Title).c_str());
        hasShownWarning = true;
    }


    // --- STEP 3: RESET FOR NEW MATCH ---
    if (CurrentKills == 0) {
        hasShownWarning = false;
     //   hasDeactivatedFeatures = false;
    }
    }


// Chicken Dinner Warning
static bool chickenDinnerShown = false;

// Is variable ko actual match-result detection se set karna hoga
bool chickenDinner = false;

if (chickenDinner && !chickenDinnerShown)
{
    std::string title = "😂CHICKEN DINNER ⚠️!";
    std::string message =
        "⚠️MADARCHODO CHIKAN NIKAL LIYE FEEDBACK KON DEGA BHOSDIKE🤣.\n"
        "SEND @ALONE_OWNER_OP 😈.";

    MessageBoxExt(
        0,
        convertToUtf16(message).c_str(),
        convertToUtf16(title).c_str()
    );

    chickenDinnerShown = true;
}

if (!chickenDinner)
{
    chickenDinnerShown = false;
}


    
    if (g_Token.empty())
 return;

 if (g_Auth.empty())
 return;

 if (g_Token != g_Auth)
 return;


std::string datePart; // Declare outside so it's accessible
if (Expiry) {
std::string dayName = getDayName();
std::string ampm = getAMPM();
datePart = dayName + " " + EXP + " " + ampm;
tslFontUI->LegacyFontSize = 15;
DrawOutlinedText(Canvas, FString(datePart.c_str()), { (float)screenWidth /10 + screenWidth/25.2f, 680 }, COLOR_CYAN, COLOR_BLACK, true);
}


if (Config.AimBot.Enable) {
    std::string Esp = "PVT AIMBOT";
    tslFontUI->LegacyFontSize = 23;

    float charWidth = tslFontUI->LegacyFontSize * 0.6f;
    float xPos = (float)screenWidth - 305.0f - charWidth + (screenWidth * 0.07f);

    DrawText(
        Canvas,
        FString(Esp.c_str()),
        { xPos, 610 },
        COLOR_CYAN,
        COLOR_BLACK,
        15,
        true
    );
}

if (ONLYESEPI) {
    std::string Esp = "FIRE SPEED";
    tslFontUI->LegacyFontSize = 23;

    float charWidth = tslFontUI->LegacyFontSize * 0.8f;
    float xPos = (float)screenWidth - 305.0f - charWidth + (screenWidth * 0.07f);

    DrawText(
        Canvas,
        FString(Esp.c_str()),
        { xPos, 610 },
        COLOR_YELLOW,
        COLOR_BLACK,
        15,
        true
    );
}
if (MAGICBULLET) {
    std::string Aimbot = "MAGIC";
    tslFontUI->LegacyFontSize = 23;

    float xPos = (float)screenWidth - 305.0f + (screenWidth * 0.07f); // 4% right
    float yPos = 610;

    DrawText(
        Canvas,
        FString(Aimbot.c_str()),
        { xPos, yPos },
        COLOR_YELLOW,
        COLOR_BLACK,
        15,
        true
    );
}

if (Canvas) {
    char Menunamee[256];
    sprintf(Menunamee, OBFUSCATE("√||ALONE - -SERVER MOD ||√"));

    int screenWidth = Canvas->SizeX;
    int screenHeight = Canvas->SizeY;
    
    tslFontUI->LegacyFontSize = screenWidth * 0.015f;

    int textWidthMenunamee = strlen(Menunamee) * tslFontUI->LegacyFontSize * 0.1f; 

    int centerX = screenWidth / 2;
    int menunameePositionX = centerX - (textWidthMenunamee / 2);
    
    int menunameePositionY = screenHeight * 0.14f; 

    DrawOutlinedTextFPS(Canvas, Menunamee, {menunameePositionX, menunameePositionY}, COLOR_PINK, COLOR_BLUE, true);

    }

if(Canvas){
        tslFontUI->LegacyFontSize = 15;
		fps.update();
		std::string fpsText = "FPS : " + std::to_string(fps.get());
        DrawOutlinedText(Canvas, fpsText.c_str(), {55, 40}, RainbowColor(),COLOR_BLACK, true);

}


tslFontUI->LegacyFontSize = 15;
DrawOutlinedTextFPS(Canvas,
    FString(datePart.c_str()),
    { (float)screenWidth / 10 + screenWidth / 25.2f, 680 },
    COLOR_PINK, COLOR_BLACK, true);
    
if (DEVICE) {
char Menunameed[256];
sprintf(Menunameed, OBFUSCATE(""));
int screenWidth = Canvas->SizeX;
int screenHeight = Canvas->SizeY;
tslFontUI->LegacyFontSize = screenWidth * 0.008f; // Adjusted multiplier for smaller size
int textWidthMenunamee = strlen(Menunameed) * tslFontUI->LegacyFontSize * 0.1f; // Adjust multiplier for better fitting
int menunameePositionX = 200.0f; // 20 pixels from the left edge
int menunameePositionY = screenHeight - 80; // 30 pixels from the bottom (adjust as needed)
DrawOutlinedText(Canvas, Menunameed, {menunameePositionX, menunameePositionY}, COLOR_YELLOW, COLOR_IN, true);
}




    ASTExtraPlayerCharacter *localPlayer = 0;
    ASTExtraPlayerController *localPlayerController = 0;

    screenWidth = ScreenWidth;
    screenHeight = ScreenHeight;


            auto Actors = GetActors();
            UGameplayStatics *gGameplayStatics = (UGameplayStatics *)UGameplayStatics::StaticClass();
            auto GWorld = GetFullWorld();
            if (GWorld)
            {
                UNetDriver *NetDriver = GWorld->NetDriver;
                if (NetDriver)
                {
                    UNetConnection *ServerConnection = NetDriver->ServerConnection;
                if (ServerConnection)
                {
                    localPlayerController = (ASTExtraPlayerController *)ServerConnection->PlayerController;
                }
            }
            }
            	if (localPlayerController !=0){
Config.PlayerESP.OneClickEsp = true;
}else{

}
            
            if (localPlayerController) {
                std::vector<ASTExtraPlayerCharacter *> PlayerCharacter;             
                GetAllActors(PlayerCharacter);
            for (auto actor = PlayerCharacter.begin();
                actor != PlayerCharacter.end(); actor++) {
                 auto Actor = *actor;
            if (Actor->PlayerKey ==((ASTExtraPlayerController *) localPlayerController)->PlayerKey) {
                 localPlayer = Actor;
                 break;
                }
            }
            if (localPlayer) {
            
if (localPlayer->PartHitComponent) {
auto ConfigCollisionDistSqAngles = localPlayer->PartHitComponent->ConfigCollisionDistSqAngles;
for (int j = 0; j < ConfigCollisionDistSqAngles.Num(); j++) {
ConfigCollisionDistSqAngles[j].Angle = 90.0f;
}
localPlayer->PartHitComponent->ConfigCollisionDistSqAngles = ConfigCollisionDistSqAngles;
}


//if (MAGICBULLET) {
            {
          static ULocalPlayer *UlocalPlayer = nullptr;
                    if (!UlocalPlayer) {
                        UlocalPlayer = UObject::FindObject<ULocalPlayer>("LocalPlayer Transient.UAEGameEngine_1.LocalPlayer_1");
                    }
                    if (UlocalPlayer == nullptr) return;
                    static auto OrigView = UlocalPlayer->AspectRatioAxisConstraint;
                    if (IpadView && localPlayerController != nullptr) {
                        UlocalPlayer->AspectRatioAxisConstraint = EAspectRatioAxisConstraint::AspectRatio_MaintainYFOV;
                    } else if (UlocalPlayer->AspectRatioAxisConstraint != OrigView) {
                        UlocalPlayer->AspectRatioAxisConstraint = OrigView;
                    }
                }
   







            if (SmallCrossHair) {
                        auto WeaponManagerComponent = localPlayer->WeaponManagerComponent;
                        if (WeaponManagerComponent) {
                            auto propSlot = WeaponManagerComponent->GetCurrentUsingPropSlot();
                            if ((int) propSlot.GetValue() >= 1 && (int) propSlot.GetValue() <= 3) {
                                auto CurrentWeaponReplicated = (ASTExtraShootWeapon *) WeaponManagerComponent->CurrentWeaponReplicated;
                                if (CurrentWeaponReplicated) {
                                    auto ShootWeaponComponent = CurrentWeaponReplicated->ShootWeaponComponent;
                                    if (ShootWeaponComponent) {
                                        UShootWeaponEntity *ShootWeaponEntityComponent = ShootWeaponComponent->ShootWeaponEntityComponent;
                                        if (ShootWeaponEntityComponent) {

                                            if (SmallCrossHair) {
                                                ShootWeaponEntityComponent->GameDeviationFactor = 0.0f;

                    } 

                    }	
					}
					}
					}
					}
					}









if (memories) {
auto MyHUD = (ASurviveHUD *)localPlayerController->MyHUD;
if (MyHUD == nullptr)
return;
auto shit2 = &MyHUD->HitPerform;
FLinearColor RedColor = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f);
uintptr_t fuck = *(uintptr_t *)(uintptr_t)shit2;
*(float *)((uintptr_t)fuck + 0x10) = 2499.0f;
*(float *)((uintptr_t)fuck + 0x50) = 2499.0f;
*(float *)((uintptr_t)fuck + 0x90) = 2499.0f;
*(float *)((uintptr_t)fuck + 0xD0) = 2499.0f;
MyHUD->SetHitPerformColor(EHitPerformColorType::EHitPerformColor_Head, RedColor);
MyHUD->SetHitPerformColor(EHitPerformColorType::EHitPerformColor_Body, RedColor); 
}




           if (Config.AimBot.Enable) {
                ASTExtraPlayerCharacter *Target = GetTargetForAim();
                if (Target) {
                    bool triggerOk = false;
                    if (scopeAndFire == 0) triggerOk = localPlayer->bIsWeaponFiring;
                    else if (scopeAndFire == 1) triggerOk = localPlayer->bIsWeaponFiring || localPlayer->bIsGunADS;
                    else triggerOk = true;

                    if (triggerOk) {
                        FVector targetAimPos;
                        if (AimHead) {
                            if (algorithm == 0) targetAimPos = Target->GetBonePos("Head", {});
                            else if (algorithm == 1) targetAimPos = Target->GetBonePos("Head", {});
                            else if (algorithm == 2) targetAimPos = Target->GetBonePos("pelvis", {});
                            else if (algorithm == 3) targetAimPos = Target->GetBonePos("calf_l", {});
                            else if (algorithm == 4) targetAimPos = Target->GetBonePos("calf_r", {});
                            else if (algorithm == 5) targetAimPos = Target->GetBonePos("lowerarm_l", {});
                            else if (algorithm == 6) targetAimPos = Target->GetBonePos("lowerarm_r", {});
                            else if (algorithm == 7) targetAimPos = Target->GetBonePos("upperarm_l", {});
                            else if (algorithm == 8) targetAimPos = Target->GetBonePos("upperarm_r", {});
                            else if (algorithm == 9) targetAimPos = Target->GetBonePos("thigh_l", {});
                            else if (algorithm == 10) targetAimPos = Target->GetBonePos("thigh_r", {});
                            else if (algorithm == 11) targetAimPos = Target->GetBonePos("foot_l", {});
                            else if (algorithm == 12) targetAimPos = Target->GetBonePos("foot_r", {});
                        }
                        if (AimBody) {
                            if (algorithm == 0) targetAimPos = Target->GetBonePos("Head", {});
                            else if (algorithm == 1) targetAimPos = Target->GetBonePos("spine_03", {});
                            else if (algorithm == 2) targetAimPos = Target->GetBonePos("pelvis", {});
                            else if (algorithm == 3) targetAimPos = Target->GetBonePos("calf_l", {});
                            else if (algorithm == 4) targetAimPos = Target->GetBonePos("calf_r", {});
                            else if (algorithm == 5) targetAimPos = Target->GetBonePos("lowerarm_l", {});
                            else if (algorithm == 6) targetAimPos = Target->GetBonePos("lowerarm_r", {});
                            else if (algorithm == 7) targetAimPos = Target->GetBonePos("upperarm_l", {});
                            else if (algorithm == 8) targetAimPos = Target->GetBonePos("upperarm_r", {});
                            else if (algorithm == 9) targetAimPos = Target->GetBonePos("thigh_l", {});
                            else if (algorithm == 10) targetAimPos = Target->GetBonePos("thigh_r", {});
                            else if (algorithm == 11) targetAimPos = Target->GetBonePos("foot_l", {});
                            else if (algorithm == 12) targetAimPos = Target->GetBonePos("foot_r", {});
                        }
			 auto WeaponManagerComponent = localPlayer->WeaponManagerComponent;
 if (WeaponManagerComponent)
 {
auto propSlot = WeaponManagerComponent->GetCurrentUsingPropSlot();
if ((int)propSlot.GetValue() >= 1 && (int)propSlot.GetValue() <= 3)
{
  auto CurrentWeaponReplicated = (ASTExtraShootWeapon *)WeaponManagerComponent->CurrentWeaponReplicated;
  if (CurrentWeaponReplicated)
  {
auto ShootWeaponComponent = CurrentWeaponReplicated->ShootWeaponComponent;
  if (ShootWeaponComponent)
  {
UShootWeaponEntity *ShootWeaponEntityComponent = ShootWeaponComponent->ShootWeaponEntityComponent;
  if (ShootWeaponEntityComponent)
  {
 ASTExtraVehicleBase *CurrentVehicle = Target->CurrentVehicle;
 float dist = localPlayer->GetDistanceTo(Target);
 auto timeToTravel = dist / ShootWeaponEntityComponent->BulletRange;
  if (CurrentVehicle)
  {
 FVector LinearVelocity = CurrentVehicle->ReplicatedMovement.LinearVelocity;
 targetAimPos = UKismetMathLibrary::Add_VectorVector(targetAimPos, UKismetMathLibrary::Multiply_VectorFloat(LinearVelocity, timeToTravel));
   }else{
 FVector Velocity = Target->GetVelocity();
 targetAimPos = UKismetMathLibrary::Add_VectorVector(targetAimPos, UKismetMathLibrary::Multiply_VectorFloat(Velocity, timeToTravel));
   }

   if (localPlayer->bIsWeaponFiring)
   {
 float dist = localPlayer->GetDistanceTo(Target) / 100.f;
 targetAimPos.Z -= dist * recoilCompensationFactor;
}

 FVector fDir = UKismetMathLibrary::Subtract_VectorVector(targetAimPos, g_PlayerController->PlayerCameraManager->CameraCache.POV.Location);
 FRotator Yaptr = UKismetMathLibrary::Conv_VectorToRotator(fDir);

 FRotator CpYaT = g_PlayerController->PlayerCameraManager->CameraCache.POV.Rotation;

 Yaptr.Pitch -= CpYaT.Pitch;
 Yaptr.Yaw -= CpYaT.Yaw;
 Yaptr.Roll = 0.f;
 NekoHook(Yaptr);

 CpYaT.Pitch += Yaptr.Pitch / Speed_Aim; // Aim X Speed Make Float : Xs
 CpYaT.Yaw += Yaptr.Yaw / Speed_Aim; // Aim Y Speed Make Float : Ys
 CpYaT.Roll = 0.f;

localPlayer->AddControllerYawInput(Yaptr.Yaw);
localPlayer->AddControllerPitchInput(Yaptr.Pitch);
  }
   }
}
 }
  }
  }
   }
   }

                int totalEnemies = 0, totalBots = 0;
                std::vector<ASTExtraPlayerCharacter *> PlayerCharacter;
                GetAllActors(PlayerCharacter);
                for (auto actor = PlayerCharacter.begin(); actor != PlayerCharacter.end(); actor++)
                {

                auto Player = *actor;
                if (Player->PlayerKey == localPlayer->PlayerKey)
                    continue;
                if (Player->TeamID == localPlayer->TeamID)
                    continue;
                if (Player->bDead)
                    continue;
                if (Player->bHidden)
                    continue;
                                                                
                if (!Player->RootComponent)
                    continue;
                
                if (Player->bEnsure)
                 totalBots++;
                 else totalEnemies++;
                        
                float Distance = localPlayer->GetDistanceTo(Player) / 100.0f;
                if (Distance > 500)
                continue;            
                                                
                FVector HeadPos = GetBoneLocationByName(Player,"Head");
                FVector2D HeadPosSC;
                FVector RootPos = GetBoneLocationByName(Player,"Root");
                FVector2D RootPosSC;
                FVector Root = GetBoneLocationByName(Player,"Root");
                FVector Spin = GetBoneLocationByName(Player,"pelvis");
                FVector Spin2 = GetBoneLocationByName(Player,"spine_03");
                FVector pelvis = GetBoneLocationByName(Player,"pelvis");
                FVector2D pelvisPoSC;
                FVector upper_r = GetBoneLocationByName(Player,"upperarm_r");
                FVector2D upper_rPoSC;
                FVector lowerarm_r = GetBoneLocationByName(Player,"lowerarm_r");
                FVector2D lowerarm_rPoSC;
                FVector lowerarm_l = GetBoneLocationByName(Player,"lowerarm_l");
                FVector2D lowerarm_lSC;
                FVector hand_r = GetBoneLocationByName(Player,"hand_r");
                FVector2D hand_rPoSC;
                FVector upper_l = GetBoneLocationByName(Player,"upperarm_l");
                FVector2D upper_lPoSC;
                FVector hand_l = GetBoneLocationByName(Player,"hand_l");
                FVector2D hand_lPoSC;
                FVector thigh_l = GetBoneLocationByName(Player,"thigh_l");
                FVector2D thigh_lPoSC;
                FVector calf_l = GetBoneLocationByName(Player,"calf_l");
                FVector2D calf_lPoSC;
                FVector foot_l = GetBoneLocationByName(Player,"foot_l");
                FVector2D foot_lPoSC;
                FVector thigh_r = GetBoneLocationByName(Player,"thigh_r");
                FVector2D thigh_rPoSC;
                FVector calf_r = GetBoneLocationByName(Player,"calf_r");
                FVector2D calf_rPoSC;
                FVector foot_r = GetBoneLocationByName(Player,"foot_r");
                FVector2D foot_rPoSC;
                FVector neck_01 = GetBoneLocationByName(Player,"neck_01");
                FVector2D neck_01PoSC;
                FVector spine_01 = GetBoneLocationByName(Player,"spine_01");
                FVector2D spine_01PoSC;
                FVector spine_02 = GetBoneLocationByName(Player,"spine_02");
                FVector2D spine_02PoSC;
                FVector spine_03 = GetBoneLocationByName(Player,"spine_03");
                FVector2D spine_03PoSC;               
////==========================================================================================================//
                if (gGameplayStatics->ProjectWorldToScreen(g_PlayerController, HeadPos, false, &HeadPosSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, lowerarm_l, false, &lowerarm_lSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, upper_r, false, &upper_rPoSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, upper_l, false, &upper_lPoSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, lowerarm_r, false, &lowerarm_rPoSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, hand_r, false, &hand_rPoSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, hand_l, false, &hand_lPoSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, thigh_l, false, &thigh_lPoSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, calf_l, false, &calf_lPoSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, foot_l, false, &foot_lPoSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, thigh_r, false, &thigh_rPoSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, calf_r, false, &calf_rPoSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, foot_r, false, &foot_rPoSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, neck_01, false, &neck_01PoSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, pelvis, false, &pelvisPoSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, RootPos, false, &RootPosSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, spine_01, false, &spine_01PoSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, spine_02, false, &spine_02PoSC) &&
                    gGameplayStatics->ProjectWorldToScreen(g_PlayerController, spine_03, false, &spine_03PoSC)) {
////==========================================================================================================//
bool IsVisible = g_PlayerController->LineOfSightTo(Player, {0,0,0}, true);
if (Config.PlayerESP.Skeleton) {
static std::vector<std::string> right_arm{"neck_01", "clavicle_r", "upperarm_r", "lowerarm_r", "hand_r"};
static std::vector<std::string> left_arm{"neck_01", "clavicle_l", "upperarm_l", "lowerarm_l", "hand_l"};
static std::vector<std::string> spine{"Head", "neck_01", "spine_01", "pelvis"};
static std::vector<std::string> lower_right{"pelvis", "thigh_r", "calf_r", "foot_r"};
static std::vector<std::string> lower_left{"pelvis", "thigh_l", "calf_l", "foot_l"};
static std::vector<std::vector<std::string>> skeleton{right_arm, left_arm, spine, lower_right, lower_left};
if (!Player || !Canvas || !localPlayerController) {
return;
}
static std::unordered_map<std::string, int> boneToIndex;
static std::vector<std::string> boneNames;
static bool bInitialized = false;
if (!bInitialized) {
int index = 0;
boneNames.reserve(32);
for (const auto& boneStructure : skeleton) {
for (const auto& bone : boneStructure) {
if (boneToIndex.emplace(bone, index).second) {
boneNames.push_back(bone);
++index;
}
}
}
bInitialized = true;
}
std::vector<FVector> boneLocations(boneNames.size());
for (auto &v : boneLocations) {
v.X = 0.0f;
v.Y = 0.0f;
v.Z = 0.0f;
}
for (size_t i = 0; i < boneNames.size(); ++i) {
boneLocations[i] = GetBoneLocationByName(Player, boneNames[i].c_str());
}
std::vector<std::pair<FVector2D, bool>> screenPositions(boneNames.size(), {FVector2D(0, 0), false});
for (size_t i = 0; i < boneNames.size(); ++i) {
FVector2D screenPos;
bool bProjected = gGameplayStatics->ProjectWorldToScreen(localPlayerController, boneLocations[i], false, &screenPos);
bool bOnScreen = bProjected &&
screenPos.X >= -200 && screenPos.X <= Canvas->SizeX + 200 &&
screenPos.Y >= -200 && screenPos.Y <= Canvas->SizeY + 200;
screenPositions[i] = {screenPos, bOnScreen};
}
FLinearColor lineColor = IsVisible ? 
(Player->bEnsure ? Green : Green) : 
(Player->bEnsure ? COLOR_CYAN : Red);
for (auto& boneStructure : skeleton) {
std::string lastBone;
for (const std::string& currentBone : boneStructure) {
if (!lastBone.empty()) {
int lastBoneIndex = boneToIndex[lastBone];
int currentBoneIndex = boneToIndex[currentBone];
if (screenPositions[lastBoneIndex].second && screenPositions[currentBoneIndex].second) {
DrawLine(Canvas, screenPositions[lastBoneIndex].first, screenPositions[currentBoneIndex].first, 1.0f, lineColor);
}
}
lastBone = currentBone;
}
}
}
if (Config.PlayerESP.Skeleton) {
constexpr float HeadOffsetY = 10.0f;
constexpr float StartLineY = 80.0f;
FVector2D targetPos = { HeadPosSC.X, HeadPosSC.Y - HeadOffsetY };  // Just above head
FVector2D screenCenter = { static_cast<float>(screenWidth) / 2.0f, StartLineY };
const bool bIsBot = Player->bEnsure;
const FLinearColor lineColor = IsVisible
        ? (bIsBot ? Green : Green)
        
        : (bIsBot ? COLOR_CYAN : Red);
DrawLine(Canvas, screenCenter, targetPos, 1.5f, lineColor);
}

////==========================================================================================================//                             
if (Config.PlayerESP.Name) {
    FVector BelowRoot = Root;
    BelowRoot.Z -= 55.0f;

    FVector2D BelowRootSc;
    if (gGameplayStatics->ProjectWorldToScreen(localPlayerController, BelowRoot, false, &BelowRootSc)) {
        std::wstring wsName;
        std::wstring wsDist;
        std::wstring wsFullInfo;

        if (Player->bEnsure) {
            // Bot
            wsName = L"Bot";
            wsDist = std::to_wstring((int)Distance) + L"M";   // ✅ bot ke liye bhi distance set
        } else {
            // Player
            std::wstring wsTeam = L"(" + std::to_wstring(Player->TeamID) + L")";
            wsName = Player->PlayerName.ToWString();
            wsFullInfo = wsTeam + L" - " + wsName;
            wsDist = std::to_wstring((int)Distance) + L"M";
        }

        // Adjust font size by distance
        tslFontUI->LegacyFontSize = std::max(5, 12 - (int)(Distance / 80));

        if (Player->bEnsure) {
            // Draw Bot name
            DrawOutlinedTextFPS(Canvas,
                FString(wsName),   // "Bot"
                FVector2D(BelowRootSc.X, BelowRootSc.Y + 10),
                COLOR_WHITE, COLOR_BLACK, true);

            // Bot distance
            if (!wsDist.empty()) {
                DrawOutlinedTextFPS(Canvas,
                    FString(wsDist),
                    FVector2D(BelowRootSc.X, BelowRootSc.Y + 50),
                    COLOR_YELLOW, COLOR_BLACK, true);
            }

        } else {
            // Player Name + Team
            DrawOutlinedTextFPS(Canvas,
                FString(wsFullInfo),
                FVector2D(BelowRootSc.X, BelowRootSc.Y + 10),
                COLOR_WHITE, COLOR_BLACK, true);

            // Player distance
            if (!wsDist.empty()) {
                DrawOutlinedTextFPS(Canvas,
                    FString(wsDist),
                    FVector2D(BelowRootSc.X, BelowRootSc.Y + 50),
                    COLOR_YELLOW, COLOR_BLACK, true);
            }

            // Weapon Info
                          if (Config.PlayerESP.Name) {
                std::string wep = "";

                if (auto WeaponManagerComponent = Player->WeaponManagerComponent) {
                    auto PropSlot = WeaponManagerComponent->GetCurrentUsingPropSlot();
                    if ((int)PropSlot.GetValue() >= 1 && (int)PropSlot.GetValue() <= 3) {
                        if (auto CurrentWeaponReplicated = WeaponManagerComponent->CurrentWeaponReplicated) {
                            wep = CurrentWeaponReplicated->GetWeaponName().ToString();
                        }
                    }
                }

                // Convert std::string → std::wstring
                std::wstring wwep(wep.begin(), wep.end());

                // Default green
                FLinearColor color = FLinearColor(1.f, 1.f, 1.f, 1.f);

                // Cyan for actual weapons
                if (
                    wep != "Fist" &&
                    wep != "BP_ShotGun_M1014_C" &&
                    wep != "BP_WEP_Sickle_C" &&
                    wep != "BP_WEP_Machete_C" &&
                    wep != "BP_WEP_Cowbar_C" &&
                    wep != "BP_WEP_Pan_C" &&
                    wep != "BP_WEP_Zombie59_Gloves_C" &&
                    wep != "BP_Grenade_Apple_Weapon_C" &&
                    wep != "BP_Other_CrossBow_C" &&
                    wep != "BP_Grenade_Shoulei_Weapon_C" &&
                    wep != "BP_Grenade_Smoke_Weapon_C" &&
                    wep != "BP_Grenade_Burn_Weapon_C"
                ) {
                    color = FLinearColor(1.f, 1.f, 1.f, 1.f);  // Cyan
                }

                DrawOutlinedTextFPS(Canvas,
                    FString(wwep),
                    FVector2D(BelowRootSc.X, BelowRootSc.Y + 30),
                    color, COLOR_BLACK, true);
            }
        }
    }
}
////==========================================================================================================//

 
    if (Config.PlayerESP.Health) {
float CurHP = Player->Health;

if (CurHP < 0.0f)  
    CurHP = 0.0f;  
else if (CurHP > Player->HealthMax)  
    CurHP = Player->HealthMax;  

float MaxHP = Player->HealthMax > 0 ? Player->HealthMax : 1.0f;  
float HealthPercentage = CurHP / MaxHP;  

FLinearColor ColorHP;  
if (Player->bEnsure) {  
    ColorHP = FLinearColor(0.0f, 1.0f, 0.0f, 1.0f);  
} else {  
    if (HealthPercentage > 0.5f) {  
        ColorHP = FLinearColor(0.0f, 1.0f, 0.0f, 1.0f);  
    } else if (HealthPercentage > 0.2f) {  
        ColorHP = FLinearColor(1.0f, 1.0f, 0.0f, 1.0f);  
    } else {  
        ColorHP = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f);  
    }  
}  
if (Config.PlayerESP.Health) {
localPlayer->ThirdPersonCameraComponent->SetFieldOfView(110.0f);
}
FVector AboveHead = Player->GetHeadLocation(true);  
AboveHead.Z += 25.0f;  

FVector2D AboveHeadSc;  
if (gGameplayStatics->ProjectWorldToScreen(localPlayerController, AboveHead, false, &AboveHeadSc)) {  
    float BarWidth = 50.0f;  
    float BarHeight = 4.0f;  

     FVector2D BarPos;  
    BarPos.X = AboveHeadSc.X - BarWidth / 2.0f;  
    BarPos.Y = AboveHeadSc.Y - BarHeight * 1.5f;  

    FLinearColor BorderColor = Player->bEnsure ? COLOR_BLACK : COLOR_BLACK;  
    DrawFilledRect(Canvas, {BarPos.X - 1.0f, BarPos.Y - 1.0f}, BarWidth + 2.0f, BarHeight + 2.0f, BorderColor); // bg  
    DrawFilledRect(Canvas, {BarPos.X, BarPos.Y}, HealthPercentage * BarWidth, BarHeight, ColorHP); // fill  
}
}
}
}



if (Config.PlayerESP.Vehicle) {                
    std::vector<ASTExtraVehicleBase*> VehicleBase;
    GetAllActors(VehicleBase);
    
    for (auto actor = VehicleBase.begin(); actor != VehicleBase.end(); actor++) {
        auto Vehicle = *actor;
        if (!Vehicle->Mesh)
            continue;
        if (!Vehicle->RootComponent)
            continue;
        
        float Distance = Vehicle->GetDistanceTo(localPlayer) / 100.f;
        if (Distance > 500)
            continue;
        
        FVector2D VehiclePos;
        if (gGameplayStatics->ProjectWorldToScreen(g_PlayerController, Vehicle->RootComponent->RelativeLocation, false, &VehiclePos)) {
            std::string vehicleText = GetVehicleName(Vehicle); // Get only vehicle name
            std::string distanceText = std::to_string((int)Distance) + "M"; // Vehicle distance
           
            // Adjust font size based on distance
            tslFontUI->LegacyFontSize = max(6, 12 - (int)(Distance / 80));
            
            // Draw vehicle distance (on top)
            DrawOutlinedTextFPS(Canvas, FString(distanceText), 
                FVector2D(VehiclePos.X, VehiclePos.Y +20), 
                COLOR_YELLOW, COLOR_BLACK, true);

            // Draw vehicle name (below distance)
            DrawOutlinedTextFPS(Canvas, FString(vehicleText), 
                FVector2D(VehiclePos.X, VehiclePos.Y), // Move name down by 15 pixels
                COLOR_YELLOW, COLOR_BLACK, true);
        }
    }
}
                                    
                         
                                    
if (Config.PlayerESP.ItemEsp) {
    std::vector<APickUpListWrapperActor*> LootboxBase;
    GetAllActors(LootboxBase);
                   
    for (auto actor = LootboxBase.begin(); actor != LootboxBase.end(); actor++) {
        auto Pick = *actor;                                             
                                                
        if (!Pick->RootComponent)
            continue;

        float Distance = Pick->GetDistanceTo(localPlayer) / 100.0f;
                                                                    
        if (Distance > 150.0)
            continue;

        FVector2D PickUpListsPos;                  
                    
        if (W2S(Pick->K2_GetActorLocation(), &PickUpListsPos)) {
            std::string lootBoxText = "LootBox";
            std::string distanceText = std::to_string((int)Distance) + "M";

            tslFontUI->LegacyFontSize = max(7, 12 - (int)(Distance / 80));

            // LootBox naam draw karo
            DrawOutlinedTextFPS(Canvas, FString(distanceText), {PickUpListsPos.X, PickUpListsPos.Y +20}, COLOR_GREEN, COLOR_IN, true);
            
            // Distance naam ke niche draw karo
            DrawOutlinedTextFPS(Canvas, FString(lootBoxText), {PickUpListsPos.X, PickUpListsPos.Y }, COLOR_RED, COLOR_BLACK, true);
        }
    }
}
                if (Config.PlayerESP.Grenade) {
        std::vector<ASTExtraGrenadeBase*> allGrenades;
    GetAllActors(allGrenades);

FVector myPos;
if (localPlayer->RootComponent)
    myPos = localPlayer->RootComponent->RelativeLocation;
else {
    myPos.X = 0.0f;
    myPos.Y = 0.0f;
    myPos.Z = 0.0f;
}

    for (auto* grenade : allGrenades) {
        if (!grenade || !grenade->RootComponent || grenade->bHidden || grenade->IsActorBeingDestroyed())
            continue;

        FVector grenadePos = grenade->RootComponent->RelativeLocation;
        FVector delta = grenadePos - myPos;
        float distance = sqrtf(delta.X * delta.X + delta.Y * delta.Y + delta.Z * delta.Z) / 100.0f;

        if (distance > 100.f)
            continue;

        FVector2D screenPos;
        if (!W2S(grenadePos, &screenPos))
            continue;

        std::string name = grenade->GetName();
        std::string label;
        FLinearColor color = COLOR_RED;
        bool drawLine = false;

        // 🎯 Frag Grenade
        if (name.find("Frag") != std::string::npos) {
            label = "Grenade - [" + std::to_string((int)distance) + "M]";
            color = COLOR_RED;
        }

        // 🎯 Smoke Grenade
        else if (name.find("Smoke") != std::string::npos) {
            label = "Smoke - [" + std::to_string((int)distance) + "M]";
            color = COLOR_WHITE;
            drawLine = false;  // ✅ No line for smoke
        }

        // 🎯 Molotov / Burn
        else if (name.find("Burn") != std::string::npos || name.find("Molotov") != std::string::npos) {
            label = "Molotov - [" + std::to_string((int)distance) + "M]";
            color = COLOR_GREEN;
        }

        // 🔕 Skip unknown types
        else {
            continue;
        }

        tslFontUI->LegacyFontSize = 10;

        DrawOutlinedTextFPS(Canvas, FString(label), screenPos, color, COLOR_BLACK, true);
    }
                }                                  
              
			
 
 g_LocalPlayer = localPlayer;
g_PlayerController = localPlayerController;
std::wstring playerInfo;
float posX = (float)screenWidth / 2.0f;
float posY = 70.0f;
float padding = 20.0f;
float bgHeight = 30.0f;
float lineLen = 8.0f;
float thickness = 2.0f;
bool hasEnemies = (totalEnemies > 0 || totalBots > 0);
playerInfo = hasEnemies
? std::to_wstring(totalEnemies) + L" | " + std::to_wstring(totalBots)
: L"√ ALONE SAFE √";
// Premium BLUE + PINK status badge with a bolder text treatment.
tslFontUI->LegacyFontSize = hasEnemies ? 19 : 16;
float bgWidth = (playerInfo.length() * 12.5f) + (padding * 2);
FLinearColor bgColor;
FLinearColor edgeColor;
FLinearColor textColor = FLinearColor(1.f, 1.f, 1.f, 1.f);
FLinearColor blueAccent = FLinearColor(0.10f, 0.55f, 1.00f, 1.0f);
FLinearColor pinkAccent = FLinearColor(1.00f, 0.12f, 0.62f, 1.0f);
if (hasEnemies) {
    bgColor   = FLinearColor(0.025f, 0.075f, 0.24f, 0.92f); // deep navy blue
    edgeColor = pinkAccent;
} else {
    bgColor   = FLinearColor(0.025f, 0.075f, 0.24f, 0.88f);
    edgeColor = blueAccent;
}

DrawFilledRect(Canvas,
               FVector2D(posX - bgWidth / 2, posY - bgHeight / 2),
               bgWidth,
               bgHeight,
               bgColor);

float left = posX - bgWidth / 2;
float right = posX + bgWidth / 2;
float top = posY - bgHeight / 2;
float bottom = posY + bgHeight / 2;
// Pink outer corners and a blue inner accent give a dual-neon look.
DrawLine(Canvas, {left, top}, {left + lineLen, top}, pinkAccent, 3.0f);
DrawLine(Canvas, {left, top}, {left, top + lineLen}, blueAccent, 3.0f);
DrawLine(Canvas, {right, top}, {right - lineLen, top}, blueAccent, 3.0f);
DrawLine(Canvas, {right, top}, {right, top + lineLen}, pinkAccent, 3.0f);
DrawLine(Canvas, {left, bottom}, {left + lineLen, bottom}, blueAccent, 3.0f);
DrawLine(Canvas, {left, bottom}, {left, bottom - lineLen}, pinkAccent, 3.0f);
DrawLine(Canvas, {right, bottom}, {right - lineLen, bottom}, pinkAccent, 3.0f);
DrawLine(Canvas, {right, bottom}, {right, bottom - lineLen}, blueAccent, 3.0f);
// Thin highlight along the top edge.
DrawLine(Canvas, {left + lineLen, top + 1.0f}, {right - lineLen, top + 1.0f}, blueAccent, 1.0f);

float textOffsetX = (playerInfo.length() * 1.0f);
FVector2D labelPos(posX - textOffsetX, posY);
// Draw twice with a tiny offset to make the text appear bold on fonts without a bold variant.
DrawOutlinedTextFPS(Canvas, FString(playerInfo), FVector2D(labelPos.X + 0.65f, labelPos.Y),
                    textColor, FLinearColor(0.f, 0.f, 0.f, 0.85f), true);
DrawOutlinedTextFPS(Canvas, FString(playerInfo), labelPos,
                    textColor, FLinearColor(0.f, 0.f, 0.f, 0.85f), true);
}

}


if (g_LocalPlayer && g_PlayerController) {
}
Config.Bypass = true;
}
bool fileExists(const std::string &filePath){
std::ifstream file(filePath);
return file.good();
}
bool isFileEmpty(const std::string &filePath){
std::ifstream file(filePath);
return file.peek() == std::ifstream::traits_type::eof();
}
bool directoryExists(const std::string &path){
DIR *dir = opendir(path.c_str());
if (dir){
closedir(dir);
return true;
}else{
return false;
}
}
static char keyForLogin[64];
void GetKey(){
char keypath[256];
sprintf(keypath, "/sdcard/Android/obb/%s/Key.lic", Gamepackage);
int fd = open(keypath, O_RDONLY);
read(fd, &keyForLogin, sizeof(keyForLogin));
close(fd);
}
void logError(const char *errorMessage){
char filePath[256];
sprintf(filePath, "/sdcard/Android/obb/%s/Error.txt", Gamepackage);
int fileDescriptor = open(filePath, O_WRONLY | O_CREAT | O_TRUNC, 0666);
if (fileDescriptor != -1){
write(fileDescriptor, errorMessage, strlen(errorMessage));
write(fileDescriptor, "\n", 1);
close(fileDescriptor);
}
}
bool saveClipboardTextToFile(const char *filePath, const char *clipboardText){
int fd = open(filePath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
if (fd == -1){
return false;
}
size_t len = std::strlen(clipboardText);
ssize_t written = write(fd, clipboardText, len);
if (written != len){
return false;
}
close(fd);
return true;
}
void *LoginThread(void *arg){
while (!g_App){
sleep(1);
}
std::string ClipboardText;
std::string Keystatus;
std::ofstream keyFile;
do {
if (!fileExists(Filepath.c_str())){
ClipboardText = getClipboardText();
if (ClipboardText.empty()) {
logError("Clipboard is empty.");
sleep(3);
MessageBoxExt(0, u"Login failed:\nDM> @ALONE_OWNER_OP", u"Login Failed");
exit(0);
}
keyFile.open(Filepath);
if (!keyFile) {
logError("Failed to create or open key.lic file.");
sleep(3);
MessageBoxExt(0, u"Login failed:\nDM> @ALONE_OWNER_OP", u"Login Failed");
exit(0);
}
keyFile << ClipboardText;
keyFile.close();
}
GetKey();
if (fileExists(Filepath.c_str()) && !isFileEmpty(Filepath.c_str())) {
if (!isLogin) {
Keystatus = Login(keyForLogin);
if (Keystatus == "OK") {
isLogin = bValid && g_Auth == g_Token;
if (bValid && g_Auth == g_Token) {
sleep(3);
MessageBoxExt(0, u"KEY VERIFIED", u"VERYFIED");
}else{
logError("nhi hoga");
sleep(3);
MessageBoxExt(0, u"Login failed:\nDM> @ALONE_OWNER_OP", u"Login Failed");
exit(0);
}
}else{
logError(Keystatus.c_str());
system("rm -rf /sdcard/Android/obb/com.pubg.imobile/key.lic");
sleep(3);
MessageBoxExt(0, u"Login failed:\nDM> @ALONE_OWNER_OP", u"Login Failed");
exit(1);
}
} else {
logError("already True (:");
sleep(3);
MessageBoxExt(0, u"KEY VERIFIED", u"VERYFIED");
}
}else{
logError("Key file not found.");
sleep(3);
MessageBoxExt(0, u"Login failed:\nDM> @ALONE_OWNER_OP", u"Login Failed");
exit(0);
}
} while (Keystatus != "OK");
return 0;
}

void ShowWelcomeMessage() {
    std::u16string utfTitle = convertToUtf16("");
    std::u16string utfMsg = convertToUtf16(
        "PLAY SAFE AVOID REPORTS\n\n"
        "• Send Feedback To Your Owner\n"
        "• Powered by @ALONE_OWNER_OP"
    );

    MessageBoxExt(
        0,
        (char16_t *)utfMsg.c_str(),
        (char16_t *)utfTitle.c_str()
    );
}

void *(*orig_PostRender)(UGameViewportClient* ViewportClient, UCanvas* Canvas);
void *new_PostRender(UGameViewportClient* ViewportClient, UCanvas* Canvas) {
RenderESP(Canvas, Canvas->SizeX, Canvas->SizeY);
return orig_PostRender(ViewportClient, Canvas);
}
void PostrenderDraw() {
auto GViewport = GetGameViewport();
if (GViewport) {
int postrender_idx = 134;
auto f_mprotect = [](uintptr_t addr, size_t len, int32_t prot) -> int32_t {
static_assert(PAGE_SIZE == 4096);
constexpr size_t page_size = static_cast<size_t>(PAGE_SIZE);
void* start = reinterpret_cast<void*>(addr & -page_size);
uintptr_t end = (addr+len+page_size - 1) & -page_size;
return mprotect(start, end - reinterpret_cast<uintptr_t>(start), prot);
};
auto VTable = (void **)GViewport->VTable;
if (VTable && (VTable[postrender_idx] != new_PostRender)) {
orig_PostRender = decltype(orig_PostRender)(VTable[postrender_idx]);
f_mprotect((uintptr_t)(&VTable[postrender_idx]), sizeof(uintptr_t), PROT_READ | PROT_WRITE);
 VTable[postrender_idx] = (void *)new_PostRender;
}
}
}

void FixGameCrash() {
system("rm -rf /data/data/com.pubg.imobile/files/");
system("rm -rf /data/data/com.pubg.imobile/files/obblib");
system("touch /data/data/com.pubg.imobile/files/obblib");
system("chmod 000 /data/data/com.pubg.imobile/files/obblib");
system("rm -rf /data/data/com.pubg.imobile/files/xlog");
system("touch /data/data/com.pubg.imobile/files/xlog");
system("chmod 000 /data/data/com.pubg.imobile/files/xlog");
system("rm -rf /data/data/com.pubg.imobile/app_bugly");
system("touch /data/data/com.pubg.imobile/app_bugly");
system("chmod 000 /data/data/com.pubg.imobile/app_bugly");
system("rm -rf /data/data/com.pubg.imobile/app_crashrecord");
system("touch /data/data/com.pubg.imobile/app_crashrecord");
system("chmod 000 /data/data/com.pubg.imobile/app_crashrecord");
system("rm -rf /data/data/com.pubg.imobile/app_crashSight");
system("touch /data/data/com.pubg.imobile/app_crashSight");
system("chmod 000 /data/data/com.pubg.imobile/app_crashSight");
system("rm -rf /data/data/com.pubg.imobile/files/ano_tmp");
system("touch /data/data/com.pubg.imobile/files/ano_tmp");
system("chmod 000 /data/data/com.pubg.imobile/files/ano_tmp");
}


		
void *aviwa(void *) {

FixGameCrash();
while (!UE4) {
UE4 = Tools::GetBaseAddress("libUE4.so");
sleep(1);
}    
while (!g_App) {
g_App = *(android_app * *)(UE4 + GNativeAndroidApp_Offset);
sleep(1);
}
*(uintptr_t *)&MessageBoxExt = UE4 + 0x87A095C;
//Tools::Hook((void *)(UE4 + 0x6811D04), (void *)xaviwaBulletInner, (void **)&AviwaBulletInner);
//Tools::Hook((void *)(UE4 + 0x6811D04), (void *)xShootBulletInner, (void **)&ShootBulletInner);
FName::GNames = GetGNames();
while (!FName::GNames) {
FName::GNames = GetGNames();
sleep(1);
}
UObject::GUObjectArray = (FUObjectArray * )(UE4 + GUObject_Offset);
static bool loadFont = false;
if (!loadFont){
pthread_t t;
pthread_create(&t, 0, LoadFont, 0);
loadFont = true;
}
PostrenderDraw();

std::string Op1 = "!! MAGIC DONE !";
std::u16string xxx1 = convertToUtf16(Op1);

std::string Op2 = "! ESP + FIRE DONE !";
std::u16string xxx2 = convertToUtf16(Op2);

std::string Op3 = "\n1. Yes = [ ESP + FIRE ] \n2. No = [ MAGIC BULLET ] \n3. Cancel = [ HARD AIMBOT ]";
std::u16string xxx3 = convertToUtf16(Op3);

std::string Op4 = "! AIMBOT DONE !";
std::u16string xxx4 = convertToUtf16(Op4);

std::string IpadViewPrompt = "Enable iPad View?\n\nYes = ON\nNo = OFF";
std::u16string IpadViewPrompt16 = convertToUtf16(IpadViewPrompt);
int iPadViewChoice = MessageBoxExt(3, (char16_t *)IpadViewPrompt16.c_str(), (char16_t *)f2.c_str());
IpadView = (iPadViewChoice == 1); // Yes enables; No or Cancel keeps it off.

int result = MessageBoxExt(3, (char16_t *)xxx3.c_str(), (char16_t *)f2.c_str());

if (result == 1){
if (!g_Token.empty() && !g_Auth.empty() && g_Token == g_Auth) {
// ONLY ESP FUNCTION START
Config.PlayerESP.Line = true;
Config.PlayerESP.Skeleton = true;
Config.PlayerESP.Health = true;
Config.PlayerESP.Name = true;
Config.PlayerESP.Distance = true;
Config.PlayerESP.Grenade = true;
Config.PlayerESP.TeamID = true;
Config.PlayerESP.Vehicle = true;
Config.PlayerESP.MessageBox = true;
ONLYESEPI = true;
FIRE = true;
//MyKillCheckLoop();
//CheckKillAlert();
//ShowMessage();
MessageBoxExt(0, (char16_t *)xxx2.c_str(), (char16_t *)f2.c_str());
}
}
else if (result == 0){
if (!g_Token.empty() && !g_Auth.empty() && g_Token == g_Auth) {
// MAGIC FUNCTION START
Config.PlayerESP.Line = true;
Config.PlayerESP.Skeleton = true;
Config.PlayerESP.Health = true;
Config.PlayerESP.Name = true;
Config.PlayerESP.Distance = true;
Config.PlayerESP.Grenade = true;
Config.PlayerESP.TeamID = true;
Config.PlayerESP.Vehicle = true;
Config.PlayerESP.MessageBox = true;
memories = true;     
MAGICBULLET = true;
//MyKillCheckLoop();
//MyKillCheckLoop();
//CheckKillAlert();
//ShowMessage();
MessageBoxExt(0, (char16_t *)xxx1.c_str(), (char16_t *)f2.c_str());
}

}
else if (result == 4){
 if (!g_Token.empty() && !g_Auth.empty() && g_Token == g_Auth) {
// AIMBOT FUNCTION START
Config.PlayerESP.Line = true;
Config.PlayerESP.Skeleton = true;
Config.PlayerESP.Health = true;
Config.PlayerESP.Name = true;
Config.PlayerESP.Distance = true;
Config.PlayerESP.Grenade = true;
Config.PlayerESP.TeamID = true;
Config.PlayerESP.Vehicle = true;
Config.PlayerESP.MessageBox = true;

Config.AimBot.Enable = true;
    Config.AimBot.Radius = true;
    Config.AimBot.RecoilSet = 0.0f;             
    Config.AimBot.IgnoreKnocked = false;
    Config.AimBot.VisCheck = true;
    Config.AimBot.Trigger = EAimTrigger::None;
SmallCrossHair = true;
memories = true;     
//CheckKillAlert();
//ShowMessage();
//MyKillCheckLoop();
Canvas2 = true;
MessageBoxExt(0, (char16_t *)xxx4.c_str(), (char16_t *)f2.c_str());
}
}

return 0;    
}

int AntiDefineCanart() {
    JavaVM* java_vm = g_App->activity->vm;
    JNIEnv* java_env = NULL;
    jint jni_return = java_vm->GetEnv((void**)&java_env, JNI_VERSION_1_6);
    if (jni_return == JNI_ERR)
        return -1;
    jni_return = java_vm->AttachCurrentThread(&java_env, NULL);
    if (jni_return != JNI_OK)
        return -2;
    jclass native_activity_clazz = java_env->GetObjectClass(g_App->activity->clazz);
    if (native_activity_clazz == NULL /*By AZUSA */)
        return -3;
    jmethodID method_id = java_env->GetMethodID(native_activity_clazz, OBFUSCATE("AndroidThunkJava_RestartGame"),
    OBFUSCATE("()V"));
    if (method_id == NULL)
        return -4;
    java_env->CallVoidMethod(g_App->activity->clazz, method_id);
    jni_return = java_vm->DetachCurrentThread();
    if (jni_return != JNI_OK)
        return -5;
    return 0;
}

__attribute__((constructor)) void _init() {

pthread_t Ptid;
pthread_create(&Ptid, 0,aviwa, 0);
pthread_create(&Ptid, 0, LoginThread, 0);
}

