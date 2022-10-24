#ifndef SMARTLOCK_COMPONENTS_LOCK_H
#define SMARTLOCK_COMPONENTS_LOCK_H

// Functions for controlling the lock.

#include <stdbool.h>

void initializeLockControl(void);
void setLockState(const bool open);
bool getLockState(void);

#endif // SMARTLOCK_COMPONENTS_LOCK_H
