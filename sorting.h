
namespace sorting
{
    std::multimap<int64_t, std::string_view> sortByCount(std::unordered_map<std::string_view, int64_t>& src)
    {
        std::multimap<int64_t, std::string_view> result;

        for (std::unordered_map<std::string_view, int64_t>::const_iterator it = src.begin(); it != src.end(); ++it)
        {
            result.insert(std::pair<int64_t, std::string_view>(it->second, it->first));
        }
        return result;
    }

    std::map<std::string_view, int64_t> sortByAlphabet(std::unordered_map<std::string_view, int64_t>& src)
    {
        std::map<std::string_view, int64_t> result;

        for (std::unordered_map<std::string_view, int64_t>::const_iterator it = src.begin(); it != src.end(); ++it)
        {
            result.insert(std::pair<std::string_view, int64_t>(it->first, it->second));
        }
        return result;
    }
}