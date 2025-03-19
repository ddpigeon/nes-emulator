#include <cstdint>
#include <string>
#include <chrono>
#include <list>
#include <vector>
#include <map>
#include <memory>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <functional>

class PixelGameEngine;
class Sprite;

// Pixel Game Engine Advanced Configuration
constexpr uint8_t nMouseButtons = 5;
constexpr uint8_t nDefaultAlpha = 0xFF;
constexpr uint32_t nDefaultPixel = (nDefaultAlpha << 24);
constexpr uint8_t nTabSizeInSpaces = 4;
constexpr size_t OLC_MAX_VERTS = 128;
enum rcode
{
    FAIL = 0,
    OK = 1,
    NO_FILE = -1
};

// O------------------------------------------------------------------------------O
// | Pixel - Represents a 32-Bit RGBA colour                                 |
// O------------------------------------------------------------------------------O
struct Pixel
{
    union
    {
        uint32_t n = nDefaultPixel;
        struct
        {
            uint8_t r;
            uint8_t g;
            uint8_t b;
            uint8_t a;
        };
    };

    enum Mode
    {
        NORMAL,
        MASK,
        ALPHA,
        CUSTOM
    };

    Pixel();
    Pixel(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = nDefaultAlpha);
    Pixel(uint32_t p);
    Pixel &operator=(const Pixel &v) = default;
    bool operator==(const Pixel &p) const;
    bool operator!=(const Pixel &p) const;
    Pixel operator*(const float i) const;
    Pixel operator/(const float i) const;
    Pixel &operator*=(const float i);
    Pixel &operator/=(const float i);
    Pixel operator+(const Pixel &p) const;
    Pixel operator-(const Pixel &p) const;
    Pixel &operator+=(const Pixel &p);
    Pixel &operator-=(const Pixel &p);
    Pixel operator*(const Pixel &p) const;
    Pixel &operator*=(const Pixel &p);
    Pixel inv() const;
};

Pixel PixelF(float red, float green, float blue, float alpha = 1.0f);
Pixel PixelLerp(const Pixel &p1, const Pixel &p2, float t);

// O------------------------------------------------------------------------------O
// | USEFUL CONSTANTS                                                             |
// O------------------------------------------------------------------------------O
static const Pixel
    GREY(192, 192, 192),
    DARK_GREY(128, 128, 128), VERY_DARK_GREY(64, 64, 64),
    RED(255, 0, 0), DARK_RED(128, 0, 0), VERY_DARK_RED(64, 0, 0),
    YELLOW(255, 255, 0), DARK_YELLOW(128, 128, 0), VERY_DARK_YELLOW(64, 64, 0),
    GREEN(0, 255, 0), DARK_GREEN(0, 128, 0), VERY_DARK_GREEN(0, 64, 0),
    CYAN(0, 255, 255), DARK_CYAN(0, 128, 128), VERY_DARK_CYAN(0, 64, 64),
    BLUE(0, 0, 255), DARK_BLUE(0, 0, 128), VERY_DARK_BLUE(0, 0, 64),
    MAGENTA(255, 0, 255), DARK_MAGENTA(128, 0, 128), VERY_DARK_MAGENTA(64, 0, 64),
    WHITE(255, 255, 255), BLACK(0, 0, 0), BLANK(0, 0, 0, 0);

// Thanks to scripticuk and others for updating the key maps
// NOTE: The GLUT platform will need updating, open to contributions ;)
enum Key
{
    NONE,
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,
    K0,
    K1,
    K2,
    K3,
    K4,
    K5,
    K6,
    K7,
    K8,
    K9,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    UP,
    DOWN,
    LEFT,
    RIGHT,
    SPACE,
    TAB,
    SHIFT,
    CTRL,
    INS,
    DEL,
    HOME,
    END,
    PGUP,
    PGDN,
    BACK,
    ESCAPE,
    RETURN,
    ENTER,
    PAUSE,
    SCROLL,
    NP0,
    NP1,
    NP2,
    NP3,
    NP4,
    NP5,
    NP6,
    NP7,
    NP8,
    NP9,
    NP_MUL,
    NP_DIV,
    NP_ADD,
    NP_SUB,
    NP_DECIMAL,
    PERIOD,
    EQUALS,
    COMMA,
    MINUS,
    OEM_1,
    OEM_2,
    OEM_3,
    OEM_4,
    OEM_5,
    OEM_6,
    OEM_7,
    OEM_8,
    CAPS_LOCK,
    ENUM_END
};

namespace Mouse
{
    static constexpr int32_t LEFT = 0;
    static constexpr int32_t RIGHT = 1;
    static constexpr int32_t MIDDLE = 2;
};

// O------------------------------------------------------------------------------O
// | HWButton - Represents the state of a hardware button (mouse/key/joy)    |
// O------------------------------------------------------------------------------O
struct HWButton
{
    bool bPressed = false;  // Set once during the frame the event occurs
    bool bReleased = false; // Set once during the frame the event occurs
    bool bHeld = false;     // Set true for all frames between pressed and released events
};

