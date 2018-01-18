#include "userthread.h"
#include "system.h"
#include "list.h"
#include "machine.h"
#include "addrspace.h"

#include <new>
#include <map>

#define MAX_NUM_USERTHREADS 128

static const int THREAD_STACK_SIZE = 5 * PageSize;

static int numThreads = 0;
static int numProcesses = 0;

/*!
 * Mapping between address space and the list of available thread stacks
 */
static std::map<AddrSpace *, List> threadStacks;

/*! Array that contains pointers to the serialized data structs for thread argumens
 *  Default value for an empty entry is 0
 */
static ThreadDescriptor_t thread_args[MAX_NUM_USERTHREADS];

/*! List that contain free thread IDs from 1 to MAX_NUM_USERTHREADS */
static List freeThreadIds;

/*!
 * Initializes the list of available thread IDs (from 1 to \c MAX_NUM_USERTHREADS)
 */
static void InitializeThreadStructs() {
    for (int ii = 0; ii < MAX_NUM_USERTHREADS; ++ii) {
        freeThreadIds.Append((void *)(ii + 1));
    }
}

/*!
 * Creates a list whose elements are pointers to the value of StackReg
 * that can be set for new threads
 *
 * This list is then associated with a respective address space in the map
 * (because different processes may have different addresses available for
 * threads stacks)
 *
 * The thread stack size is assumed to be fixed and be multiple of PageSize
 *
 */
static void SetUpAvailThreadStacks(AddrSpace *addrSpace, bool kernelRequest = false) {
    int begin = addrSpace->GetStack() - (kernelRequest ? 0 : THREAD_STACK_SIZE);
    int end = addrSpace->GetEndOfNoff();
    List &listOfStacks = threadStacks[addrSpace];
    while (begin - end > THREAD_STACK_SIZE) {
        listOfStacks.Append((void *)begin);
        begin -= THREAD_STACK_SIZE;
    }
}

/*!
 * \param f <--  thread parameters serialized to \c ThreadParam_t structure
 *
 * \brief This function serves as a callback for \c Thread::Fork method
 *
 * \returns tid (from 1 to MAX_NUM_USERTHREADS) if thread creation was successful
 *          -1 in case of error
 *
 * \note This function acquires a memory ownership of a structure pointed by \a f
 *       The memory used by f should be released when the thread dies
 */
static void StartUserThread(int f) {
    ThreadParam_t *cvtParam = (ThreadParam_t *)f;

    cvtParam->space->InitRegisters();
    cvtParam->space->RestoreState();

    int instr = (int)cvtParam->functionPtr;

    machine->WriteRegister (PCReg, instr);
    machine->WriteRegister (NextPCReg, 4 + instr);
    machine->WriteRegister(FIRST_PARAM_REGISTER, (int)cvtParam->functionParam);
    machine->WriteRegister (StackReg, cvtParam->topOfStack);
    machine->WriteRegister(31, cvtParam->retAddress);
    machine->Run();
}

/*!
 * Sets up a new kernel thread associated with the new user thread
 *
 * \param funPtr            <- pointer to function of type \c ThreadFun_t
 * \param arg               <- argument to be passed to \a funPtr when calling it
 * \param retAddress        <- address of a function that shall be called when
 *                             funPtr finishes executing
 * \param kernelRequest     <- whether this function is used for creating a new process
 *                             if this parameter is set to \c true the function will
 *                             allow 0 address for \a funPtr
 *
 *
 * \return thread ID if thread creation was successful
 *         -1 if \a funPtr is NULL
 *         -2 if number of threads in the system is equal to MAX_NUM_USERTHREADS
 *         -3 if there is no space left on stack for a new thread
 *         -4 ran out of memory
 */
