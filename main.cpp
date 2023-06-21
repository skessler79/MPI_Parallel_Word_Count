#include "pch.h"

#include "file_io.h"
#include "serialization.h"
#include "local_buf.h"
#include "sorting.h"
#include "string_process.h"

int main(int argc, char** argv)
{
    int my_rank, comm_sz;

    // Raw string buffer local to each process 
    char* local_buffer;
    int32_t local_buf_sz;

    // User input variables
    int min_word, max_word;
    char order;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    if(my_rank == 0)
    {
        // Get user input into an aggregated raw string buffer
        std::string buffer;   
        file_io::getInput(buffer, min_word, max_word, order);

        // Split buffer into chunks for each process and send them
        local_buf::sendLocalBuffer(buffer, &local_buffer, local_buf_sz, comm_sz);
    }
    else
    {
        // Receive chunk from process 0
        local_buf::receiveLocalBuffer(&local_buffer, local_buf_sz);
    }

    // Broadcast word length limits to all processes
    MPI_Bcast(&max_word, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&min_word, 1, MPI_INT, 0, MPI_COMM_WORLD);


    double start_count = MPI_Wtime();

    // Split string into individual words with counts
    std::unordered_map<std::string_view, int64_t> words;

    // Transform string to lowercase and split to individual words
    string_process::transformStringLower(local_buffer, local_buf_sz);
    string_process::splitString(std::string_view{local_buffer}, words, min_word, max_word);

    char* serial_buffer;

    if (my_rank != 0)
    {
        // Serialize unordered_map and send to process 0
        int64_t serial_buf_sz = serialization::calculateSerializeMapSize(words);
        serial_buffer = new char[serial_buf_sz];
        serialization::serializeMap(words, serial_buffer);
        MPI_Send(serial_buffer, serial_buf_sz, MPI_BYTE, 0, 0, MPI_COMM_WORLD);
    
        delete[] serial_buffer;
    }
    else
    {
        // Deserialize buffers received into unordered_map
        for(int i = 1; i < comm_sz; ++i)
        {
            int32_t serial_buf_sz;
            MPI_Status status;

            // Get size of serial buffer
            MPI_Probe(MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_BYTE, &serial_buf_sz);

            serial_buffer = new char[serial_buf_sz];

            MPI_Recv(serial_buffer, serial_buf_sz, MPI_BYTE, status.MPI_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            std::unordered_map<std::string_view, int64_t> local_words;
            serialization::deserializeBuffer(serial_buffer, words);
        }
        
        // Sorting and output to file
        if (order == 'n')
        {
            // Sort by word count and output in descending order
            std::multimap<int64_t, std::string_view> sorted_words = sorting::sortByCount(words);
            file_io::fileOutput(sorted_words);
        }
        else if (order == 'a')
        {
            // Sort by alphabetical order and output in ascending order
            std::map<std::string_view, int64_t> sorted_words = sorting::sortByAlphabet(words);
            file_io::fileOutput(sorted_words);
        }
        
        double end_count = MPI_Wtime();
        std::cout << "Counting words : " << end_count - start_count << std::endl;
        

#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"      // False positive
        delete[] serial_buffer;
    }

    MPI_Finalize();
}