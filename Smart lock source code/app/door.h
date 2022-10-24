#ifndef SMARTLOCK_APP_DOOR_H
#define SMARTLOCK_APP_DOOR_H

// The main portion of the application code, handling entry and exit of people through the door.

extern unsigned int personCount, personLimit;

void initializeDoorHandler(void);
void tickDoorHandler(void);

#endif // SMARTLOCK_APP_DOOR_H