// O------------------------------------------------------------------------------O
// | vX2d - A generic 2D vector type                                         |
// O------------------------------------------------------------------------------O
#if !defined(OLC_IGNORE_VEC2D)
template <class T>
struct v2d_generic
{
    T x = 0;
    T y = 0;
    v2d_generic() : x(0), y(0) {}
    v2d_generic(T _x, T _y) : x(_x), y(_y) {}
    v2d_generic(const v2d_generic &v) : x(v.x), y(v.y) {}
    v2d_generic &operator=(const v2d_generic &v) = default;
    T mag() const { return T(std::sqrt(x * x + y * y)); }
    T mag2() const { return x * x + y * y; }
    v2d_generic norm() const
    {
        T r = 1 / mag();
        return v2d_generic(x * r, y * r);
    }
    v2d_generic perp() const { return v2d_generic(-y, x); }
    v2d_generic floor() const { return v2d_generic(std::floor(x), std::floor(y)); }
    v2d_generic ceil() const { return v2d_generic(std::ceil(x), std::ceil(y)); }
    v2d_generic max(const v2d_generic &v) const { return v2d_generic(std::max(x, v.x), std::max(y, v.y)); }
    v2d_generic min(const v2d_generic &v) const { return v2d_generic(std::min(x, v.x), std::min(y, v.y)); }
    v2d_generic cart() { return {std::cos(y) * x, std::sin(y) * x}; }
    v2d_generic polar() { return {mag(), std::atan2(y, x)}; }
    v2d_generic clamp(const v2d_generic &v1, const v2d_generic &v2) const { return this->max(v1).min(v2); }
    v2d_generic lerp(const v2d_generic &v1, const double t) { return this->operator*(T(1.0 - t)) + (v1 * T(t)); }
    T dot(const v2d_generic &rhs) const { return this->x * rhs.x + this->y * rhs.y; }
    T cross(const v2d_generic &rhs) const { return this->x * rhs.y - this->y * rhs.x; }
    v2d_generic operator+(const v2d_generic &rhs) const { return v2d_generic(this->x + rhs.x, this->y + rhs.y); }
    v2d_generic operator-(const v2d_generic &rhs) const { return v2d_generic(this->x - rhs.x, this->y - rhs.y); }
    v2d_generic operator*(const T &rhs) const { return v2d_generic(this->x * rhs, this->y * rhs); }
    v2d_generic operator*(const v2d_generic &rhs) const { return v2d_generic(this->x * rhs.x, this->y * rhs.y); }
    v2d_generic operator/(const T &rhs) const { return v2d_generic(this->x / rhs, this->y / rhs); }
    v2d_generic operator/(const v2d_generic &rhs) const { return v2d_generic(this->x / rhs.x, this->y / rhs.y); }
    v2d_generic &operator+=(const v2d_generic &rhs)
    {
        this->x += rhs.x;
        this->y += rhs.y;
        return *this;
    }
    v2d_generic &operator-=(const v2d_generic &rhs)
    {
        this->x -= rhs.x;
        this->y -= rhs.y;
        return *this;
    }
    v2d_generic &operator*=(const T &rhs)
    {
        this->x *= rhs;
        this->y *= rhs;
        return *this;
    }
    v2d_generic &operator/=(const T &rhs)
    {
        this->x /= rhs;
        this->y /= rhs;
        return *this;
    }
    v2d_generic &operator*=(const v2d_generic &rhs)
    {
        this->x *= rhs.x;
        this->y *= rhs.y;
        return *this;
    }
    v2d_generic &operator/=(const v2d_generic &rhs)
    {
        this->x /= rhs.x;
        this->y /= rhs.y;
        return *this;
    }
    v2d_generic operator+() const { return {+x, +y}; }
    v2d_generic operator-() const { return {-x, -y}; }
    bool operator==(const v2d_generic &rhs) const { return (this->x == rhs.x && this->y == rhs.y); }
    bool operator!=(const v2d_generic &rhs) const { return (this->x != rhs.x || this->y != rhs.y); }
    const std::string str() const { return std::string("(") + std::to_string(this->x) + "," + std::to_string(this->y) + ")"; }
    friend std::ostream &operator<<(std::ostream &os, const v2d_generic &rhs)
    {
        os << rhs.str();
        return os;
    }
    operator v2d_generic<int32_t>() const { return {static_cast<int32_t>(this->x), static_cast<int32_t>(this->y)}; }
    operator v2d_generic<float>() const { return {static_cast<float>(this->x), static_cast<float>(this->y)}; }
    operator v2d_generic<double>() const { return {static_cast<double>(this->x), static_cast<double>(this->y)}; }
};

