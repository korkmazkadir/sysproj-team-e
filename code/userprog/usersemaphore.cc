#include "usersemaphore.h"
#include "system.h"
#include "limits.h"
#include <algorithm>

/*!
 * \brief Initializes the semaphore with the specified value
 * \param semPtr    <- MIPS virtual memory pointer to the semaphore ID
 * \param val       <- initial value for the semaphore
 * \return      0 when semaphore was initialized successfully
 *             -1 when semaphore quota is reached and no more semaphores can be created
 *             -2 when a negative value is passed
 *             -3 attempt to initialize the same semaphore multiple times
 */
int SemaphoreManager::DoSemInit(sem_t *semPtr, int val) {
    int retVal = -1;
    auto firstFreeSemaphore = std::find(m_semaphores.begin(), m_semaphores.end(), nullptr);

    /* cannot allocate more semaphores */
    if (firstFreeSemaphore == m_semaphores.end()) {
        retVal = -1;
        goto error_exit;
    }

    if (val < 0) {
        retVal = -2;
        goto error_exit;
    }

    if (m_initializedSemaphores.find(semPtr) != m_initializedSemaphores.end()) {
        retVal = -3;
        goto error_exit;
    }

    m_initializedSemaphores.insert(semPtr);
    *firstFreeSemaphore = new Semaphore("", val);
    machine->WriteMem((int)semPtr, sizeof(int), (int)std::distance(m_semaphores.begin(), firstFreeSemaphore));
    retVal = 0;

    error_exit:
    return retVal;
}

/*!
 * \brief SemaphoreManager::DoSemPost
 *        Implements a user level post operation
 * \param semPtr <- virtual address of the semaphore ID
 * \return  0 if post operation is performed successfully
 *         -1 if invalid semPtr is specified
 *         -2 if the semaphore would overflow after performing post
 */
int SemaphoreManager::DoSemPost(sem_t *semPtr) {
    int retVal = 0;
    int semIx = getSemaphoreIxByVirtMem(semPtr);
    Semaphore *curSemaphore = nullptr;
    int semRetVal = 0;
    if (semIx < 0) {
        retVal = -1;
        goto early_exit;
    }
    curSemaphore = m_semaphores[semIx];

    semRetVal = curSemaphore->V();
    if (semRetVal == -1) {
        retVal = -2;
        goto early_exit;
    }

    early_exit:
    return retVal;
}

/*!
 * \brief Implements a user-level wait operation on the semaphore
 * \param semPtr    <- MIPS pointer to the semaphore ID
 * \return      0 when wait operation has been successfully performed
 *             -1 if invalid pointer is specified
 */
int SemaphoreManager::DoSemWait(sem_t *semPtr) {
    int retVal = 0;
    int semIx = getSemaphoreIxByVirtMem(semPtr);
    Semaphore *curSemaphore = nullptr;

    if (semIx < 0) {
        retVal = -1;
        goto early_exit;
    }
    curSemaphore = m_semaphores[semIx];

    curSemaphore->P();
    early_exit:
    return retVal;
}

/*!
 * \brief Destroys the semaphore whose address is stored by the parameter
 *        Releases the resources allocated for a kernel-level semaphore
 * \param semPtr    <- MIPS pointer to the semaphore ID
 * \return  0 when the semaphore has been successfully destoryed
 *         -1 when semPtr points to an invalid address
 */
int SemaphoreManager::DoSemDestroy(sem_t *semPtr) {
    int retVal = 0;
    int semIx = getSemaphoreIxByVirtMem(semPtr);
    if (semIx < 0) {
        retVal = -1;
        goto early_exit;
    }

    delete m_semaphores[semIx];
    m_semaphores[semIx] = nullptr;
    m_initializedSemaphores.erase(semPtr);

    early_exit:
    return retVal;
}

SemaphoreManager::SemaphoreManager() {
    std::fill(m_semaphores.begin(), m_semaphores.end(), nullptr);
}

/*!
 * \brief Checks whether the parameter represents an address of a valid semaphore
 * \param semPtr    <- MIPS pointer to the semaphore ID
 * \return     0 if semPtr is valid
 *            -1 otherwise
 */
int SemaphoreManager::getSemaphoreIxByVirtMem(sem_t *semPtr)
{
    int semaphoreIndex = -1;
    machine->ReadMem((int)semPtr, sizeof(int), &semaphoreIndex);

    if ((semaphoreIndex < 0) || (semaphoreIndex >= SEM_VALUE_MAX) || (!m_semaphores[semaphoreIndex])) {
        semaphoreIndex = -1;
        goto early_exit;
    }

    early_exit:
    return semaphoreIndex;
}
