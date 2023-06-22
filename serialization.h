
namespace serialization
{
    // Calculate buffer size required for serialization
    int64_t calculateSerializeMapSize(const std::unordered_map<std::string_view, int64_t>& words)
    {
        int64_t buffer_size = sizeof(int);
        for(const auto& it : words)
        {
            buffer_size += sizeof(int) + it.first.size() + sizeof(int64_t);
        }
        return buffer_size;
    }

    // Serialize unordered_map in order to send via MPI_Send
    void serializeMap(const std::unordered_map<std::string_view, int64_t>& words, char* buffer)
    {
        // Serialization buffer
        char* current_ptr = buffer;

        // Write the size of the unordered_map
        int map_size = words.size();
        std::memcpy(current_ptr, &map_size, sizeof(int));
        current_ptr += sizeof(int);
        

        // Write each serialized key-value pair
        for(const auto& it : words)
        {
            const std::string_view& key = it.first;
            int64_t value = it.second;

            // Write key size
            int key_size = key.size();
            std::memcpy(current_ptr, &key_size, sizeof(int));
            current_ptr += sizeof(int);

            // Write key
            std::memcpy(current_ptr, key.data(), key_size);
            current_ptr += key_size;

            // Write value
            std::memcpy(current_ptr, &value, sizeof(int64_t));
            current_ptr += sizeof(int64_t);
        }
    }

    // Deserialize received buffer into unordered map
    void deserializeBuffer(char* serial_buffer, std::unordered_map<std::string_view, int64_t>& words)
    {
        char* current_ptr = serial_buffer;

        int map_size;
        std::memcpy(&map_size, current_ptr, sizeof(int));
        current_ptr += sizeof(int);

        for(int i = 0; i < map_size; ++i)
        {
            // Read key size
            int key_size;
            std::memcpy(&key_size, current_ptr, sizeof(int));
            current_ptr += sizeof(int);

            // Read key string
            std::string_view key(current_ptr, key_size);
            current_ptr += key_size;

            // Read value
            int64_t value;
            std::memcpy(&value, current_ptr, sizeof(int64_t));
            current_ptr += sizeof(int64_t);
            
            auto results = words.try_emplace(key, value);
            if (!results.second)
            {
                results.first->second += value;
            }
        }
    }
}
