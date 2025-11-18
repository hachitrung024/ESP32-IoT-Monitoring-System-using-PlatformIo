#ifndef __NPK_SENSOR_H__
#define __NPK_SENSOR_H__
#include "global.h"
#include <ModbusMaster.h>

#define REG_ADDR_NITROGEN_CONTENT            0x001E  // 40031
#define REG_ADDR_PHOSPHORUS_CONTENT          0x001F  // 40032
#define REG_ADDR_POTASSIUM_CONTENT           0x0020  // 40033

#define NPK_SLAVE_ID         1      // ID 

extern ModbusMaster npkNode;
void npk_sensor_task(void * pvParameter);
#endif // __NPK_SENSOR_H__