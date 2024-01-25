#include "Pch.h"

#include "Bitmap.hpp"

using namespace SWBitmaps;


// Bitmap ----------------------------------------------------------------------

// -----------------------------------------------------------------------------
void Bitmap::Initialize(IN const std::wstring& path)
{
    m_Path = path;

    LoadFromPath();
    ReadHeader();
    MapImage();
}

// -----------------------------------------------------------------------------
void Bitmap::Destroy()
{
    m_MappedImage.Clear();

    if (m_ImageBuff != nullptr)
    {
        free(m_ImageBuff);
        m_ImageBuff = nullptr;
    }
}

// Image manipulation ----------------------------------------------------------

// -----------------------------------------------------------------------------
void Bitmap::SaveToFile(IN const std::wstring& path)
{
    std::ofstream file(path,
        std::ios_base::binary | std::ios_base::out);

    if (!file.is_open())
    {
        m_bIsValid = false;
        return;
    }

    for (uint64_t i = 0; i < m_SizeOfBuff; i += BITMAP_CHUNK)
        file.write((m_ImageBuff + i), BITMAP_CHUNK);

    file.close();
}

// -----------------------------------------------------------------------------
#define FOR_WHOLE_IMAGE_I_K                            \
for (uint64_t i = 0; i < m_MappedImage.GetHeight(); i++)    \
{                                                           \
    for (uint64_t k = 0; k < m_MappedImage.GetWidth(); k++) \
    {

#define FOR_WHOLE_IMAGE_I_K_END }}

// -----------------------------------------------------------------------------
void Bitmap::ColorWhole(IN Color c)
{
    FOR_WHOLE_IMAGE_I_K
        if (MappedPixel::IsInvalid(m_MappedImage.Pixel(i, k)))
            continue;

        auto& p = m_MappedImage.Pixel(i, k);

        p.Red() = c.Red;
        p.Green() = c.Green;
        p.Blue() = c.Blue;
    FOR_WHOLE_IMAGE_I_K_END
}

// -----------------------------------------------------------------------------
void SWBitmaps::Bitmap::ColorHalf(IN Color c)
{
    FOR_WHOLE_IMAGE_I_K
        if (MappedPixel::IsInvalid(m_MappedImage.Pixel(i, k)))
            continue;

        auto& p = m_MappedImage.Pixel(i, k);

        p.Red() = c.Red;
        p.Green() = c.Green;
        p.Blue() = c.Blue;
    FOR_WHOLE_IMAGE_I_K_END
}

// -----------------------------------------------------------------------------
void Bitmap::MakeItRainbow()
{
#pragma warning ( push )
#pragma warning ( disable : 4244 )
    srand(time(NULL));
#pragma warning ( pop )

    FOR_WHOLE_IMAGE_I_K
        if (MappedPixel::IsInvalid(m_MappedImage.Pixel(i, k)))
            continue;
        
        auto& p = m_MappedImage.Pixel(i, k);
        
        p.Red() = std::rand() % 256;
        p.Green() = std::rand() % 256;
        p.Blue() = std::rand() % 256;
    FOR_WHOLE_IMAGE_I_K_END
}

// -----------------------------------------------------------------------------
void Bitmap::MakeItNegative()
{
    FOR_WHOLE_IMAGE_I_K
        if (MappedPixel::IsInvalid(m_MappedImage.Pixel(i, k)))
            continue;
        
        auto& p = m_MappedImage.Pixel(i, k);
        
        p.Red() = std::rand() % 256;
        p.Green() = std::rand() % 256;
        p.Blue() = std::rand() % 256;
    FOR_WHOLE_IMAGE_I_K_END
}

// Private ---------------------------------------------------------------------

// -----------------------------------------------------------------------------
void Bitmap::LoadFromPath()
{
    std::ifstream file(m_Path,
        std::ios_base::binary | std::ios_base::in | std::ios_base::ate);

    if (!file.is_open())
    {
        m_bIsValid = false;
        return;
    }
 
    uint64_t endOfFile = file.tellg();

    m_SizeOfBuff = sizeof(char) * endOfFile;
    m_ImageBuff = (char*) malloc(m_SizeOfBuff);

    file.seekg(std::ios_base::beg);

    for (uint64_t i = 0; !file.eof(); i++)
        file.read((m_ImageBuff + file.tellg()), BITMAP_CHUNK);

    file.close();

    m_bIsValid = true;

#ifdef _DEBUG
    // PrintFirstChunk();
#endif // _DEBUG
}

// -----------------------------------------------------------------------------
void Bitmap::ReadHeader()
{




    m_HeaderOffset = 56; 
    m_HeaderOffset++;// And jump over
}

// -----------------------------------------------------------------------------
void Bitmap::MapImage()
{
    // Every row the countDown should be reseted

    uint8_t countDown = -1;

    for (uint64_t i = m_HeaderOffset; i < m_SizeOfBuff; i++)
    {
        if (countDown >= 3)
        {
            countDown = 0;
            m_MappedImage.PushBackPixel();
        }

        // The order of colors is BGR not RGB
        // https://pl.wikipedia.org/wiki/Windows_Bitmap (bottom of page)
        switch (countDown)
        {
        case 0:
            m_MappedImage.LastPixel().SetBlueRef((uint8_t&)m_ImageBuff[i]);
            break;

        case 1:
            m_MappedImage.LastPixel().SetGreenRef((uint8_t&)m_ImageBuff[i]);
            break;

        case 2:
            m_MappedImage.LastPixel().SetRedRef((uint8_t&)m_ImageBuff[i]);
            break;

        default:
            break;
        }
        
        countDown++;
    }
}

// -----------------------------------------------------------------------------
void Bitmap::PrintFirstChunk()
{
    std::cout << "--------------------------------------------------" << std::endl;
    std::string s(m_ImageBuff, BITMAP_CHUNK);
    for (size_t i = 0; i < BITMAP_CHUNK; i++)
        std::cout << (int)s[i] << " ";
    std::cout << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;
}