#include <olcPGE/olcPGEX_CustomFont.hpp>


namespace olc
{

uint32_t _next_utf8_codepoint( const std::string& s, int& i )
{
    uint32_t c;
    uint8_t mask = (uint8_t)s[i] & 0xf0;
    if( mask == 0xf0 ) { c = ((uint32_t)s[i] & 0x07) << 18 | ((uint32_t)s[i+1] & 0x3f) << 12 | ((uint32_t)s[i+2] & 0x3f) << 6 | s[i+3] & 0x3f; i+=4; }
    else if( mask == 0xe0 ) { c = ((uint32_t)s[i] & 0x0f) << 12 | ((uint32_t)s[i+1] & 0x3f) << 6 | s[i+2] & 0x3f; i+=3; }
    else if( (mask & 0xe0) == 0xc0 ) { c = ((uint32_t)s[i] & 0x1f) << 6 | s[i+1] & 0x3f; i+=2; }
    else { c = s[i]; i++; }
    return c;
}


CustomFont::CustomFont(const std::string& sFontFile, olc::ResourcePack* pack)
{ LoadFromFile(sFontFile, pack); }


olc::rcode
CustomFont::LoadFromFile(const std::string& sFontFile, olc::ResourcePack* pack)
{
    fontSprite = std::make_unique<olc::Sprite>();

    auto result = fontSprite->LoadFromFile( sFontFile, pack );
    if ( result != olc::rcode::OK )
      return result;

    fontDecal = std::make_unique<olc::Decal>( fontSprite.get() );

    // Find the CFON signature and extract the embedded font information
    int dataRow = -1;
    for( auto i = fontSprite->height-1; dataRow<0 && i>-1; i-- )
    {
        if( fontSprite->GetPixel( 0, i ) == olc::Pixel('C','F','O','N') )
            dataRow = i;
    }

    if( dataRow > -1 )
    {
        // CFON signature found - presume the pixels contain valid font information
        uint32_t nFormatVersion = fontSprite->GetPixel( 1, dataRow ).n;
        uint32_t nGlyphs        = fontSprite->GetPixel( 2, dataRow ).n;
        uint32_t nOffset             = fontSprite->GetPixel( 3, dataRow ).n;
        fCharSize = { fontSprite->GetPixel( 4, dataRow ).n,
                      fontSprite->GetPixel( 5, dataRow ).n};
        uint32_t nCharsPerRow   = fontSprite->width / fCharSize.x;

        for( uint32_t i=0; i < nGlyphs; i++ )
        {
            int pixelIndex      = i*2 + 6;
            int pixelX          = pixelIndex % fontSprite->width;
            int pixelY          = pixelIndex / fontSprite->width + dataRow;
            uint32_t nWidth     = fontSprite->GetPixel( pixelX, pixelY ).n;
            uint32_t nGlyph     = fontSprite->GetPixel( pixelX+1, pixelY ).n;
            uint32_t x               = i % nCharsPerRow * fCharSize.x + nOffset;
            uint32_t y               = i / nCharsPerRow * fCharSize.y;
            mGlyphPositionsMono.emplace( nGlyph, std::make_pair<olc::vu2d, olc::vu2d>( {x,y}, {fCharSize.x,fCharSize.y-1}) );
            mGlyphPositionsProp.emplace( nGlyph, std::make_pair<olc::vu2d, olc::vu2d>( {x,y}, {nWidth, fCharSize.y-1}) );
        }
    }
    else
    {
        // No "CFON" signature found - hint to std out
        printf( "%s is not a valid custom font\n", sFontFile.c_str() );

        return olc::rcode::FAIL;
    }

    return olc::rcode::OK;
}


olc::vu2d CustomFont::GetTextSize(const std::string& s) const
{
    olc::vu2d size = { 0,1 };
    olc::vu2d pos = { 0,1 };
    for( int i=0; i < (int)s.size(); )
    {
        uint32_t c = _next_utf8_codepoint( s, i );
        if (c == '\n') { pos.y++;  pos.x = 0; }
        else pos.x++;
        size.x = std::max(size.x, pos.x);
        size.y = std::max(size.y, pos.y);
    }
    const olc::vf2d textSize = vf2d(size) * vf2d( fCharSize.x, fCharSize.y );
    return olc::vu2d(textSize.x, textSize.y);
}


olc::vu2d CustomFont::GetTextSizeProp(const std::string& s) const
{
    olc::vu2d size = { 0,1 };
    olc::vu2d pos = { 0,1 };
    for( int i=0; i < (int)s.size(); )
    {
        uint32_t c = _next_utf8_codepoint( s, i );
        if (c == '\n') { pos.y += 1 ;  pos.x = 0; }
        else pos.x += mGlyphPositionsProp.at(c).second.x;
        size.x = std::max(size.x, pos.x);
        size.y = std::max(size.y, pos.y);
    }

    size.y *= fCharSize.y;
    return size;
}


void CustomFont::DrawStringDecal(const olc::vf2d& pos, const std::string& sText, const Pixel col, const olc::vf2d& scale) const
{
    olc::vf2d spos = { 0.0f, 0.0f };
    for( int i=0; i < (int)sText.size(); )
    {
        uint32_t c = _next_utf8_codepoint( sText, i );
        if (c == '\n')
        {
            spos.x = 0; spos.y += fCharSize.y * scale.y;
        }
        else
        {
            auto& glyph = mGlyphPositionsMono.at(c);
            pge->DrawPartialDecal(pos + spos, fontDecal.get(), glyph.first, glyph.second, scale, col);
            spos.x += fCharSize.x * scale.x;
        }
    }
}


void CustomFont::DrawRotatedStringDecal(const olc::vf2d& pos, const std::string& sText, const float fAngle, const olc::vf2d& center, const Pixel col, const olc::vf2d& scale ) const
{
    olc::vf2d spos = center;
    for( int i=0; i < (int)sText.size(); )
    {
        uint32_t c = _next_utf8_codepoint( sText, i );
        if (c == '\n')
        {
            spos.x = center.x; spos.y -= fCharSize.y;
        }
        else
        {
            auto& glyph = mGlyphPositionsMono.at(c);
            pge->DrawPartialRotatedDecal(pos, fontDecal.get(), fAngle, spos, glyph.first, glyph.second, scale, col);
            spos.x -= fCharSize.x;
        }
    }
}


void CustomFont::DrawStringPropDecal(const olc::vf2d& pos, const std::string& sText, const Pixel col, const olc::vf2d& scale) const
{
    olc::vf2d spos = { 0.0f, 0.0f };
    for( int i=0; i < (int)sText.size(); )
    {
        uint32_t c = _next_utf8_codepoint( sText, i );
        if (c == '\n')
        {
            spos.x = 0; spos.y += fCharSize.y * scale.y;
        }
        else
        {
            auto& glyph = mGlyphPositionsProp.at(c);
            pge->DrawPartialDecal(pos + spos, fontDecal.get(), glyph.first, glyph.second, scale, col);
            spos.x += glyph.second.x * scale.x;
        }
    }
}


void CustomFont::DrawRotatedStringPropDecal(const olc::vf2d& pos, const std::string& sText, const float fAngle, const olc::vf2d& center, const Pixel col, const olc::vf2d& scale ) const
{
    olc::vf2d spos = center;
    for( int i=0; i < (int)sText.size(); )
    {
        uint32_t c = _next_utf8_codepoint( sText, i );
        if (c == '\n')
        {
            spos.x = center.x; spos.y -= fCharSize.y;
        }
        else
        {
            auto& glyph = mGlyphPositionsProp.at(c);
            pge->DrawPartialRotatedDecal(pos, fontDecal.get(), fAngle, spos, glyph.first, glyph.second, scale, col);
            spos.x -= glyph.second.x;
        }
    }
}

void DrawFontDecal(const olc::vf2d& pos, const olc::CustomFont* font, const std::string& sText, const Pixel col, const olc::vf2d& scale)
{
    return font->DrawStringDecal(pos, sText, col, scale);
}


void DrawRotatedFontDecal(const olc::vf2d& pos, const olc::CustomFont* font, const std::string& sText, const float fAngle, const olc::vf2d& center, const Pixel col, const olc::vf2d& scale )
{
    return font->DrawRotatedStringDecal(pos, sText, fAngle, center, col, scale);
}


void DrawFontPropDecal(const olc::vf2d& pos, const olc::CustomFont* font, const std::string& sText, const Pixel col, const olc::vf2d& scale)
{
    return font->DrawStringPropDecal(pos, sText, col, scale);
}


void DrawRotatedFontPropDecal(const olc::vf2d& pos, const olc::CustomFont* font, const std::string& sText, const float fAngle, const olc::vf2d& center, const Pixel col, const olc::vf2d& scale )
{
    return font->DrawRotatedStringPropDecal(pos, sText, fAngle, center, col, scale);
}
}
