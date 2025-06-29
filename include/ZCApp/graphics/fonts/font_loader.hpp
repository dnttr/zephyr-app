//
// Created by Damian Netter on 26/06/2025.
//

#pragma once

#include <array>
#include <vector>

namespace zc_app
{
    class font_loader
    {
        static constexpr std::array<wchar_t, 209> ascii = {
            L'\x0030', L'\x0031', L'\x0032', L'\x0033', L'\x0034',
            L'\x0035', L'\x0036', L'\x0037', L'\x0038', L'\x0039',
            // Uppercase A-Z
            L'\x0041', L'\x0042', L'\x0043', L'\x0044', L'\x0045',
            L'\x0046', L'\x0047', L'\x0048', L'\x0049', L'\x004A',
            L'\x004B', L'\x004C', L'\x004D', L'\x004E', L'\x004F',
            L'\x0050', L'\x0051', L'\x0052', L'\x0053', L'\x0054',
            L'\x0055', L'\x0056', L'\x0057', L'\x0058', L'\x0059',
            L'\x005A',
            // Lowercase a-z
            L'\x0061', L'\x0062', L'\x0063', L'\x0064', L'\x0065',
            L'\x0066', L'\x0067', L'\x0068', L'\x0069', L'\x006A',
            L'\x006B', L'\x006C', L'\x006D', L'\x006E', L'\x006F',
            L'\x0070', L'\x0071', L'\x0072', L'\x0073', L'\x0074',
            L'\x0075', L'\x0076', L'\x0077', L'\x0078', L'\x0079',
            L'\x007A',
            // Basic punctuation
            L'\x0020', // space
            L'\x0021', // !
            L'\x0022', // "
            L'\x0023', // #
            L'\x0024', // $
            L'\x0025', // %
            L'\x0026', // &
            L'\x0027', // '
            L'\x0028', // (
            L'\x0029', // )
            L'\x002A', // *
            L'\x002B', // +
            L'\x002C', // ,
            L'\x002D', // -
            L'\x002E', // .
            L'\x002F', // /
            L'\x003A', // :
            L'\x003B', // ;
            L'\x003C', // <
            L'\x003D', // =
            L'\x003E', // >
            L'\x003F', // ?
            L'\x0040', // @
            L'\x005B', // [
            L'\x005C', // \
    L'\x005D', // ]
            L'\x005E', // ^
            L'\x005F', // _
            L'\x0060', // `
            L'\x007B', // {
            L'\x007C', // |
            L'\x007D', // }
            L'\x007E', // ~

            //Latin 1 Supplement
            L'\u00C0', L'\u00C1', L'\u00C2', L'\u00C3', L'\u00C4', L'\u00C5',
            L'\u00C6', L'\u00C7', L'\u00C8', L'\u00C9', L'\u00CA', L'\u00CB',
            L'\u00CC', L'\u00CD', L'\u00CE', L'\u00CF', L'\u00D1', L'\u00D2',
            L'\u00D3', L'\u00D4', L'\u00D5', L'\u00D6', L'\u00D8', L'\u00D9',
            L'\u00DA', L'\u00DB', L'\u00DC', L'\u00DD',
            L'\u00E0', L'\u00E1', L'\u00E2', L'\u00E3', L'\u00E4', L'\u00E5',
            L'\u00E6', L'\u00E7', L'\u00E8', L'\u00E9', L'\u00EA', L'\u00EB',
            L'\u00EC', L'\u00ED', L'\u00EE', L'\u00EF', L'\u00F1', L'\u00F2',
            L'\u00F3', L'\u00F4', L'\u00F5', L'\u00F6', L'\u00F8', L'\u00F9',
            L'\u00FA', L'\u00FB', L'\u00FC', L'\u00FD', L'\u00FF',

            //Latin Extended-A
            L'\u0104', L'\u0105', L'\u0106', L'\u0107', L'\u010C', L'\u010D',
            L'\u010E', L'\u010F', L'\u0110', L'\u0111', L'\u0118', L'\u0119',
            L'\u0139', L'\u013A', L'\u013D', L'\u013E', L'\u0141', L'\u0142',
            L'\u0143', L'\u0144', L'\u0147', L'\u0148', L'\u0150', L'\u0151',
            L'\u0152', L'\u0153', L'\u015A', L'\u015B', L'\u0160', L'\u0161',
            L'\u0162', L'\u0163', L'\u0164', L'\u0165', L'\u016E', L'\u016F',
            L'\u0170', L'\u0171', L'\u0179', L'\u017A', L'\u017B', L'\u017C',
            L'\u017D', L'\u017E',

            L'\u00A1', // ¡
            L'\u00A3', // £
            L'\u00A5', // ¥
            L'\u00A9', // ©
            L'\u00AE', // ®
            L'\u20AC', // €
            L'\u2122', // ™
            L'\u2022', // •
            L'\u2018', // ‘
            L'\u2019', // ’
            L'\u201C', // “
            L'\u201D', // ”
            L'\u2026', // …
            L'\0' //Null terminator
        };

    public:
        static constexpr int ATLAS_WIDTH = 2048;
        static constexpr int ATLAS_HEIGHT = 2048;

        static void push_font(const std::string &name, std::vector<int8_t> provided_data);

        static void load_font(const std::string &name, int pixel_size);
    };
}
