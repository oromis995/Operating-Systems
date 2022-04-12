#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define nPartitions 4
//total memory
#define memSize 1024
//one partition
#define partitionSize 256
#define true 1
#define false 0
#define nInterrupts 5

//state definitions
#define newState 10
#define ready 11
#define runMe 12
#define wait 13
#define terminate 14


#define scheduler 0
#define iowStart 1
#define iowComplete 2
#define jPoolLoad 3
#define jobHalt 4


// Here is the time quantum. Setting this to a large
// number will give the same results as FCFS.
#define TQ 5

struct pcb {
    int pid;
    int arrival;
    int first;
    int complete;
    int bursts;
    int state;
    int partitionNum;
    int jPC;
    char instructions[128];
};

// Computer Components
char userMem[memSize];
char partAvail[nPartitions];
int PC;
char IR;
char iFlags[nInterrupts];

// OS support variables.
struct pcb jobPool[16];
int pid = 0;
int currentJob = -1;
int TQinterrupt;
// This variable turns on Time Quantum interrupts
// for use with the round robin scheduler.
// mode allows for single variable schedule switching
// mode 0 = fcfs, 1 = sjf, 2 = rr
int mode = 1;

// Functions
void load(char *job, int address, int len);
void cpuMe2(int nJobs, int time);
int submitJobs(char *jFile);
int doTerminateQ(int nJobs);
void interruptService(int nJobs, int time);
int getNewPid(void);
void showMemory(void);
int firstComefirstServe(int nJobs, int time);
int shortestJobFirst(int nJobs, int time);
int roundRobinMe(int nJobs, int time);
void getRunStats(int nJobs);

int main()
{
    int j;

    int nJobs;
    char running = true;
    int failSafe = 0;
    int numComplete = 0;

    int time = 0;

    // Set the time quantum interrupt time.
    TQinterrupt = time;

    printf("Hello TV Land!\n");

    // Boot Sequence.
    // Clear memory.
    for(j=0;j<memSize;j++) {
        userMem[j] = '_';
    }
    // Clear interrupt flags.
    for(j=0;j<nInterrupts;j++) {
        iFlags[j] = false;
    }
    // Set partition available flags.
    for(j=0;j<nPartitions;j++) {
        partAvail[j] = true;
    }

    // Load jobs into our jobpool/pcb structure.  This does not mean they are in
    // memory yet.  We will fish them out of the pool and put them into
    // memory as they arrive based on arrival time.
    nJobs = submitJobs("jobs.txt");
    printf("\nPreparing to process: nJobs = %d \n", nJobs);

    while(running) {
        // Set flags.
        // Jobs are loaded into the job pool on a constant interval.
        if ((time%1)== 0) {
            iFlags[jPoolLoad] = true;
        }

        if (mode==2) {
            if (time == TQinterrupt) {
                iFlags[scheduler] = true;
                TQinterrupt = time + TQ;
            }
        }

        // Service interrupts.
        interruptService(nJobs, time);

        // Call CPU for user job.
        if (currentJob != -1) {
            cpuMe2(nJobs, time);
        }

        numComplete = doTerminateQ(nJobs);
        if (numComplete == nJobs)
            running = false;

        failSafe++;
        if (failSafe == 200)
            running = false;

        time++;
    }
    showMemory();
    getRunStats(nJobs);

    return 0;
}



int doTerminateQ(int nJobs)
{
    int j;
    int completed = 0;

    for(j=0;j<nJobs;j++) {
        if (jobPool[j].state == terminate) {
            completed++;
        }
    }
    return completed;
}

int submitJobs(char *jFile)
{
    FILE *in;
    int nJobs;
    int j;
    int nBursts;

    int arrivalTime;
    char instructions[129];

    in = fopen(jFile, "r");
    if (in != NULL) {
        printf("\nsubmitJobs: Input file %s opened \n", jFile);
        fscanf(in, "%d", &nJobs);
        printf("submitJobs: number of jobs = %d \n\n", nJobs);

        for(j=0;j<nJobs;j++) {
            // Read arrival time and instructions from job file.
            fscanf(in, "%d", &arrivalTime);
            printf("submitJobs: arrivalTime = %d \n", arrivalTime);
            fscanf(in, "%s", instructions);
            printf("submitJobs: instructions = %s \n", instructions);

            // Load the arrival time and instructions into the submitted jobs array of pcbs.
            jobPool[j].arrival = arrivalTime;
            jobPool[j].first = -1;
            jobPool[j].complete = -1;
            jobPool[j].state = newState;

            // Load pcb with relevant data.
            jobPool[j].pid = getNewPid();
            strcpy(jobPool[j].instructions, instructions);
            nBursts = strlen(jobPool[j].instructions);
            jobPool[j].bursts = nBursts;
        }
        fclose(in);
    }
    else {
        printf("submitJobs: Error - Job input file %s not opened \n", jFile);
    }

    return nJobs;
}