int do_UserThreadCreate(int funPtr, int arg, int retAddress, AddrSpace *space, OpenFile *workingDirectoryFile, Thread *parent ,bool kernelRequest) {

    int retVal = -1;
    static bool firstTime = true;

    // Initialize structs during the first call
    if (firstTime) {
        firstTime = false;
        InitializeThreadStructs();
    }

    if (!kernelRequest && (0 == funPtr)) {
        /* The call should fail and return -1 */
        retVal = -1;
        goto early_exit;
    } else if (freeThreadIds.IsEmpty()) {
        /* Thread quota reached */
        retVal = -2;
        goto early_exit;
    } else {
        /* Proceed with creating our thread */

        if (space) {
            auto it = threadStacks.find(space);
            if (it == threadStacks.end()) {
                SetUpAvailThreadStacks(space, kernelRequest);
            }
        } else {
            DEBUG_MSG("Current thread Address space is NULL. \n");
        }

        void *topOfThreadStack = threadStacks[space].Remove();

        // Not enough space for this thread - fail the request
        if (!topOfThreadStack) {
            retVal = -3;
            goto early_exit;
        }

        ThreadParam_t *serializedThreadParam = new (std::nothrow) ThreadParam_t;

        if (!serializedThreadParam) {
            retVal = -4;
            goto early_exit;
        }

        serializedThreadParam->functionPtr = (ThreadFun_t)funPtr;
        serializedThreadParam->space = space;
        serializedThreadParam->functionParam = (void *)arg;
        serializedThreadParam->retAddress = retAddress;
        serializedThreadParam->topOfStack = (int)topOfThreadStack;

        // Generate debug name for the thread
        // TODO: Remove after active dev phase
        int threadNum = (int)freeThreadIds.Remove();

        Thread *newThread = new (std::nothrow) Thread("");

        /* Memory allocation failed */
        if (!newThread) {
            retVal = -4;
            goto early_exit;
        }

        /*!
         * Save the pointer to serialized parameter and thread pointer so it can be properly deleted
         * when the thread exits
         */
        thread_args[threadNum - 1].threadPtr = newThread;
        thread_args[threadNum - 1].argPtr = (int)serializedThreadParam;
        thread_args[threadNum - 1].waitingForJoin = true;
        //thread_args[threadNum - 1].kThread = kernelRequest;
        
        

        newThread->SetTID(threadNum);
        newThread->space = space;
        newThread->Fork(StartUserThread, (int)serializedThreadParam);
        newThread->SetWorkingDirectory(workingDirectoryFile);
        if(parent != NULL){
            //printf("Setting current thread table \n");
            newThread->setOpenFileTable(parent->getOpenFileTable());
        }
        
                
        retVal = threadNum;
        ++numThreads;

        if (kernelRequest) {
            ++numProcesses;
            if (1 == numProcesses) {
//                haltSync.P();
            }
        } else {
            int pTid = currentThread->Tid();
            thread_args[pTid - 1].children.Append((void*)threadNum);//store children tid
        }


    }

    early_exit:
    return retVal;
}

/*!
 * \param tid <--   thread ID that is exiting
 * \returns 0 if thread exit is successful
 *         -1 if tid is invalid
 */
void do_UserThreadExit() {
    int tid = currentThread->Tid();
    ThreadDescriptor_t *descriptor = &thread_args[tid - 1];
    ThreadParam_t *cvt = (ThreadParam_t *)descriptor->argPtr;

    threadStacks[currentThread->space].Append((void *)cvt->topOfStack);

    delete cvt;
    thread_args[tid - 1].synch->V();
    --numThreads;
    if (0 == numThreads) {
//        haltSync.V();
    }

    //TODO: Consider removing threadPtr from the descriptor
    descriptor->threadPtr->Finish();

    // Do not delete descriptor.threadPtr explicitly since it will be done by the scheduler
}

void do_SomeUserThreadExit(Thread* thread) {
    int tid = thread->Tid();
    ThreadDescriptor_t *descriptor = &thread_args[tid - 1];
    ThreadParam_t *cvt = (ThreadParam_t *)descriptor->argPtr;

    threadStacks[thread->space].Append((void *)cvt->topOfStack);

    delete cvt;
    thread_args[tid - 1].synch->V();
    --numThreads;
    if (0 == numThreads) {
//        haltSync.V();
    }

    //TODO: Consider removing threadPtr from the descriptor
    //descriptor->threadPtr->Finish();
    delete descriptor->threadPtr;

    // Do not delete descriptor.threadPtr explicitly since it will be done by the scheduler
}

/*!
 * \brief do_UserThreadJoin
 * \param tid
 * \return
 */
int do_UserThreadJoin(int tid) {
    int retVal = -1;

    ThreadDescriptor_t *descriptor;
    if ((tid < 1) || (tid > MAX_NUM_USERTHREADS)) {
        goto early_exit;
    }
    descriptor = &thread_args[tid - 1];

    if (!descriptor->waitingForJoin) {
        goto early_exit;
    }

    descriptor->synch->P();
    descriptor->argPtr = 0;
    descriptor->waitingForJoin = false;
    freeThreadIds.Append((void *)tid);
    retVal = 0;
early_exit:
    return retVal;
}


int do_KernelThreadCreate(AddrSpace *space, OpenFile *workingDirectoryFile) {
    int retVal = do_UserThreadCreate(0, 0, 0, space, workingDirectoryFile,NULL,true );
    return retVal;
}


void do_ExitCurrentProcess()
{    
    int tid = currentThread->Tid();


    //Check if it has userlevel threads
    while(!thread_args[tid - 1].children.IsEmpty()) {
        int childTid = (int)thread_args[tid - 1].children.Remove(); //Get child tid
        //Thread* thread = thread_args[childTid - 1].threadPtr;
        if(thread_args[childTid - 1].waitingForJoin) {
            do_UserThreadJoin(childTid); //consider deleting them

            //For deletion, doesn't work yet(tries to access thread after deleting it)
            /*do_SomeUserThreadExit(thread);

            scheduler->EvictThreadsById(childTid);

            printf("Waiting for children left behind %d\n", childTid);*/
        }
    }

    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    thread_args[tid - 1].synch->V();
    
    scheduler->EvictThreadsById(tid);
    delete currentThread->space;
    currentThread->space = nullptr;

    interrupt->SetLevel(oldLevel);

    --numProcesses;

    //--numThreads; //deleting thread from count?
    //printf("xiting num proc %d %d \n", numProcesses, tid );

    if (0 >= numProcesses) {
    //  haltSync.V();
        interrupt->Halt();
    }

    currentThread->Finish();
}
