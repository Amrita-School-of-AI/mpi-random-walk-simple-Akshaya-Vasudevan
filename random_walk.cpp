#include <iostream>
#include <cstdlib> // For atoi, rand, srand
#include <ctime>   // For time
#include <mpi.h>

void walker_process();
void controller_process();

int domain_size;
int max_steps;
int world_rank;
int world_size;

int main(int argc, char **argv)
{
    // Initialize the MPI environment
    MPI_Init(&argc, &argv);

    // Get the number of processes and the rank of this process
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    if (argc != 3)
    {
        if (world_rank == 0)
        {
            std::cerr << "Usage: mpirun -np <p> " << argv[0] << " <domain_size> <max_steps>" << std::endl;
        }
        MPI_Finalize();
        return 1;
    }

    domain_size = std::atoi(argv[1]);
    max_steps = std::atoi(argv[2]);

    if (world_rank == 0)
    {
        controller_process();
    }
    else
    {
        walker_process();
    }

    MPI_Finalize();
    return 0;
}

void walker_process()
{
    std::srand(std::time(NULL) + world_rank); // Seed RNG
    int position = 0;
    int steps = 0;

    while (steps < max_steps && position >= -domain_size && position <= domain_size)
    {
        int move = (std::rand() % 2 == 0) ? -1 : 1;
        position += move;
        steps++;
    }

    // Print result (with "finished" keyword)
    std::cout << "Rank " << world_rank << ": Walker finished in " << steps << " steps. finished" << std::endl;

    // Send dummy message to controller
    int dummy = 1;
    MPI_Send(&dummy, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
}

void controller_process()
{
    int walkers = world_size - 1;
    int dummy;
    MPI_Status status;

    for (int i = 0; i < walkers; ++i)
    {
        MPI_Recv(&dummy, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
    }

    // ✨ Fixed this line
    std::cout << "Controller: All " << walkers << " walkers have finished." << std::endl;
}
