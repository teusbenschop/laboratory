#include <cstdlib>
#include <cstddef>

// Include the file named ft2build.h.
// It contains various macro declarations that are later used
// to #include the appropriate public FreeType 2 header files.
// Include the main FreeType 2 API header file.
// Do that using the macro FT_FREETYPE_H.
// FT_FREETYPE_H is a special macro defined in file ftheader.h.
// It contains some installation-specific macros
// to name other public header files of the FreeType 2 API.
#include <ft2build.h>
#include FT_FREETYPE_H

#include <thread>
#include <vector>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <random>
#include <atomic>
#include <chrono>
#include <future>
#include <memory>
#include <latch>
#include <barrier>
#include <sstream>
#include <shared_mutex>
#include "main.h"

// Based on the FreeType tutorial at https://freetype.org/freetype2/docs/tutorial/index.html


// The handle to the FreeType 2 library.
FT_Library freetype_library {};

// The font face object that stores describes the type face and style.
FT_Face font_face {};

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    try {
        
        // Initialize the FreeType library handle.
        // This creates a new instance of the FreeType 2 library
        // and sets the handle library to it.
        // It loads each module that FreeType knows about in the library.
        // Among others, the new library object is able to handle
        // TrueType, Type 1, CID-keyed & OpenType/CFF fonts gracefully.
        // The function returns an error code.
        // An error code of 0 (FT_Err_Ok) always means that the operation was successful.
        // otherwise, the value describes the error, and library is set to NULL.
        // A list of all FreeType error codes can be found in file fterrdef.h.
        FT_Error freetype_error = FT_Init_FreeType (&freetype_library);
        if (freetype_error != FT_Err_Ok) {
            std::stringstream ss {};
            ss << "Failed to initialize the FreeType2 library with error code " << freetype_error;
            throw std::runtime_error (ss.str());
        }
        std::cout << "Initialized the FreeType2 font library" << std::endl;

        // Load the font face from file.
        // The font face describes the given typeface and style.
        std::string font_path = "./arial.ttf";
        freetype_error = FT_New_Face (freetype_library, font_path.c_str(), 0, &font_face);
        if (freetype_error == FT_Err_Unknown_File_Format)
        {
            std::stringstream ss {};
            ss << "Could not load the font because the font format of file " << font_path << " is unsupported";
            throw std::runtime_error (ss.str());
        }
        else if (freetype_error)
        {
            std::stringstream ss {};
            ss << "Could not load the font because of error " << freetype_error;
            throw std::runtime_error (ss.str());
        }
        std::cout << "Loaded font " << font_path << std::endl;
        
        // The font face object models all information that globally describes the face.
        // Usually, this data can be accessed directly by dereferencing a handle,
        // like in face−>num_glyphs.
        // The complete list of available fields is in the FT_FaceRec structure description.

        // The number of of glyphs available in the font face.
        // A glyph is a character image, nothing more.
        // It thus doesn't necessarily correspond to a character code.
        std::cout << "The font contains " << font_face->num_glyphs << " glyphs" << std::endl;

        // The face_flags is a 32-bit integer containing bit flags that describe some face properties.
        // For example, the flag FT_FACE_FLAG_SCALABLE indicates that the face's font format is scalable
        // and that glyph images can be rendered for all character pixel sizes.
        // For more information on face flags, please read the FreeType 2 API Reference.
        // https://freetype.org/freetype2/docs/reference/ft2-base_interface.html#ft_face_flag_xxx
        {
            int face_flags = font_face->face_flags;
            bool font_is_scalable = face_flags & FT_FACE_FLAG_SCALABLE;
            if (font_is_scalable) {
                std::cout << "The font is scalable" << std::endl;
            } else {
                throw std::runtime_error ("Error, the font is not scalable");
            }
        }

        // The maximum height of the font in pixels.
        int pixel_height {25};
        
        // Set the height of the font in pixels.
        // The width is 0, that means, use the same as the heigfht.
        std::cout << "Setting the font height to " << pixel_height << " pixels" << std::endl;
        freetype_error = FT_Set_Pixel_Sizes (font_face, 0, pixel_height);
        if (freetype_error != FT_Err_Ok) {
            std::stringstream ss {};
            ss << "Failed to set the font height with error code " << freetype_error;
            throw std::runtime_error (ss.str());
        }

        // The character to display.
        // Just now the ASCII encoding is used, but this should be improved to use Unicode.
        // Because this should use the UTF-32 representation form of Unicode.
        // For example, if loading character U+1F028, use value 0x1F028 as the value for charcode.
        char character_to_display = 'A';
        FT_ULong charcode = static_cast<FT_ULong>(character_to_display);
        
        // Get the glyph index from the font based on the character code point.
        FT_UInt glyph_index = FT_Get_Char_Index (font_face, charcode);
        // This function does not return an error code.
        // When a given character code has no glyph image in the face, value 0 is returned.
        // By convention, this always corresponds to a special glyph image called "the missing glyph",
        // which is commonly displayed as a box or a space.
        // Do the error check here.
        if (glyph_index == 0) {
            std::stringstream ss {};
            ss << "Cannot find the glyph index for character " << character_to_display << " in the font";
            throw std::runtime_error (ss.str());
        }
        std::cout << "The glyph index for character " << character_to_display << " is " << glyph_index << std::endl;

        // Based on the glyph index, load the corresponding glyph image.
        // The glyph image can be stored in various formats within the font file.
        // For fixed-size formats like FNT or PCF, each image is a bitmap.
        // Scalable formats like TrueType or CFF use vectorial shapes (outlines) to describe each glyph.
        // Some formats may have even more exotic ways of representing glyphs
        // (e.g., MetaFont – but this format is not supported).
        // FreeType 2 is flexible enough to support any kind of glyph format through a simple API.
        // The glyph image is always stored in a special object called a glyph slot.
        // A glyph slot is a container that is able to hold one glyph image at a time,
        // be it a bitmap, an outline, or something else.
        // Each font face object has a single glyph slot object that can be accessed as face->glyph.
        // Its fields are explained by the FT_GlyphSlotRec structure documentation.

        // The load_flags value is a set of bit flags to indicate some special operations.
        // The default value FT_LOAD_DEFAULT is 0.
        FT_Int32 load_flags {FT_LOAD_DEFAULT};
        
        // This function tries to load the corresponding glyph image from the font face.
        freetype_error = FT_Load_Glyph (font_face, glyph_index, load_flags);
        // If a bitmap is found for the corresponding glyph and pixel size, it is loaded into the slot.
        // Embedded bitmaps are always favoured over native image formats,
        // because it is assumed that they are higher-quality versions of the same glyph.
        // This can be changed by using the FT_LOAD_NO_BITMAP flag.
        // Otherwise, a native image for the glyph is loaded.
        // It is also scaled to the current pixel size,
        // as well as hinted for certain formats like TrueType and Type 1.
        if (freetype_error != FT_Err_Ok) {
            std::stringstream ss {};
            ss << "Failed to load the glyph from the font with error code " << freetype_error;
            throw std::runtime_error (ss.str());
        }
        std::cout << "The glyph for character " << character_to_display << " is loaded into the slot" << std::endl;

        // The field font_face−>glyph−>format describes the format used
        // for storing the glyph image in the slot.
        // If it is not FT_GLYPH_FORMAT_BITMAP, one can immediately convert it to a bitmap.
        // Since we need a monochrome rendering here, we are going to render it anyway.
        // The parameter render_mode is a set of bit flags to specify how to render the glyph image.
        // FT_RENDER_MODE_NORMAL, the default, renders an anti-aliased coverage bitmap
        // with 256 gray levels (also called a pixmap), as this is the default.
        // You can alternatively use FT_RENDER_MODE_MONO if you want to generate a 1-bit monochrome bitmap.
        // More values are available for the FT_Render_Mode enumeration value.
        FT_Render_Mode render_mode {FT_RENDER_MODE_MONO};
        freetype_error = FT_Render_Glyph (font_face->glyph, render_mode);
        if (freetype_error != FT_Err_Ok) {
            std::stringstream ss {};
            ss << "Failed to render the glyph into a monochrome bitmap with error code " << freetype_error;
            throw std::runtime_error (ss.str());
        }
        std::cout << "Rendered the glyph into a monochrome bitmap" << std::endl;

        // Once you have a bitmapped glyph image, you can access it directly through glyph->bitmap
        // (a simple descriptor for bitmaps or pixmaps),
        // and position it through glyph->bitmap_left and glyph->bitmap_top.
        // For optimal rendering on a screen the bitmap should be used
        // as an alpha channel in linear blending with gamma correction.
        // Note that bitmap_left is the horizontal distance
        // from the current pen position to the leftmost border of the glyph bitmap,
        // while bitmap_top is the vertical distance from the pen position (on the baseline)
        // to the topmost border of the glyph bitmap.
        // It is positive to indicate an upwards distance.
        std::cout << "The horizontal distance from the current pen position to the leftmost border of the glyph bitmap is " << font_face->glyph->bitmap_left << " pixels" << std::endl;
        std::cout << "The vertical distance from the pen position (on the baseline) to the topmost border of the glyph bitmap is " << font_face->glyph->bitmap_top << " pixels" << std::endl;

        // Here is the bitmap available for further processing.
        // https://freetype.org/freetype2/docs/reference/ft2-basic_types.html#ft_bitmap
        FT_Bitmap bitmap {font_face->glyph->bitmap};
        
        std::cout << "The number of bitmap rows: " << bitmap.rows << std::endl;
        std::cout << "Width (number of pixels in a row): " << bitmap.width << std::endl;
        std::cout << "Pitch: " << bitmap.pitch << std::endl;
        // The pitch's absolute value is the number of bytes taken by one bitmap row, including padding.
        // However, the pitch is positive when the bitmap has a ‘down’ flow,
        // and negative when it has an ‘up’ flow.
        // In all cases, the pitch is an offset to add to a bitmap pointer in order to go down one row.
        // Note that ‘padding’ means the alignment of a bitmap to a byte border,
        // and FreeType functions normally align to the smallest possible integer value.
        // For the B/W rasterizer, pitch is always an even number.
        std::cout << "Buffer: " << &(bitmap.buffer) << std::endl;
        // A typeless pointer to the bitmap buffer.
        // This value should be aligned on 32-bit boundaries in most cases.
        if (bitmap.pixel_mode == FT_PIXEL_MODE_MONO) std::cout << "Monochrome pixel model" << std::endl;
        else std::cout << "Pixel mode: " << bitmap.pixel_mode << std::endl;

        // Print the bitmap to the terminal.
        // The monochrome bitmap is in one-bit-per-pixel format.
        // This means that each byte of the bitmap represents 1 row and 8 columns.
        // In order to check if a pixel in column j is set in the bitmap, check the jth bit in a row.
        int rows = bitmap.rows;
        int cols = bitmap.width;
        for (int i = 0; i < rows; ++i)
        {
            for (int j = 0; j < cols; ++j)
            {
                int off  = i * bitmap.pitch + j / 8;
                if (bitmap.buffer[off] & (0xC0 >> (j % 8))) {
                    std::cout << "x";
                } else {
                    std::cout << " ";
                }
            }
            std::cout << std::endl;
        }
        
        
        
        
        
        
        // Clear up.
        FT_Done_Face (font_face);
        FT_Done_FreeType (freetype_library);

    } catch (const std::exception & exception) {
        std::cerr << exception.what() << std::endl;
    }
    return EXIT_SUCCESS;
}


