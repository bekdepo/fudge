struct smp_cpu
{

    unsigned int id;
    unsigned int core;
    unsigned int chip;
    unsigned int domain;

};

struct smp_architecture
{

    struct smp_cpu cpus[32];
    unsigned int count;

};
