#include "Utf16Converter.h"

String utf16leToString(uint8_t *data, size_t length)
{
    String result = "";

    // Ensure the length is even
    if (length % 2 != 0)
    {
        return result;
    }

    for (size_t i = 0; i < length; i += 2)
    {
        // Combine the two bytes to form a UTF-16 character
        uint16_t utf16_char = data[i] | (data[i + 1] << 8);

        // If the character is null (0x0000), break the loop as it indicates the end of the string
        if (utf16_char == 0x0000)
        {
            break;
        }

        // Append the character to the result string if it is within the ASCII range
        if (utf16_char <= 0x007F)
        {
            result += (char)utf16_char;
        }
    }

    return result;
}
