#pragma once

#define BITMAP_CHUNK 4096

namespace SWBitmaps
{
    struct Color
    {
        uint8_t Red = 0;
        uint8_t Green = 0;
        uint8_t Blue = 0;

        uint8_t& operator[](IN const uint8_t& i)
        {
            switch (i)
            {
            case 0:
                return Red;

            case 1:
                return Green;

            case 2:
                return Blue;

            default:
                throw;
            }
        }
    };

#pragma region Color definitions

    #define SWBITMAPS_COLOR_BLACK Color({0, 0, 0})
    #define SWBITMAPS_COLOR_WHITE Color({255, 255, 255})

#pragma endregion

    struct MappedPixel
    {
        MappedPixel() = default;

        MappedPixel(uint8_t* r, uint8_t* g, uint8_t* b) :
            m_Red(r),
            m_Green(g),
            m_Blue(b)
        {};

        uint8_t& Red() { return *m_Red; }
        uint8_t& Green() { return *m_Green; }
        uint8_t& Blue() { return *m_Blue; }

        void SetRedRef(uint8_t& r) { m_Red = &r; }
        void SetGreenRef(uint8_t& g) { m_Green = &g; }
        void SetBlueRef(uint8_t& b) { m_Blue = &b; }

        static bool IsEmpty(const MappedPixel& mp)
        {
            if (mp.m_Red == nullptr &&
                mp.m_Green == nullptr &&
                mp.m_Blue == nullptr)
                return true;

            return false;
        }

        static bool IsInvalid(const MappedPixel& mp)
        {
            if (mp.m_Red == nullptr ||
                mp.m_Green == nullptr ||
                mp.m_Blue == nullptr)
                return true;

            return false;
        }

        bool operator==(const MappedPixel& right)
        {
            if (m_Red == right.m_Red &&
                m_Green == right.m_Green &&
                m_Blue == right.m_Blue)
                return true;

            return false;
        }

    private:

        uint8_t* m_Red = nullptr;
        uint8_t* m_Green = nullptr;
        uint8_t* m_Blue = nullptr;

    };

    // Name of header type     | Size in bytes
    #define BITMAPCOREHEADER       12
    #define BITMAPCOREHEADER2      64
    #define BITMAPINFOHEADER       40      // (Created by Photoshop)
    #define BITMAPV2INFOHEADER     52
    #define BITMAPV3INFOHEADER     56      // (Created by Photoshop)
    #define BITMAPV4HEADER        108
    #define BITMAPV5HEADER        124

    class Bitmap
    {
    public:

        Bitmap() = default;

        ~Bitmap()
        {
            Destroy();
        };

    public:

        void Initialize(IN const std::wstring& path);

        void Destroy();

    public:

        void SaveToFile(IN const std::wstring& path);

    public:

        // Image manipulation ----------------------------------------------------------

        void ColorWhole(IN Color c);

        void ColorHalf(IN Color c);

        void MakeItBlack()
        {
            ColorWhole(SWBITMAPS_COLOR_BLACK);
        }

        void MakeItWhite()
        {
            ColorWhole(SWBITMAPS_COLOR_WHITE);
        }

        void MakeItRainbow();

        void MakeItNegative();

    public:

        // Getters ---------------------------------------------------------------------

        const bool& IsValid() const { return m_bIsValid; }

    private:

        void LoadFromPath();

        void ReadHeader();

        void MapImage();

        void PrintFirstChunk();

    private:

        std::wstring m_Path = L"";

        bool m_bIsValid = false;

        // First byte after the header
        uint8_t m_HeaderOffset = 0;
        uint64_t m_SizeOfBuff = 0;
        char* m_ImageBuff = nullptr;
        
        std::vector<MappedPixel> m_MappedImage;
    };
}
