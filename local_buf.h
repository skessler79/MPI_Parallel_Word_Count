
namespace local_buf
{
    void sendLocalBuffer(std::string& buffer, char** local_buffer, int32_t& local_buf_sz, int comm_sz)
    {
        // Send to other processes.
        int64_t buf_sz = buffer.length();
        int64_t slice_sz = buf_sz / comm_sz;
        int64_t local_slice_sz;

        // Populate local buffer for process 0
        int64_t curr_slice = std::find_if_not(buffer.begin() + slice_sz, buffer.end(), isalpha) - buffer.begin();

        int64_t last_slice = curr_slice;
        local_buf_sz = curr_slice;
        *local_buffer = new char[curr_slice + 1];
        strcpy(*local_buffer, buffer.substr(0, curr_slice).c_str());

        // Send string slices to processes except last slice
        for (int i = 1; i < comm_sz - 1; ++i)
        {
            curr_slice = std::find_if_not(buffer.begin() + curr_slice + slice_sz, buffer.end(), isalpha) - buffer.begin();
            local_slice_sz = curr_slice - last_slice;
            std::string slice = buffer.substr(last_slice + 1, local_slice_sz);
            MPI_Send(slice.c_str(), local_slice_sz + 1, MPI_CHAR, i, 0, MPI_COMM_WORLD);
            last_slice = curr_slice;
        }

        // Send last string slice to last process
        local_slice_sz = buf_sz - last_slice - 1;
        std::string slice = buffer.substr(last_slice + 1, local_slice_sz);

        MPI_Send(slice.c_str(), local_slice_sz + 1, MPI_CHAR, comm_sz - 1, 0, MPI_COMM_WORLD);
    }

    void receiveLocalBuffer(char** local_buffer, int32_t& local_buf_sz)
    {
        // Probe for count
        MPI_Status status;
        MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_BYTE, &local_buf_sz);

        // Receive chunk
        *local_buffer = new char[local_buf_sz + 1];
        MPI_Recv(*local_buffer, local_buf_sz + 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
}