// Note: joshinils has some good suggestions here, but they are complicated to implement at this moment,
// however they will appear in a future version of PGE
template <class T>
inline v2d_generic<T> operator*(const float &lhs, const v2d_generic<T> &rhs)
{
    return v2d_generic<T>((T)(lhs * (float)rhs.x), (T)(lhs * (float)rhs.y));
}
template <class T>
inline v2d_generic<T> operator*(const double &lhs, const v2d_generic<T> &rhs)
{
    return v2d_generic<T>((T)(lhs * (double)rhs.x), (T)(lhs * (double)rhs.y));
}
template <class T>
inline v2d_generic<T> operator*(const int &lhs, const v2d_generic<T> &rhs)
{
    return v2d_generic<T>((T)(lhs * (int)rhs.x), (T)(lhs * (int)rhs.y));
}
template <class T>
inline v2d_generic<T> operator/(const float &lhs, const v2d_generic<T> &rhs)
{
    return v2d_generic<T>((T)(lhs / (float)rhs.x), (T)(lhs / (float)rhs.y));
}
template <class T>
inline v2d_generic<T> operator/(const double &lhs, const v2d_generic<T> &rhs)
{
    return v2d_generic<T>((T)(lhs / (double)rhs.x), (T)(lhs / (double)rhs.y));
}
template <class T>
inline v2d_generic<T> operator/(const int &lhs, const v2d_generic<T> &rhs)
{
    return v2d_generic<T>((T)(lhs / (int)rhs.x), (T)(lhs / (int)rhs.y));
}

// To stop dandistine crying...
template <class T, class U>
inline bool operator<(const v2d_generic<T> &lhs, const v2d_generic<U> &rhs)
{
    return lhs.y < rhs.y || (lhs.y == rhs.y && lhs.x < rhs.x);
}
template <class T, class U>
inline bool operator>(const v2d_generic<T> &lhs, const v2d_generic<U> &rhs)
{
    return lhs.y > rhs.y || (lhs.y == rhs.y && lhs.x > rhs.x);
}

typedef v2d_generic<int32_t> vi2d;
typedef v2d_generic<uint32_t> vu2d;
typedef v2d_generic<float> vf2d;
typedef v2d_generic<double> vd2d;
#endif

// O------------------------------------------------------------------------------O
// | ResourcePack - A virtual scrambled filesystem to pack your assets into  |
// O------------------------------------------------------------------------------O
struct ResourceBuffer : public std::streambuf
{
    ResourceBuffer(std::ifstream &ifs, uint32_t offset, uint32_t size);
    std::vector<char> vMemory;
};

class ResourcePack : public std::streambuf
{
public:
    ResourcePack();
    ~ResourcePack();
    bool AddFile(const std::string &sFile);
    bool LoadPack(const std::string &sFile, const std::string &sKey);
    bool SavePack(const std::string &sFile, const std::string &sKey);
    ResourceBuffer GetFileBuffer(const std::string &sFile);
    bool Loaded();

private:
    struct sResourceFile
    {
        uint32_t nSize;
        uint32_t nOffset;
    };
    std::map<std::string, sResourceFile> mapFiles;
    std::ifstream baseFile;
    std::vector<char> scramble(const std::vector<char> &data, const std::string &key);
    std::string makeposix(const std::string &path);
};

class ImageLoader
{
public:
    ImageLoader() = default;
    virtual ~ImageLoader() = default;
    virtual rcode LoadImageResource(Sprite *spr, const std::string &sImageFile, ResourcePack *pack) = 0;
    virtual rcode SaveImageResource(Sprite *spr, const std::string &sImageFile) = 0;
};

// O------------------------------------------------------------------------------O
// | Sprite - An image represented by a 2D array of Pixel               |
// O------------------------------------------------------------------------------O
class Sprite
{
public:
    Sprite();
    Sprite(const std::string &sImageFile, ResourcePack *pack = nullptr);
    Sprite(int32_t w, int32_t h);
    Sprite(const Sprite &) = delete;
    ~Sprite();

public:
    rcode LoadFromFile(const std::string &sImageFile, ResourcePack *pack = nullptr);

public:
    int32_t width = 0;
    int32_t height = 0;
    enum Mode
    {
        NORMAL,
        PERIODIC,
        CLAMP
    };
    enum Flip
    {
        NONE = 0,
        HORIZ = 1,
        VERT = 2
    };

public:
    void SetSampleMode(Sprite::Mode mode = Sprite::Mode::NORMAL);
    Pixel GetPixel(int32_t x, int32_t y) const;
    bool SetPixel(int32_t x, int32_t y, Pixel p);
    Pixel GetPixel(const vi2d &a) const;
    bool SetPixel(const vi2d &a, Pixel p);
    Pixel Sample(float x, float y) const;
    Pixel Sample(const vf2d &uv) const;
    Pixel SampleBL(float u, float v) const;
    Pixel SampleBL(const vf2d &uv) const;
    Pixel *GetData();
    Sprite *Duplicate();
    Sprite *Duplicate(const vi2d &vPos, const vi2d &vSize);
    vi2d Size() const;
    std::vector<Pixel> pColData;
    Mode modeSample = Mode::NORMAL;

    static std::unique_ptr<ImageLoader> loader;
};

// O------------------------------------------------------------------------------O
// | Decal - A GPU resident storage of an Sprite                        |
// O------------------------------------------------------------------------------O
class Decal
{
public:
    Decal(Sprite *spr, bool filter = false, bool clamp = true);
    Decal(const uint32_t nExistingTextureResource, Sprite *spr);
    virtual ~Decal();
    void Update();
    void UpdateSprite();

public: // But dont touch
    int32_t id = -1;
    Sprite *sprite = nullptr;
    vf2d vUVScale = {1.0f, 1.0f};
};

