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
    if (!m_Header.Valid)
        return;

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

// -----------------------------------------------------------------------------
void Bitmap::SaveToFile(IN const std::wstring& path)
{
    std::ofstream file(path,
        std::ios_base::binary | std::ios_base::out);

    if (!file.is_open())
    {
        m_Header.Valid = false;
        return;
    }

    for (uint64_t i = 0; i < m_SizeOfBuff; i += BITMAP_CHUNK)
        file.write((m_ImageBuff + i), BITMAP_CHUNK);

    file.close();
}

// Image manipulation ----------------------------------------------------------

// -----------------------------------------------------------------------------
#define FOR_WHOLE_IMAGE_I_K                                 \
for (uint64_t i = 0; i < m_MappedImage.GetHeight(); i++)    \
{                                                           \
    for (uint64_t k = 0; k < m_MappedImage.GetWidth(i); k++) \
    {

#define FOR_WHOLE_IMAGE_I_K_END }}

// -----------------------------------------------------------------------------
void Bitmap::ColorWhole(IN Color c)
{
    FOR_WHOLE_IMAGE_I_K;
        if (MappedPixel::IsInvalid(m_MappedImage.Pixel(i, k)))
            continue;

        auto& p = m_MappedImage.Pixel(i, k);

        p.Red() = c.Red;
        p.Green() = c.Green;
        p.Blue() = c.Blue;
    FOR_WHOLE_IMAGE_I_K_END;

    m_MappedImage.Pixel(1, 2).Red() = 255;
    m_MappedImage.Pixel(1, 2).Blue() = 255;
    m_MappedImage.Pixel(1, 2).Green() = 255;
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
        
        p.Red() = 256 - p.Red();
        p.Green() = 256 - p.Green();
        p.Blue() = 256 - p.Blue();
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
        m_Header.Valid = false;
        return;
    }
 
    uint64_t endOfFile = file.tellg();

    m_SizeOfBuff = sizeof(char) * endOfFile;
    m_ImageBuff = (char*) malloc(m_SizeOfBuff);

    file.seekg(std::ios_base::beg);

    for (uint64_t i = 0; !file.eof(); i++)
        file.read((m_ImageBuff + file.tellg()), BITMAP_CHUNK);

    file.close();

#ifdef _DEBUG
    // PrintFirstChunk();
#endif // _DEBUG
}

#define CAST_READ_JUMP(loadTo, dataType, buffer, jumpVal)  \
loadTo = *((dataType*)&buffer[jumpVal]);                        \
jumpVal += sizeof(dataType);

// -----------------------------------------------------------------------------
void Bitmap::ReadHeader()
{
    if ((m_ImageBuff[0] != 'B' ||
        m_ImageBuff[1] != 'M') ||
        m_SizeOfBuff < 14)
        return;
    m_Header.Valid = true;


    m_Header.FileSize = *((uint32_t*)(&m_ImageBuff[2]));
    m_Header.FileBeginOffset = *((uint32_t*)&m_ImageBuff[10]);

    if (m_Header.FileBeginOffset == 54)
    {
        uint8_t jump = 14;
        CAST_READ_JUMP(m_Header.SizeOfHeader, uint32_t, m_ImageBuff, jump);
        CAST_READ_JUMP(m_Header.Width, int32_t, m_ImageBuff, jump);
        CAST_READ_JUMP(m_Header.Height, int32_t, m_ImageBuff, jump);
        CAST_READ_JUMP(m_Header.ColorPlanes, uint16_t, m_ImageBuff, jump);
        CAST_READ_JUMP(m_Header.ColorDepth, uint16_t, m_ImageBuff, jump);
        CAST_READ_JUMP(m_Header.CompressionMethod, uint32_t, m_ImageBuff, jump);
        CAST_READ_JUMP(m_Header.ImageSize, uint32_t, m_ImageBuff, jump);
        CAST_READ_JUMP(m_Header.HorizontalResolution, int32_t, m_ImageBuff, jump);
        CAST_READ_JUMP(m_Header.VerticalResolution, int32_t, m_ImageBuff, jump);
        CAST_READ_JUMP(m_Header.ColorsInPalete, uint32_t, m_ImageBuff, jump);
        CAST_READ_JUMP(m_Header.ImportantColorsUsed, uint32_t, m_ImageBuff, jump);
    }
}

// -----------------------------------------------------------------------------
void Bitmap::MapImage()
{
    // Every row the countDown should be reseted to countDownNullVal
    const uint8_t countDownNullVal = -1;
    uint8_t countDown = countDownNullVal;

    
    const uint64_t calcWidth = std::floorl((((m_Header.ColorDepth * m_Header.Width) + 31) / 32)) * 4;

    uint64_t row = 0;
    for (uint64_t i = m_Header.FileBeginOffset; 
        i < m_SizeOfBuff; 
        i++, row++, countDown++)
    {
        if (row % calcWidth == 0)
        {
            row = 0;
            m_MappedImage.PushBackRow();
            countDown = countDownNullVal;
        }
        if (countDown >= 3)
        {
            m_MappedImage.PushBackPixel();
            countDown = 0;
        }

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
            throw;
        }
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