void load(char *job, int address, int len)
{
    int j;
    printf("job instructions = %s \n", job);
    for(j=0;j<len;j++) {
        if ((address + j) < 1024) {
            userMem[address + j] = job[j];
        }
        else {
            printf("Loader: Error - address (%d) exceeds memory size \n", address+j);
        }
    }
}

void cpuMe2(int nJobs, int time)
{
    // Fetch
    IR = userMem[PC];

    // Decode and Execute
    if (IR == 'a') {
        // assignment statement
    }
    else if (IR == 'i') {
        // input statement
        iFlags[iowStart] = true;
    }
    else if (IR == 'o') {
        // output statement
        iFlags[iowStart] = true;
    }
    else if (IR == 'w') {
        // wait statement
        iFlags[iowStart] = true;
    }
    else if (IR == 'e') {
        // end of program statement
        iFlags[jobHalt] = true;
    }

    // Increment program counter.
    PC++;
}

void interruptService(int nJobs, int time)
{
    int j, k;
    int address, len;
    int pNumber;
    int newJob;
    char found, looking;

    // Input, output, wait start flag
    if (iFlags[iowStart] == true) {
        // For now, reset the flag.
        iFlags[iowStart] = false;
    }
    // Input, output, wait complete flag
    if (iFlags[iowComplete] == true) {
        // For now, reset the flag.
        iFlags[iowComplete] = false;
    }
    // Job pool load.  Handles the "new state"
    if (iFlags[jPoolLoad] == true) {
        for(j=0;j<nJobs;j++) {
            // if a job has arrived do new state operations
            if ((time >= jobPool[j].arrival)&&(jobPool[j].state == newState)) {
                printf("\nservice jobpool load: time = %d  job arrival time = %d \n", time, jobPool[j].arrival);

                // Load job into empty partition.
                // Find first empty partition.
                k = 0;
                found = false;
                looking = true;
                while(looking == true) {
                    if (partAvail[k] == true) {
                        found = true;
                        looking = false;
                    }
                    else {
                        k++;
                    }
                    if (k == nPartitions) {
                        looking = false;
                    }
                } // end while
                if (found == true) {
                    partAvail[k] = false;
                    // Load the code into the partition.
                    address = k * partitionSize;
                    len = jobPool[j].bursts;
                    load(jobPool[j].instructions, address, len);
                    // update the partition number.
                    jobPool[j].partitionNum = k;
                    // update the state of the process.
                    jobPool[j].state = ready;
                    // update the jobs pc to the program entry point.
                    jobPool[j].jPC = address;
                    // Set scheduler flag.
                    if (currentJob == -1) {
                        iFlags[scheduler] = true;
                    }
                    printf("service jobpool load: Job in partition %d \n", k);
                }
                else {
                    printf("service jobpool load: No partition available. \n");
                }
            }
        } // end for.

        // Reset job pool load interrupt flag.
        iFlags[jPoolLoad] = false;
    } // End if job pool interrupt flag was set.

    if (iFlags[jobHalt] == true) {
        printf("\nservice job complete: current job = %d    time = %d \n", currentJob, time);
        // Set current job to complete.
        jobPool[currentJob].state = terminate;
        // Set completion time.
        jobPool[currentJob].complete = time;
        // Release the partition for the job.
        pNumber = jobPool[currentJob].partitionNum;
        printf("PC = %d partitionSize = %d Freeing partition number %d \n", PC, partitionSize, pNumber);
        partAvail[pNumber] = true;
        // Reset the job complete interrupt flag.
        iFlags[jobHalt] = false;
        // Set the scheduler flag.
        iFlags[scheduler] = true;

        if (mode==2) {
            TQinterrupt = time + TQ;
        }
    }

    // Scheduler flag
    if (iFlags[scheduler] == true) {
        printf("\nservice schedule. \n");
        // Set current job based on scheduling algorithm.
        if(mode == 0){
            newJob = firstComefirstServe(nJobs, time);
        }
        else if(mode == 1){
            newJob = shortestJobFirst(nJobs, time);
        }
        else if(mode == 2){
            newJob = roundRobinMe(nJobs, time);
        }
        

        printf("service schedule: newJob = %d \n", newJob);

        // If a new job has been scheduled.
        if (newJob != -1) {
            // Based on current job, execute context switch.
            if (currentJob == -1) {
                currentJob = newJob;
                jobPool[currentJob].state = runMe;
                // Set first time in run q.
                if (jobPool[currentJob].first == -1)
                    jobPool[currentJob].first = time;
            }
            else {
                // Save Context data.
                if (jobPool[currentJob].state != terminate) {
                    jobPool[currentJob].state = ready;
                    jobPool[currentJob].jPC = PC;
                }

                // Switch the context to the new job.
                currentJob = newJob;
                jobPool[currentJob].state = runMe;
                // Set first time in run q.
                if (jobPool[currentJob].first == -1)
                    jobPool[currentJob].first = time;
                PC = jobPool[currentJob].jPC;
            }
        }

        // Reset scheduler flag.
        iFlags[scheduler] = false;
    }
}

