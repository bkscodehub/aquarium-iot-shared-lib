#pragma once

#ifndef TIME_UTILS_H
#define TIME_UTILS_H

void setClock();
String getTimestamp();  // Get datetime in UTC
int getHourNow();
int getMinuteNow();
bool isTimeInRange(int onHour, int onMinute, int offHour, int offMinute);

#endif
