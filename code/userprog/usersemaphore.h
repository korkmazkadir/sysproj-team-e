#ifndef USERSEMAPHORE_H
#define USERSEMAPHORE_H

#include <array>
#include  <set>

#include "semaphore.h"
#include "synch.h"


/*!
 * \class SemaphoreManager
 * \brief Encapsulates the logic for managing user-space semaphores
 *        Application users must provide a sem_t pointer for all semaphore-related operations
 *        The underlying value of the pointer represents an index in the internal \c SemaphoreManager
 *        array structure (array of \c Semaphore)
 */
class SemaphoreManager
{
public:
    SemaphoreManager();
    ~SemaphoreManager() = default;
    SemaphoreManager(const SemaphoreManager &) = delete;
    SemaphoreManager &operator= (const SemaphoreManager &) = delete;

    int DoSemInit(sem_t *semPtr, int val);
    int DoSemPost(sem_t *semPtr);
    int DoSemWait(sem_t *semPtr);
    int DoSemDestroy(sem_t *semPtr);

private:
    int getSemaphoreIxByVirtMem(sem_t *semPtr);

    std::array<Semaphore *, SEM_VALUE_MAX> m_semaphores;
    std::set<sem_t *> m_initializedSemaphores;
};




#endif