enum class DecalMode
{
    NORMAL,
    ADDITIVE,
    MULTIPLICATIVE,
    STENCIL,
    ILLUMINATE,
    WIREFRAME,
    MODEL3D,
};

enum class DecalStructure
{
    LINE,
    FAN,
    STRIP,
    LIST
};

// O------------------------------------------------------------------------------O
// | Renderable - Convenience class to keep a sprite and decal together      |
// O------------------------------------------------------------------------------O
class Renderable
{
public:
    Renderable() = default;
    Renderable(Renderable &&r) : pSprite(std::move(r.pSprite)), pDecal(std::move(r.pDecal)) {}
    Renderable(const Renderable &) = delete;
    rcode Load(const std::string &sFile, ResourcePack *pack = nullptr, bool filter = false, bool clamp = true);
    void Create(uint32_t width, uint32_t height, bool filter = false, bool clamp = true);
    Decal *Decal() const;
    Sprite *Sprite() const;

private:
    std::unique_ptr<::Sprite> pSprite = nullptr;
    std::unique_ptr<::Decal> pDecal = nullptr;
};

// O------------------------------------------------------------------------------O
// | Auxilliary components internal to engine                                     |
// O------------------------------------------------------------------------------O

struct DecalInstance
{
    Decal *decal = nullptr;
    std::vector<vf2d> pos;
    std::vector<vf2d> uv;
    std::vector<float> w;
    std::vector<Pixel> tint;
    DecalMode mode = DecalMode::NORMAL;
    DecalStructure structure = DecalStructure::FAN;
    uint32_t points = 0;
};

struct LayerDesc
{
    vf2d vOffset = {0, 0};
    vf2d vScale = {1, 1};
    bool bShow = false;
    bool bUpdate = false;
    Renderable pDrawTarget;
    uint32_t nResID = 0;
    std::vector<DecalInstance> vecDecalInstance;
    Pixel tint = WHITE;
    std::function<void()> funcHook = nullptr;
};

class Renderer
{
public:
    virtual ~Renderer() = default;
    virtual void PrepareDevice() = 0;
    virtual rcode CreateDevice(std::vector<void *> params, bool bFullScreen, bool bVSYNC) = 0;
    virtual rcode DestroyDevice() = 0;
    virtual void DisplayFrame() = 0;
    virtual void PrepareDrawing() = 0;
    virtual void SetDecalMode(const DecalMode &mode) = 0;
    virtual void DrawLayerQuad(const vf2d &offset, const vf2d &scale, const Pixel tint) = 0;
    virtual void DrawDecal(const DecalInstance &decal) = 0;
    virtual uint32_t CreateTexture(const uint32_t width, const uint32_t height, const bool filtered = false, const bool clamp = true) = 0;
    virtual void UpdateTexture(uint32_t id, Sprite *spr) = 0;
    virtual void ReadTexture(uint32_t id, Sprite *spr) = 0;
    virtual uint32_t DeleteTexture(const uint32_t id) = 0;
    virtual void ApplyTexture(uint32_t id) = 0;
    virtual void UpdateViewport(const vi2d &pos, const vi2d &size) = 0;
    virtual void ClearBuffer(Pixel p, bool bDepth) = 0;
    static PixelGameEngine *ptrPGE;
};

class Platform
{
public:
    virtual ~Platform() = default;
    virtual rcode ApplicationStartUp() = 0;
    virtual rcode ApplicationCleanUp() = 0;
    virtual rcode ThreadStartUp() = 0;
    virtual rcode ThreadCleanUp() = 0;
    virtual rcode CreateGraphics(bool bFullScreen, bool bEnableVSYNC, const vi2d &vViewPos, const vi2d &vViewSize) = 0;
    virtual rcode CreateWindowPane(const vi2d &vWindowPos, vi2d &vWindowSize, bool bFullScreen) = 0;
    virtual rcode SetWindowTitle(const std::string &s) = 0;
    virtual rcode StartSystemEventLoop() = 0;
    virtual rcode HandleSystemEvent() = 0;
    static PixelGameEngine *ptrPGE;
};

class PGEX;

// The Static Twins (plus one)
static std::unique_ptr<Renderer> renderer;
static std::unique_ptr<Platform> platform;
static std::map<size_t, uint8_t> mapKeys;

// O------------------------------------------------------------------------------O
// | PixelGameEngine - The main BASE class for your application              |
// O------------------------------------------------------------------------------O
class PixelGameEngine
{
public:
    PixelGameEngine();
    virtual ~PixelGameEngine();

public:
    rcode Construct(int32_t screen_w, int32_t screen_h, int32_t pixel_w, int32_t pixel_h,
                         bool full_screen = false, bool vsync = false, bool cohesion = false);
    rcode Start();

public: // User Override Interfaces
    // Called once on application startup, use to load your resources
    virtual bool OnUserCreate();
    // Called every frame, and provides you with a time per frame value
    virtual bool OnUserUpdate(float fElapsedTime);
    // Called once on application termination, so you can be one clean coder
    virtual bool OnUserDestroy();

