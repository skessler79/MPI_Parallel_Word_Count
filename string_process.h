
namespace string_process
{
    // Transform string to lowercase
    inline void transformStringLower(char* str, const int32_t& str_sz)
    {
        std::transform(str, str + str_sz, str, [](char c){ return std::tolower(c); });
    }

    // Split string into words and count frequency of words
    void splitString(const std::string_view& str, std::unordered_map<std::string_view, int64_t>& tokens, const int& min_word, const int& max_word)
    {
        auto pos = str.begin();
        auto lastPos = pos;
        size_t length = str.length();

        // Loop until end of string
        while (lastPos < str.end())
        {
            // Find next non alphabetic character
            pos = std::find_if_not(lastPos, str.end(), isalpha);

            // End of string
            if (pos == str.end())
            {
                pos = str.begin() + length;
            }

            // Insert into hashmap
            if (pos != lastPos)
            {
                // Check length of word
                int len = pos - lastPos;
                if(len >= min_word && len <= max_word)
                {
                    auto results = tokens.try_emplace(std::string_view(lastPos, len), 1);
                    if (!results.second)
                    {
                        ++(results.first->second);
                    }
                }
            }

            // Find next lowercase character
            lastPos = std::find_if(pos, str.end(), islower);
        }
    }
}