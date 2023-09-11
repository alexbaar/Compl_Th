#include <iostream>
#include <string>
#include <random>
#include <time.h>
#include <chrono>
#include <omp.h>

using namespace std;
using namespace std::chrono;

const int N = 500000;

void displayArray(int array3[], int arraySize)
{
    for (int i = 0; i < arraySize; i++)
    {
        cout << array3[i] << "  ";
    }
    cout << endl;
}

int partition(int array3[], int low, int high)
{
    int pivot = array3[low];
    int i = low, j = high;

    while (true)
    {
        while (array3[i] < pivot)
            i++;
        
        while (array3[j] > pivot)
            j--;
        
        if (i >= j)
            return j;
        
        swap(array3[i], array3[j]);
        i++;
        j--;
    }
}

void quicksort(int array3[], int low, int high)
{
    if (low < high)
    {
        int partitionIndex = partition(array3, low, high);

        // Parallelize quicksort calls using OpenMP
        #pragma omp parallel sections
        {
            #pragma omp section
            quicksort(array3, low, partitionIndex);

            #pragma omp section
            quicksort(array3, partitionIndex + 1, high);
        }
    }
}

int main()
{
    // Random Number Generation
    mt19937 e2(time(nullptr));
    int A[N];

    // Initialize array with non-zero values; if we skip this, there will be a zero value appearing in the sorted array, which 'steals' the spot of a number 
    // from the initial array; we initialize with any number outside of the random number generator, here the range was set between -100 and 100.
    // so we can use numbers under the lower limit or over the upper limit from line 70
    for (int i = 0; i < N; i++) {
        A[i] = -999999; // Set to a large negative value (guaranteed to be outside the range)
    }

    // Set the number of threads
    int num_threads = 8; // Adjust this to the desired number of threads
    omp_set_num_threads(num_threads);

    // Generate random values between -100 and 100 and assign them to A[i]
    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        uniform_int_distribution<int> dist(-400000, 400000);
        int random_value = dist(e2);
        A[i] = random_value; // Assign the generated random value to A[i]
    }

    // show unsorted array, populated with random values
    cout << "UNSORTED" << endl;
    //displayArray(A, N);

    cout << "Quicksort applied" << endl;

    // perform QS on the array; record the time taken by the process
    auto start = high_resolution_clock::now();
    quicksort(A, 0, N-1);
    auto end = high_resolution_clock::now();
    // chose microseconds 
    // 1 second = 1 000 000 microseconds
    auto duration = duration_cast<microseconds>(end - start);

    // Show sorted array and print time taken by QS operation
    //displayArray(A, N);
    // Print execution time
    cout << "Parallel execution time: " << duration.count() << " microseconds" << endl;
    
    return 0;
}