    // Called when a text entry is confirmed with "enter" key
    virtual void OnTextEntryComplete(const std::string &sText);
    // Called when a console command is executed
    virtual bool OnConsoleCommand(const std::string &sCommand);

public: // Hardware Interfaces
    // Returns true if window is currently in focus
    bool IsFocused() const;
    // Get the state of a specific keyboard button
    HWButton GetKey(Key k) const;
    // Get the state of a specific mouse button
    HWButton GetMouse(uint32_t b) const;
    // Get Mouse X coordinate in "pixel" space
    int32_t GetMouseX() const;
    // Get Mouse Y coordinate in "pixel" space
    int32_t GetMouseY() const;
    // Get Mouse Wheel Delta
    int32_t GetMouseWheel() const;
    // Get the mouse in window space
    const vi2d &GetWindowMouse() const;
    // Gets the mouse as a vector to keep Tarriest happy
    const vi2d &GetMousePos() const;

    static const std::map<size_t, uint8_t> &GetKeyMap() { return mapKeys; }

public: // Utility
    // Returns the width of the screen in "pixels"
    int32_t ScreenWidth() const;
    // Returns the height of the screen in "pixels"
    int32_t ScreenHeight() const;
    // Returns the width of the currently selected drawing target in "pixels"
    int32_t GetDrawTargetWidth() const;
    // Returns the height of the currently selected drawing target in "pixels"
    int32_t GetDrawTargetHeight() const;
    // Returns the currently active draw target
    Sprite *GetDrawTarget() const;
    // Resize the primary screen sprite
    void SetScreenSize(int w, int h);
    // Specify which Sprite should be the target of drawing functions, use nullptr
    // to specify the primary screen
    void SetDrawTarget(Sprite *target);
    // Gets the current Frames Per Second
    uint32_t GetFPS() const;
    // Gets last update of elapsed time
    float GetElapsedTime() const;
    // Gets Actual Window size
    const vi2d &GetWindowSize() const;
    // Gets pixel scale
    const vi2d &GetPixelSize() const;
    // Gets actual pixel scale
    const vi2d &GetScreenPixelSize() const;
    // Gets "screen" size
    const vi2d &GetScreenSize() const;
    // Gets any files dropped this frame
    const std::vector<std::string> &GetDroppedFiles() const;
    const vi2d &GetDroppedFilesPoint() const;

public: // CONFIGURATION ROUTINES
    // Layer targeting functions
    void SetDrawTarget(uint8_t layer, bool bDirty = true);
    void EnableLayer(uint8_t layer, bool b);
    void SetLayerOffset(uint8_t layer, const vf2d &offset);
    void SetLayerOffset(uint8_t layer, float x, float y);
    void SetLayerScale(uint8_t layer, const vf2d &scale);
    void SetLayerScale(uint8_t layer, float x, float y);
    void SetLayerTint(uint8_t layer, const Pixel &tint);
    void SetLayerCustomRenderFunction(uint8_t layer, std::function<void()> f);

    std::vector<LayerDesc> &GetLayers();
    uint32_t CreateLayer();

