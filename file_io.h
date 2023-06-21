// Read in a single file into the buffer
namespace file_io
{
    std::string readFile(const std::string& file_name)
    {
        std::string buffer;
        std::ifstream f(file_name);
        f.seekg(0, std::ios::end);
        buffer.resize(f.tellg());
        f.seekg(0);
        f.read(buffer.data(), buffer.size());

        return buffer;
    }

    // Get input from user
    void getInput(std::string& buffer, int& min_word, int& max_word, char& order)
    {
        // buffer = readFile("lorem.txt");

        int num_files;
        // std::string buffer;
        std::string file_path;

        // Get number of text files to process
        std::cout << "Enter the number of text files: " << std::flush;
        std::cin >> num_files;

        // Get path of each text file
        for (int i = 1; i <= num_files; ++i)
        {
            std::cout << "Enter the path of text file " << i << ": ";
            std::cin >> file_path;

            buffer += readFile(file_path);
            buffer += ' ';
        }

        // Get min and max length of word
        std::cout << "Enter the minimum length of word to consider: ";
        std::cin >> min_word;
        std::cout << "Enter the maximum length of word to consider: ";
        std::cin >> max_word;

        // Get order of output
        std::cout << "Enter 'a' for alphabetical order or 'n' for number of words order: ";
        std::cin >> order;
    }

    template <typename T>
    void fileOutput(const T& sorted_words)
    {
        // File output
        std::ofstream out_file("output.txt");

        if constexpr(std::is_same<T, std::multimap<int64_t, std::string_view>>::value)
        {

            for (std::multimap<int64_t, std::string_view>::const_reverse_iterator it = sorted_words.rbegin(); it != sorted_words.rend(); ++it)
            {
                out_file << it->first << '\t' << it->second << '\n';
            }
        }
        else
        {
            for (const auto& it : sorted_words)
            {
                out_file << it.first << '\t' << it.second << '\n';
            }
        }
    }
}