int firstComefirstServe(int nJobs, int time){
    int jArrivalTime = 10000;
    int myJob = -1;

    for (int i = 0; i < nJobs; i++) {
        if(jobPool[i].state == ready){
            if(jobPool[i].arrival < jArrivalTime){
                jArrivalTime = jobPool[i].arrival;
                myJob = i;
            }
        }
    }

    if(myJob != -1){
        printf("FCFS scheduled: myJob = %d \n", myJob);
    }
    
    return myJob;
}

int shortestJobFirst(int nJobs, int time){
    int jLength = 10000;
    int myJob = -1;

    for (int i = 0; i < nJobs; i++) {
        if(jobPool[i].state == ready){
            if(jobPool[i].bursts < jLength){
                jLength = jobPool[i].bursts;
                myJob = i;
            }
        }
    }

    if(myJob != -1){
        printf("SJF scheduled: myJob = %d \n", myJob);
    }
    return myJob;
}


int roundRobinMe(int nJobs, int time){
    int jProgCount = 10000;
    int myJob = -1;

    for (int i = 0; i < nJobs; i++) {
        if(jobPool[i].state == ready){
            printf("id= %d pc= %d realPC= %d \n", i, jobPool[i].jPC, (jobPool[i].jPC-(jobPool[i].partitionNum*partitionSize)));
            if((jobPool[i].jPC-(jobPool[i].partitionNum*partitionSize)) < jProgCount){
                jProgCount = (jobPool[i].jPC-(jobPool[i].partitionNum*partitionSize));
                myJob = i;
            }
        }
    }

    if(myJob != -1){
        printf("RR scheduled: myJob = %d \n", myJob);
    }
    return myJob;
}

int getNewPid(void)
{
    int myPid;

    myPid = pid;
    pid++;
    return myPid;
}

void showMemory(void)
{
    int j;

    printf("\n\nUser Memory: \n");
    for(j=1;j<=memSize;j++) {
        printf("%c", userMem[j-1]);
        if ((j%64) == 0){
            printf("\n");
        }
    }
    printf("\n");
}


void getRunStats(int nJobs)
{
    int j;
    float sumFirst, sumComplete;
    float art, att;

    printf("\nProcessing Statistics: \n");
    printf("pid   bursts   arrival   first   complete \n");
    sumFirst = sumComplete = 0.0f;
    for(j=0;j<nJobs;j++) {
        printf("%d     %d       %d       %d     %d \n", jobPool[j].pid, jobPool[j].bursts, jobPool[j].arrival, jobPool[j].first, jobPool[j].complete);
        sumFirst = sumFirst + (jobPool[j].first - jobPool[j].arrival);
        sumComplete = sumComplete + (jobPool[j].complete - jobPool[j].arrival);
    }
    art = sumFirst/nJobs;
    att = sumComplete/nJobs;
    printf("art: %.02f att: %.02f \n", art, att);
}

