#ifndef ControllerManager_h
#define ControllerManager_h

#include "../Utils/List.h"

class Controller;
class PiLink;
class SensorList;

class ControllerManager {
public:
  ControllerManager();
  ~ControllerManager();

  void addController(Controller *controller);
  void addSensorsToList(SensorList *sensorList);
  void begin();
  void sensorsHasBeenUpdated();
  bool hasMoreThanOne() const { return _controllerList.hasMoreThanOne(); };
  void outputControllers(PiLink *piLink);

protected:
  List<Controller> _controllerList;
};

#endif // ControllerManager_h