    // Change the pixel mode for different optimisations
    // Pixel::NORMAL = No transparency
    // Pixel::MASK   = Transparent if alpha is < 255
    // Pixel::ALPHA  = Full transparency
    void SetPixelMode(Pixel::Mode m);
    Pixel::Mode GetPixelMode();
    // Use a custom blend function
    void SetPixelMode(std::function<Pixel(const int x, const int y, const Pixel &pSource, const Pixel &pDest)> pixelMode);
    // Change the blend factor from between 0.0f to 1.0f;
    void SetPixelBlend(float fBlend);

public: // DRAWING ROUTINES
    // Draws a single Pixel
    virtual bool Draw(int32_t x, int32_t y, Pixel p = WHITE);
    bool Draw(const vi2d &pos, Pixel p = WHITE);
    // Draws a line from (x1,y1) to (x2,y2)
    void DrawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, Pixel p = WHITE, uint32_t pattern = 0xFFFFFFFF);
    void DrawLine(const vi2d &pos1, const vi2d &pos2, Pixel p = WHITE, uint32_t pattern = 0xFFFFFFFF);
    // Draws a circle located at (x,y) with radius
    void DrawCircle(int32_t x, int32_t y, int32_t radius, Pixel p = WHITE, uint8_t mask = 0xFF);
    void DrawCircle(const vi2d &pos, int32_t radius, Pixel p = WHITE, uint8_t mask = 0xFF);
    // Fills a circle located at (x,y) with radius
    void FillCircle(int32_t x, int32_t y, int32_t radius, Pixel p = WHITE);
    void FillCircle(const vi2d &pos, int32_t radius, Pixel p = WHITE);
    // Draws a rectangle at (x,y) to (x+w,y+h)
    void DrawRect(int32_t x, int32_t y, int32_t w, int32_t h, Pixel p = WHITE);
    void DrawRect(const vi2d &pos, const vi2d &size, Pixel p = WHITE);
    // Fills a rectangle at (x,y) to (x+w,y+h)
    void FillRect(int32_t x, int32_t y, int32_t w, int32_t h, Pixel p = WHITE);
    void FillRect(const vi2d &pos, const vi2d &size, Pixel p = WHITE);
    // Draws a triangle between points (x1,y1), (x2,y2) and (x3,y3)
    void DrawTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, Pixel p = WHITE);
    void DrawTriangle(const vi2d &pos1, const vi2d &pos2, const vi2d &pos3, Pixel p = WHITE);
    // Flat fills a triangle between points (x1,y1), (x2,y2) and (x3,y3)
    void FillTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, Pixel p = WHITE);
    void FillTriangle(const vi2d &pos1, const vi2d &pos2, const vi2d &pos3, Pixel p = WHITE);
    // Fill a textured and coloured triangle
    void FillTexturedTriangle(const std::vector<vf2d> &vPoints, std::vector<vf2d> vTex, std::vector<Pixel> vColour, Sprite *sprTex);
    void FillTexturedPolygon(const std::vector<vf2d> &vPoints, const std::vector<vf2d> &vTex, const std::vector<Pixel> &vColour, Sprite *sprTex, DecalStructure structure = DecalStructure::LIST);
    // Draws an entire sprite at location (x,y)
    void DrawSprite(int32_t x, int32_t y, Sprite *sprite, uint32_t scale = 1, uint8_t flip = Sprite::NONE);
    void DrawSprite(const vi2d &pos, Sprite *sprite, uint32_t scale = 1, uint8_t flip = Sprite::NONE);
    // Draws an area of a sprite at location (x,y), where the
    // selected area is (ox,oy) to (ox+w,oy+h)
    void DrawPartialSprite(int32_t x, int32_t y, Sprite *sprite, int32_t ox, int32_t oy, int32_t w, int32_t h, uint32_t scale = 1, uint8_t flip = Sprite::NONE);
    void DrawPartialSprite(const vi2d &pos, Sprite *sprite, const vi2d &sourcepos, const vi2d &size, uint32_t scale = 1, uint8_t flip = Sprite::NONE);
    // Draws a single line of text - traditional monospaced
    void DrawString(int32_t x, int32_t y, const std::string &sText, Pixel col = WHITE, uint32_t scale = 1);
    void DrawString(const vi2d &pos, const std::string &sText, Pixel col = WHITE, uint32_t scale = 1);
    vi2d GetTextSize(const std::string &s);
    // Draws a single line of text - non-monospaced
    void DrawStringProp(int32_t x, int32_t y, const std::string &sText, Pixel col = WHITE, uint32_t scale = 1);
    void DrawStringProp(const vi2d &pos, const std::string &sText, Pixel col = WHITE, uint32_t scale = 1);
    vi2d GetTextSizeProp(const std::string &s);

    // Decal Quad functions
    void SetDecalMode(const DecalMode &mode);
    void SetDecalStructure(const DecalStructure &structure);
    // Draws a whole decal, with optional scale and tinting
    void DrawDecal(const vf2d &pos, Decal *decal, const vf2d &scale = {1.0f, 1.0f}, const Pixel &tint = WHITE);
    // Draws a region of a decal, with optional scale and tinting
    void DrawPartialDecal(const vf2d &pos, Decal *decal, const vf2d &source_pos, const vf2d &source_size, const vf2d &scale = {1.0f, 1.0f}, const Pixel &tint = WHITE);
    void DrawPartialDecal(const vf2d &pos, const vf2d &size, Decal *decal, const vf2d &source_pos, const vf2d &source_size, const Pixel &tint = WHITE);
    // Draws fully user controlled 4 vertices, pos(pixels), uv(pixels), colours
    void DrawExplicitDecal(Decal *decal, const vf2d *pos, const vf2d *uv, const Pixel *col, uint32_t elements = 4);
    // Draws a decal with 4 arbitrary points, warping the texture to look "correct"
    void DrawWarpedDecal(Decal *decal, const vf2d (&pos)[4], const Pixel &tint = WHITE);
    void DrawWarpedDecal(Decal *decal, const vf2d *pos, const Pixel &tint = WHITE);
    void DrawWarpedDecal(Decal *decal, const std::array<vf2d, 4> &pos, const Pixel &tint = WHITE);
    // As above, but you can specify a region of a decal source sprite
    void DrawPartialWarpedDecal(Decal *decal, const vf2d (&pos)[4], const vf2d &source_pos, const vf2d &source_size, const Pixel &tint = WHITE);
    void DrawPartialWarpedDecal(Decal *decal, const vf2d *pos, const vf2d &source_pos, const vf2d &source_size, const Pixel &tint = WHITE);
    void DrawPartialWarpedDecal(Decal *decal, const std::array<vf2d, 4> &pos, const vf2d &source_pos, const vf2d &source_size, const Pixel &tint = WHITE);
    // Draws a decal rotated to specified angle, wit point of rotation offset
    void DrawRotatedDecal(const vf2d &pos, Decal *decal, const float fAngle, const vf2d &center = {0.0f, 0.0f}, const vf2d &scale = {1.0f, 1.0f}, const Pixel &tint = WHITE);
    void DrawPartialRotatedDecal(const vf2d &pos, Decal *decal, const float fAngle, const vf2d &center, const vf2d &source_pos, const vf2d &source_size, const vf2d &scale = {1.0f, 1.0f}, const Pixel &tint = WHITE);
    // Draws a multiline string as a decal, with tiniting and scaling
    void DrawStringDecal(const vf2d &pos, const std::string &sText, const Pixel col = WHITE, const vf2d &scale = {1.0f, 1.0f});
    void DrawStringPropDecal(const vf2d &pos, const std::string &sText, const Pixel col = WHITE, const vf2d &scale = {1.0f, 1.0f});
    // Draws a single shaded filled rectangle as a decal
    void DrawRectDecal(const vf2d &pos, const vf2d &size, const Pixel col = WHITE);
    void FillRectDecal(const vf2d &pos, const vf2d &size, const Pixel col = WHITE);
    // Draws a corner shaded rectangle as a decal
    void GradientFillRectDecal(const vf2d &pos, const vf2d &size, const Pixel colTL, const Pixel colBL, const Pixel colBR, const Pixel colTR);
    // Draws an arbitrary convex textured polygon using GPU
    void DrawPolygonDecal(Decal *decal, const std::vector<vf2d> &pos, const std::vector<vf2d> &uv, const Pixel tint = WHITE);
    void DrawPolygonDecal(Decal *decal, const std::vector<vf2d> &pos, const std::vector<float> &depth, const std::vector<vf2d> &uv, const Pixel tint = WHITE);
    void DrawPolygonDecal(Decal *decal, const std::vector<vf2d> &pos, const std::vector<vf2d> &uv, const std::vector<Pixel> &tint);
    void DrawPolygonDecal(Decal *decal, const std::vector<vf2d> &pos, const std::vector<vf2d> &uv, const std::vector<Pixel> &colours, const Pixel tint);
    // Draws a line in Decal Space
    void DrawLineDecal(const vf2d &pos1, const vf2d &pos2, Pixel p = WHITE);
    void DrawRotatedStringDecal(const vf2d &pos, const std::string &sText, const float fAngle, const vf2d &center = {0.0f, 0.0f}, const Pixel col = WHITE, const vf2d &scale = {1.0f, 1.0f});
    void DrawRotatedStringPropDecal(const vf2d &pos, const std::string &sText, const float fAngle, const vf2d &center = {0.0f, 0.0f}, const Pixel col = WHITE, const vf2d &scale = {1.0f, 1.0f});
    // Clears entire draw target to Pixel
    void Clear(Pixel p);
    // Clears the rendering back buffer
    void ClearBuffer(Pixel p, bool bDepth = true);
    // Returns the font image
    Sprite *GetFontSprite();

    // Clip a line segment to visible area
    bool ClipLineToScreen(vi2d &in_p1, vi2d &in_p2);

    // Dont allow PGE to mark layers as dirty, so pixel graphics don't update
    void EnablePixelTransfer(const bool bEnable = true);

    // Command Console Routines
    void ConsoleShow(const Key &keyExit, bool bSuspendTime = true);
    bool IsConsoleShowing() const;
    void ConsoleClear();
    std::stringstream &ConsoleOut();
    void ConsoleCaptureStdOut(const bool bCapture);

    // Text Entry Routines
    void TextEntryEnable(const bool bEnable, const std::string &sText = "");
    std::string TextEntryGetString() const;
    int32_t TextEntryGetCursor() const;
    bool IsTextEntryEnabled() const;

private:
    void UpdateTextEntry();
    void UpdateConsole();

public:
    // Experimental Lightweight 3D Routines ================
#ifdef OLC_ENABLE_EXPERIMENTAL
    // Set Manual View Matrix
    void LW3D_View(const std::array<float, 16> &m);
    // Set Manual World Matrix
    void LW3D_World(const std::array<float, 16> &m);
    // Set Manual Projection Matrix
    void LW3D_Projection(const std::array<float, 16> &m);

    // Draws a vector of vertices, interprted as individual triangles
    void LW3D_DrawTriangles(Decal *decal, const std::vector<std::array<float, 3>> &pos, const std::vector<vf2d> &tex, const std::vector<Pixel> &col);

    void LW3D_ModelTranslate(const float x, const float y, const float z);

    // Camera convenience functions
    void LW3D_SetCameraAtTarget(const float fEyeX, const float fEyeY, const float fEyeZ,
                                const float fTargetX, const float fTargetY, const float fTargetZ,
                                const float fUpX = 0.0f, const float fUpY = 1.0f, const float fUpZ = 0.0f);
    void LW3D_SetCameraAlongDirection(const float fEyeX, const float fEyeY, const float fEyeZ,
                                      const float fDirX, const float fDirY, const float fDirZ,
                                      const float fUpX = 0.0f, const float fUpY = 1.0f, const float fUpZ = 0.0f);

    // 3D Rendering Flags
    void LW3D_EnableDepthTest(const bool bEnableDepth);
    void LW3D_EnableBackfaceCulling(const bool bEnableCull);
#endif
public: // Branding
    std::string sAppName;

private: // Inner mysterious workings
    Sprite *pDrawTarget = nullptr;
    Pixel::Mode nPixelMode = Pixel::NORMAL;
    float fBlendFactor = 1.0f;
    vi2d vScreenSize = {256, 240};
    vf2d vInvScreenSize = {1.0f / 256.0f, 1.0f / 240.0f};
    vi2d vPixelSize = {4, 4};
    vi2d vScreenPixelSize = {4, 4};
    vi2d vMousePos = {0, 0};
    int32_t nMouseWheelDelta = 0;
    vi2d vMousePosCache = {0, 0};
    vi2d vMouseWindowPos = {0, 0};
    int32_t nMouseWheelDeltaCache = 0;
    vi2d vWindowSize = {0, 0};
    vi2d vViewPos = {0, 0};
    vi2d vViewSize = {0, 0};
    bool bFullScreen = false;
    vf2d vPixel = {1.0f, 1.0f};
    bool bHasInputFocus = false;
    bool bHasMouseFocus = false;
    bool bEnableVSYNC = false;
    float fFrameTimer = 1.0f;
    float fLastElapsed = 0.0f;
    int nFrameCount = 0;
    bool bSuspendTextureTransfer = false;
    Renderable fontRenderable;
    std::vector<LayerDesc> vLayers;
    uint8_t nTargetLayer = 0;
    uint32_t nLastFPS = 0;
    bool bPixelCohesion = false;
    DecalMode nDecalMode = DecalMode::NORMAL;
    DecalStructure nDecalStructure = DecalStructure::FAN;
    std::function<Pixel(const int x, const int y, const Pixel &, const Pixel &)> funcPixelMode;
    std::chrono::time_point<std::chrono::system_clock> m_tp1, m_tp2;
    std::vector<vi2d> vFontSpacing;
    std::vector<std::string> vDroppedFiles;
    std::vector<std::string> vDroppedFilesCache;
    vi2d vDroppedFilesPoint;
    vi2d vDroppedFilesPointCache;

    // Command Console Specific
    bool bConsoleShow = false;
    bool bConsoleSuspendTime = false;
    Key keyConsoleExit = Key::F1;
    std::stringstream ssConsoleOutput;
    std::streambuf *sbufOldCout = nullptr;
    vi2d vConsoleSize;
    vi2d vConsoleCursor = {0, 0};
    vf2d vConsoleCharacterScale = {1.0f, 2.0f};
    std::vector<std::string> sConsoleLines;
    std::list<std::string> sCommandHistory;
    std::list<std::string>::iterator sCommandHistoryIt;

    // Text Entry Specific
    bool bTextEntryEnable = false;
    std::string sTextEntryString = "";
    int32_t nTextEntryCursor = 0;
    std::vector<std::tuple<Key, std::string, std::string>> vKeyboardMap;

    // State of keyboard
    bool pKeyNewState[256] = {0};
    bool pKeyOldState[256] = {0};
    HWButton pKeyboardState[256] = {0};

    // State of mouse
    bool pMouseNewState[nMouseButtons] = {0};
    bool pMouseOldState[nMouseButtons] = {0};
    HWButton pMouseState[nMouseButtons] = {0};

    // The main engine thread
    void EngineThread();

    // If anything sets this flag to false, the engine
    // "should" shut down gracefully
    static std::atomic<bool> bAtomActive;

public:
    // "Break In" Functions
    void olc_UpdateMouse(int32_t x, int32_t y);
    void olc_UpdateMouseWheel(int32_t delta);
    void olc_UpdateWindowSize(int32_t x, int32_t y);
    void olc_UpdateViewport();
    void olc_ConstructFontSheet();
    void olc_CoreUpdate();
    void olc_PrepareEngine();
    void olc_UpdateMouseState(int32_t button, bool state);
    void olc_UpdateKeyState(int32_t key, bool state);
    void olc_UpdateMouseFocus(bool state);
    void olc_UpdateKeyFocus(bool state);
    void olc_Terminate();
    void olc_DropFiles(int32_t x, int32_t y, const std::vector<std::string> &vFiles);
    void olc_Reanimate();
    bool olc_IsRunning();

    // At the very end of this file, chooses which
    // components to compile
    virtual void olc_ConfigureSystem();

    // NOTE: Items Here are to be deprecated, I have left them in for now
    // in case you are using them, but they will be removed.
    // vf2d	vSubPixelOffset = { 0.0f, 0.0f };

public: // PGEX Stuff
    friend class PGEX;
    void pgex_Register(PGEX *pgex);

private:
    std::vector<PGEX *> vExtensions;
};

// O------------------------------------------------------------------------------O
// | PGE EXTENSION BASE CLASS - Permits access to PGE functions from extension    |
// O------------------------------------------------------------------------------O
class PGEX
{
    friend class PixelGameEngine;

public:
    PGEX(bool bHook = false);

protected:
    virtual void OnBeforeUserCreate();
    virtual void OnAfterUserCreate();
    virtual bool OnBeforeUserUpdate(float &fElapsedTime);
    virtual void OnAfterUserUpdate(float fElapsedTime);

protected:
    static PixelGameEngine *pge;
};