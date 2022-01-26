#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "weatherJson.h"

bool DEBUG_PARSE_FLAG = !true;
bool loadJson = true;

char response[JSON_SIZE] = {0};
char buffer[JSON_SIZE] = {0};
int jsonSize = 0;

void jsonParse(Json* jsonStr, Position* position, Current* currentWeather,
               Minutely* weatherMinutely, Hourly* weatherHourly,Daily* weatherDaily);

void init(Json* jsonStr, Position* position, Current* currentWeather,
          Minutely* weatherMinutely,Hourly* weatherHourly,Daily* weatherDaily) {
  if (loadJson) {
    system("cd ../ && mkdir storage && cd ./storage && mkdir json");
    system("curl "JSON_LINK" > ../storage/json/weatherCurrent.json");
    system("clear");
  }
  freopen("../storage/json/weatherCurrent.json","r",stdin);
  gets(response);
  system("clear");
  fclose(stdin);
  for (int i = 0; response[i] != '\0'; ++i) {
    jsonStr->string[i] = response[i];
  }
  jsonParse(jsonStr, position, currentWeather, weatherMinutely, weatherHourly, weatherDaily);
}

void offFileLoader() {loadJson = false;}
void onFileLoader() {loadJson = true;}

int getJsonSize(Json* jsonStr) {
  if (jsonSize == 0) {
    for (int i = 0; i < sizeof(jsonStr->string) && jsonStr->string[i] != '\0'; ++i) {
      jsonSize++;
    }
  }
  return jsonSize;
}

void clearValues(Position* position, Current* currentWeather, Minutely* weatherMinutely,Hourly* weatherHourly,Daily* weatherDaily) {
  position->lat = 0;
  position->lon = 0;
  position->timezone_offset = 0;
  memset(position->timezone, 0, sizeof(position->timezone));
//  memset(currentWeather->weather_description, 0, sizeof(currentWeather->weather_description));
//  memset(currentWeather->weather_main, 0, sizeof(currentWeather->weather_main));
//  memset(currentWeather->weather_icon, 0, sizeof(currentWeather->weather_icon));
//  currentWeather->temp = 0;
//  currentWeather->clouds = 0;
//  currentWeather->dewPoint = 0;
//  currentWeather->feels_like = 0;
//  currentWeather->humidity = 0;
//  currentWeather->pressure = 0;
//  currentWeather->sunrise_sec = 0;
//  currentWeather->sunset_sec = 0;
//  currentWeather->time_sec = 0;
//  currentWeather->uvi = 0;
//  currentWeather->visibility = 0;
//  currentWeather->weather_id = 0;
//  currentWeather->wind_deg = 0;
//  currentWeather->wind_gust = 0;
//  currentWeather->wind_speed = 0;
}

void jsonParse(Json* jsonStr, Position* position, Current* currentWeather, Minutely* weatherMinutely,Hourly* weatherHourly,Daily* weatherDaily) {
  char keys[4][JSON_HEADERS_SIZE] = {0};
  char values[4][JSON_VALUES_SIZE] = {0};
  int keyWriteIndex = 0, valueWriteIndex = 0;
  bool inArray = false;
  bool readKey = false;
  bool readValue = false;
  bool readNumber = false;
//  -----------------------
  bool fillPositionStruct = true;
  bool fillCurrentStruct = false;
  bool fillMinutelyStruct = false;
  bool fillHourlyStruct = false;
  bool fillDailyStruct = false;
//  -----------------------
  int minutelyCount = 0;
  int hourlyCount = 0;
  int dailyCount = 0;
//  -----------------------
  int cursorPosition = 0;
  int openCurlyBracketsCount = 0;
  int deepLvl = 0;
//  clearValues(position, currentWeather, weatherMinutely, weatherHourly, weatherDaily);
  for (int i = 0; i < getJsonSize(jsonStr) - 3; ++i) {
    buffer[i] = jsonStr->string[i];
    switch (jsonStr->string[i]) {
      case '{':
        openCurlyBracketsCount++;
        deepLvl++;
        valueWriteIndex = 0;
        keyWriteIndex = 0;
        memset(values[deepLvl], 0, sizeof(values[deepLvl]));
        memset(keys[deepLvl], 0, sizeof(keys[deepLvl]));
        break;
      case '}':
        openCurlyBracketsCount--;
        deepLvl--;
        valueWriteIndex = 0;
        keyWriteIndex = 0;
        memset(values[deepLvl], 0, sizeof(values[deepLvl]));
        memset(keys[deepLvl], 0, sizeof(keys[deepLvl]));
        break;
      case '\"':
        if (jsonStr->string[i-1] == ':') {
          if (jsonStr->string[i-2] == '\"') {
            readKey = false;
            readValue = true;
            keyWriteIndex = 0;
          }
        }
        if (jsonStr->string[i-1] == ',' || jsonStr->string[i-1] == '{' || jsonStr->string[i-1] == '[') {
          readKey = true;
          readValue = false;
          valueWriteIndex = 0;
          memset(values[deepLvl], 0, sizeof(values[deepLvl]));
        }
        break;
      case ':':
//        if (jsonStr->string[i + 1] == '{' || jsonStr->string[i + 2] == '{') {deepLvl++;}
        switch (jsonStr->string[i - 1]) {
          case '\"':
            readKey = false;
            keyWriteIndex = 0;
            break;
        }
        break;
      case ',':
        readValue = false;
        readNumber = false;
        keyWriteIndex = 0;
        valueWriteIndex = 0;
        memset(values[deepLvl], 0, sizeof(values[deepLvl]));
        memset(keys[deepLvl], 0, sizeof(keys[deepLvl]));
        break;
      case '[':
        inArray = true;
        readKey = false;
        readValue = false;
        valueWriteIndex = 0;
        keyWriteIndex = 0;
        memset(values[deepLvl], 0, sizeof(values[deepLvl]));
        memset(keys[deepLvl], 0, sizeof(keys[deepLvl]));
        break;
      default:
        if ((jsonStr->string[i] >= 48 && jsonStr->string[i] <= 57) || (jsonStr->string[i] == 46 && jsonStr->string[i+1] >= 48 && jsonStr->string[i+1] <= 57)) {
          readNumber = true;
        }
    }
    if (DEBUG_PARSE_FLAG) {
      printf("index (%d) -> %d %d %d %d %d \n", i, readKey, readValue, readNumber, inArray, deepLvl);
      printf("index (%d) -> %s\n", i, buffer);
    }
    if (readKey) {
      keys[deepLvl][keyWriteIndex] = jsonStr->string[i];
      keyWriteIndex++;
    }
    if (readValue) {
      values[deepLvl][valueWriteIndex] = jsonStr->string[i];
      valueWriteIndex++;
    }
    if (readNumber) {
      values[deepLvl][valueWriteIndex] = jsonStr->string[i];
      valueWriteIndex++;
    }
    if (jsonStr->string[i+1] == ',' || jsonStr->string[i+1] == '}') {
      if (fillPositionStruct) {
        if ((position->lat == 0) && (strcmp(keys[deepLvl], "\"lat\"")==0)) {
          position->lat = strtof(values[deepLvl], NULL);
        } else if ((position->lon == 0) && (strcmp(keys[deepLvl], "\"lon\"")==0)) {
          position->lon = strtof(values[deepLvl], NULL);
        } else if ((position->timezone[0] == '\0') && (strcmp(keys[deepLvl], "\"timezone\"")==0)) {
          strcpy(position->timezone, &values[deepLvl][0]);
        } else if ((position->timezone_offset == 0) && (strcmp(keys[deepLvl], "\"timezone_offset\"")==0)) {
          position->timezone_offset = (int)strtol(values[deepLvl], NULL, 10);
          fillPositionStruct = false; fillCurrentStruct = true;
        }
      } else if (fillCurrentStruct) {
        if ((currentWeather->time_sec == 0) && (strcmp(keys[deepLvl], "\"dt\"")==0)) {
          currentWeather->time_sec = strtol(values[deepLvl], NULL, 10);
        } else if ((currentWeather->sunrise_sec == 0) && (strcmp(keys[deepLvl], "\"sunrise\"")==0)) {
          currentWeather->sunrise_sec = strtol(values[deepLvl], NULL, 10);
        } else if ((currentWeather->sunset_sec == 0) && (strcmp(keys[deepLvl], "\"sunset\"")==0)) {
          currentWeather->sunset_sec = strtol(values[deepLvl], NULL, 10);
        } else if ((currentWeather->temp == 0) && (strcmp(keys[deepLvl], "\"temp\"")==0)) {
          currentWeather->temp = strtof(values[deepLvl], NULL);
        } else if ((currentWeather->feels_like == 0) && (strcmp(keys[deepLvl], "\"feels_like\"")==0)) {
          currentWeather->feels_like = strtof(values[deepLvl], NULL);
        } else if ((currentWeather->pressure == 0) && (strcmp(keys[deepLvl], "\"pressure\"")==0)) {
          currentWeather->pressure = (int)strtol(values[deepLvl], NULL, 10);
        } else if ((currentWeather->humidity == 0) && (strcmp(keys[deepLvl], "\"humidity\"")==0)) {
          currentWeather->humidity = (int)strtol(values[deepLvl], NULL, 10);
        } else if ((currentWeather->dewPoint == 0) && (strcmp(keys[deepLvl], "\"dew_point\"")==0)) {
          currentWeather->dewPoint = strtof(values[deepLvl], NULL);
        } else if ((currentWeather->uvi == 0) && (strcmp(keys[deepLvl], "\"uvi\"")==0)) {
          currentWeather->uvi = (int)strtol(values[deepLvl], NULL, 10);
        } else if ((currentWeather->clouds == 0) && (strcmp(keys[deepLvl], "\"clouds\"")==0)) {
          currentWeather->clouds = (int)strtol(values[deepLvl], NULL, 10);
        } else if ((currentWeather->visibility == 0) && (strcmp(keys[deepLvl], "\"visibility\"")==0)) {
          currentWeather->visibility = (int)strtol(values[deepLvl], NULL, 10);
        } else if ((currentWeather->wind_speed == 0) && (strcmp(keys[deepLvl], "\"wind_speed\"")==0)) {
          currentWeather->wind_speed = strtof(values[deepLvl], NULL);
        } else if ((currentWeather->wind_deg == 0) && (strcmp(keys[deepLvl], "\"wind_deg\"")==0)) {
          currentWeather->wind_deg = (int)strtol(values[deepLvl], NULL, 10);
        } else if ((currentWeather->wind_gust == 0) && (strcmp(keys[deepLvl], "\"wind_gust\"")==0)) {
          currentWeather->wind_gust = strtof(values[deepLvl], NULL);
        } else if ((currentWeather->weather_id == 0) && (strcmp(keys[deepLvl], "\"id\"")==0)) {
          currentWeather->weather_id = (int)strtol(values[deepLvl], NULL, 10);
        } else if ((currentWeather->weather_main[0] == '\0') && (strcmp(keys[deepLvl], "\"main\"")==0)) {
          for (int j = dailyCount*JSON_TXT_FIELD_SIZE, k=0; values[deepLvl][k] != '\0'; ++j, ++k) {currentWeather->weather_main[j] = values[deepLvl][k];}
        } else if ((currentWeather->weather_description[0] == '\0') && (strcmp(keys[deepLvl], "\"description\"")==0)) {
          for (int j = dailyCount*JSON_TXT_FIELD_SIZE, k=0; values[deepLvl][k] != '\0'; ++j, ++k) {currentWeather->weather_description[j] = values[deepLvl][k];}
        } else if ((currentWeather->weather_icon[0] == '\0') && (strcmp(keys[deepLvl], "\"icon\"")==0)) {
          for (int j = dailyCount*JSON_TXT_FIELD_SIZE, k=0; values[deepLvl][k] != '\0'; ++j, ++k) {currentWeather->weather_icon[j] = values[deepLvl][k];}
          fillCurrentStruct = false;
          fillMinutelyStruct = true;
        }
      } else if (fillMinutelyStruct) {
        if ((weatherMinutely->time_sec[minutelyCount] == 0) && (strcmp(keys[deepLvl], "\"dt\"")==0)) {
          weatherMinutely->time_sec[minutelyCount] = strtol(values[deepLvl], NULL, 10);
        } else if ((weatherMinutely->precipitation[minutelyCount] == 0) && (strcmp(keys[deepLvl], "\"precipitation\"")==0)) {
          weatherMinutely->precipitation[minutelyCount] = (int)strtol(values[deepLvl], NULL, 10);
          minutelyCount++;
        } if (minutelyCount == 61) {
          fillMinutelyStruct = false;
          fillHourlyStruct = true;
        }
      } else if (fillHourlyStruct) {
        if ((weatherHourly->time_sec[hourlyCount] == 0) && (strcmp(keys[deepLvl], "\"dt\"")==0)) {
          weatherHourly->time_sec[hourlyCount] = strtol(values[deepLvl], NULL, 10);
        }else if ((weatherHourly->temp[hourlyCount] == 0) && (strcmp(keys[deepLvl], "\"temp\"")==0)) {
          weatherHourly->temp[hourlyCount] = strtof(values[deepLvl], NULL);
        } else if ((weatherHourly->feels_like[hourlyCount] == 0) && (strcmp(keys[deepLvl], "\"feels_like\"")==0)) {
          weatherHourly->feels_like[hourlyCount] = strtof(values[deepLvl], NULL);
        } else if ((weatherHourly->pressure[hourlyCount] == 0) && (strcmp(keys[deepLvl], "\"pressure\"")==0)) {
          weatherHourly->pressure[hourlyCount] = (int)strtol(values[deepLvl], NULL, 10);
        } else if ((weatherHourly->humidity[hourlyCount] == 0) && (strcmp(keys[deepLvl], "\"humidity\"")==0)) {
          weatherHourly->humidity[hourlyCount] = (int)strtol(values[deepLvl], NULL, 10);
        } else if ((weatherHourly->dewPoint[hourlyCount] == 0) && (strcmp(keys[deepLvl], "\"dew_point\"")==0)) {
          weatherHourly->dewPoint[hourlyCount] = strtof(values[deepLvl], NULL);
        } else if ((weatherHourly->uvi[hourlyCount] == 0) && (strcmp(keys[deepLvl], "\"uvi\"")==0)) {
          weatherHourly->uvi[hourlyCount] = (int)strtol(values[deepLvl], NULL, 10);
        } else if ((weatherHourly->clouds[hourlyCount] == 0) && (strcmp(keys[deepLvl], "\"clouds\"")==0)) {
          weatherHourly->clouds[hourlyCount] = (int)strtol(values[deepLvl], NULL, 10);
        } else if ((weatherHourly->visibility[hourlyCount] == 0) && (strcmp(keys[deepLvl], "\"visibility\"")==0)) {
          weatherHourly->visibility[hourlyCount] = (int)strtol(values[deepLvl], NULL, 10);
        } else if ((weatherHourly->wind_speed[hourlyCount] == 0) && (strcmp(keys[deepLvl], "\"wind_speed\"")==0)) {
          weatherHourly->wind_speed[hourlyCount] = strtof(values[deepLvl], NULL);
        } else if ((weatherHourly->wind_deg[hourlyCount] == 0) && (strcmp(keys[deepLvl], "\"wind_deg\"")==0)) {
          weatherHourly->wind_deg[hourlyCount] = (int)strtol(values[deepLvl], NULL, 10);
        } else if ((weatherHourly->wind_gust[hourlyCount] == 0) && (strcmp(keys[deepLvl], "\"wind_gust\"")==0)) {
          weatherHourly->wind_gust[hourlyCount] = strtof(values[deepLvl], NULL);
        } else if ((weatherHourly->weather_id[hourlyCount] == 0) && (strcmp(keys[deepLvl], "\"id\"")==0)) {
          weatherHourly->weather_id[hourlyCount] = (int)strtol(values[deepLvl], NULL, 10);
        } else if ((weatherHourly->weather_main[hourlyCount] == '\0') && (strcmp(keys[deepLvl], "\"main\"")==0)) {
          for (int j = dailyCount*JSON_TXT_FIELD_SIZE, k=0; values[deepLvl][k] != '\0'; ++j, ++k) {weatherHourly->weather_main[j] = values[deepLvl][k];}
        } else if ((weatherHourly->weather_description[hourlyCount] == '\0') && (strcmp(keys[deepLvl], "\"description\"")==0)) {
          for (int j = dailyCount*JSON_TXT_FIELD_SIZE, k=0; values[deepLvl][k] != '\0'; ++j, ++k) {weatherHourly->weather_description[j] = values[deepLvl][k];}
        } else if ((weatherHourly->weather_icon[hourlyCount] == '\0') && (strcmp(keys[deepLvl], "\"icon\"")==0)) {
          for (int j = dailyCount*JSON_TXT_FIELD_SIZE, k=0; values[deepLvl][k] != '\0'; ++j, ++k) {weatherHourly->weather_icon[j] = values[deepLvl][k];}
        } else if ((weatherHourly->pop[hourlyCount] == 0) && (strcmp(keys[deepLvl], "\"pop\"")==0)) {
          weatherHourly->pop[hourlyCount] = strtof(values[deepLvl], NULL);
          hourlyCount++;
          if (hourlyCount == 48) {
            fillHourlyStruct = false;
            fillDailyStruct = true;
          }
        }
      } else if (fillDailyStruct) {
        if ((weatherDaily->time_sec[dailyCount] == 0) && (strcmp(keys[deepLvl], "\"dt\"")==0)) {
          weatherDaily->time_sec[dailyCount] = strtol(values[deepLvl], NULL, 10);
        } else if ((weatherDaily->sunrise_sec[dailyCount] == 0) && (strcmp(keys[deepLvl], "\"sunrise\"")==0)) {
          weatherDaily->sunrise_sec[dailyCount] = strtol(values[deepLvl], NULL, 10);
        } else if ((weatherDaily->sunset_sec[dailyCount] == 0) && (strcmp(keys[deepLvl], "\"sunset\"")==0)) {
          weatherDaily->sunset_sec[dailyCount] = strtol(values[deepLvl], NULL, 10);
        } else if ((weatherDaily->moonrise_sec[dailyCount] == 0) && (strcmp(keys[deepLvl], "\"moonrise\"")==0)) {
          weatherDaily->moonrise_sec[dailyCount] = strtol(values[deepLvl], NULL, 10);
        } else if ((weatherDaily->moonset_sec[dailyCount] == 0) && (strcmp(keys[deepLvl], "\"moonset\"")==0)) {
          weatherDaily->moonset_sec[dailyCount] = strtol(values[deepLvl], NULL, 10);
        } else if ((weatherDaily->moon_phase[dailyCount] == 0) && (strcmp(keys[deepLvl], "\"moon_phase\"")==0)) {
          weatherDaily->moon_phase[dailyCount] = strtof(values[deepLvl], NULL);
        } else if ((weatherDaily->temp_day[dailyCount] == 0) && (strcmp(keys[deepLvl-1], "\"temp\"")==0) && (strcmp(keys[deepLvl], "\"day\"")==0)) {
          weatherDaily->temp_day[dailyCount] = strtof(values[deepLvl], NULL);
        } else if ((weatherDaily->temp_min[dailyCount] == 0) && (strcmp(keys[deepLvl-1], "\"temp\"")==0) && (strcmp(keys[deepLvl], "\"min\"")==0)) {
          weatherDaily->temp_min[dailyCount] = strtof(values[deepLvl], NULL);
        } else if ((weatherDaily->temp_max[dailyCount] == 0) && (strcmp(keys[deepLvl-1], "\"temp\"")==0) && (strcmp(keys[deepLvl], "\"max\"")==0)) {
          weatherDaily->temp_max[dailyCount] = strtof(values[deepLvl], NULL);
        } else if ((weatherDaily->temp_night[dailyCount] == 0) && (strcmp(keys[deepLvl-1], "\"temp\"")==0) && (strcmp(keys[deepLvl], "\"night\"")==0)) {
          weatherDaily->temp_night[dailyCount] = strtof(values[deepLvl], NULL);
        } else if ((weatherDaily->temp_eve[dailyCount] == 0) && (strcmp(keys[deepLvl-1], "\"temp\"")==0) && (strcmp(keys[deepLvl], "\"eve\"")==0)) {
          weatherDaily->temp_eve[dailyCount] = strtof(values[deepLvl], NULL);
        } else if ((weatherDaily->temp_morn[dailyCount] == 0) && (strcmp(keys[deepLvl-1], "\"temp\"")==0) && (strcmp(keys[deepLvl], "\"morn\"")==0)) {
          weatherDaily->temp_morn[dailyCount] = strtof(values[deepLvl], NULL);
        } else if ((weatherDaily->temp_day[dailyCount] == 0) && (strcmp(keys[deepLvl-1], "\"feels_like\"")==0) && (strcmp(keys[deepLvl], "\"day\"")==0)) {
          weatherDaily->feels_like_day[dailyCount] = strtof(values[deepLvl], NULL);
        } else if ((weatherDaily->temp_night[dailyCount] == 0) && (strcmp(keys[deepLvl-1], "\"feels_like\"")==0) && (strcmp(keys[deepLvl], "\"night\"")==0)) {
          weatherDaily->feels_like_night[dailyCount] = strtof(values[deepLvl], NULL);
        } else if ((weatherDaily->temp_eve[dailyCount] == 0) && (strcmp(keys[deepLvl-1], "\"feels_like\"")==0) && (strcmp(keys[deepLvl], "\"eve\"")==0)) {
          weatherDaily->feels_like_eve[dailyCount] = strtof(values[deepLvl], NULL);
        } else if ((weatherDaily->temp_morn[dailyCount] == 0) && (strcmp(keys[deepLvl-1], "\"feels_like\"")==0) && (strcmp(keys[deepLvl], "\"morn\"")==0)) {
          weatherDaily->feels_like_morn[dailyCount] = strtof(values[deepLvl], NULL);
        } else if ((weatherDaily->pressure[dailyCount] == 0) && (strcmp(keys[deepLvl], "\"pressure\"")==0)) {
          weatherDaily->pressure[dailyCount] = (int)strtol(values[deepLvl], NULL, 10);
        } else if ((weatherDaily->humidity[dailyCount] == 0) && (strcmp(keys[deepLvl], "\"humidity\"")==0)) {
          weatherDaily->humidity[dailyCount] = (int)strtol(values[deepLvl], NULL, 10);
        } else if ((weatherDaily->dew_point[dailyCount] == 0) && (strcmp(keys[deepLvl], "\"dew_point\"")==0)) {
          weatherDaily->dew_point[dailyCount] = strtof(values[deepLvl], NULL);
        } else if ((weatherDaily->wind_speed[dailyCount] == 0) && (strcmp(keys[deepLvl], "\"wind_speed\"")==0)) {
          weatherDaily->wind_speed[dailyCount] = strtof(values[deepLvl], NULL);
        } else if ((weatherDaily->wind_deg[dailyCount] == 0) && (strcmp(keys[deepLvl], "\"wind_deg\"")==0)) {
          weatherDaily->wind_deg[dailyCount] = (int)strtol(values[deepLvl], NULL, 10);
        } else if ((weatherDaily->wind_gust[dailyCount] == 0) && (strcmp(keys[deepLvl], "\"wind_gust\"")==0)) {
          weatherDaily->wind_gust[dailyCount] = strtof(values[deepLvl], NULL);
        } else if ((weatherDaily->weather_id[dailyCount] == 0) && (strcmp(keys[deepLvl], "\"id\"")==0)) {
          weatherDaily->weather_id[dailyCount] = (int)strtol(values[deepLvl], NULL, 10);
        } else if ((weatherDaily->weather_main[dailyCount*JSON_TXT_FIELD_SIZE] == '\0') && (strcmp(keys[deepLvl], "\"main\"")==0)) {
          for (int j = dailyCount*JSON_TXT_FIELD_SIZE, k=0; values[deepLvl][k] != '\0'; ++j, ++k) {weatherDaily->weather_main[j] = values[deepLvl][k];}
        } else if ((weatherDaily->weather_description[dailyCount*JSON_TXT_FIELD_SIZE] == '\0') && (strcmp(keys[deepLvl], "\"description\"")==0)) {
          for (int j = dailyCount*JSON_TXT_FIELD_SIZE, k=0; values[deepLvl][k] != '\0'; ++j, ++k) {weatherDaily->weather_description[j] = values[deepLvl][k];}
        } else if ((weatherDaily->weather_icon[dailyCount*JSON_TXT_FIELD_SIZE] == '\0') && (strcmp(keys[deepLvl], "\"icon\"")==0)) {
          for (int j = dailyCount*JSON_TXT_FIELD_SIZE, k=0; values[deepLvl][k] != '\0'; ++j, ++k) {weatherDaily->weather_icon[j] = values[deepLvl][k];}
        } else if ((weatherDaily->clouds[dailyCount] == 0) && (strcmp(keys[deepLvl], "\"clouds\"")==0)) {
          weatherDaily->clouds[dailyCount] = (int)strtol(values[deepLvl], NULL, 10);
        } else if ((weatherDaily->pop[dailyCount] == 0) && (strcmp(keys[deepLvl], "\"pop\"")==0)) {
          weatherDaily->pop[dailyCount] = strtof(values[deepLvl], NULL);
        } else if ((weatherDaily->snow[dailyCount] == 0) && (strcmp(keys[deepLvl], "\"snow\"")==0)) {
          weatherDaily->snow[dailyCount] = strtof(values[deepLvl], NULL);
        } else if ((weatherDaily->uvi[dailyCount] == 0) && (strcmp(keys[deepLvl], "\"uvi\"")==0)) {
          weatherDaily->uvi[dailyCount] = strtof(values[deepLvl], NULL);
          dailyCount++;
          if (dailyCount == 8) {fillDailyStruct = false;}
        }
      }
    }
    if ((jsonStr->string[i+1] == ',' || jsonStr->string[i+1] == '}') && (values[deepLvl][0] != '\0' && keys[deepLvl][0] != '\0')) {
//      printf("key1: %s key2: %s key3: %s key4: %s \nvalue: %s\n", keys[0], keys[1], keys[2], keys[3], values[deepLvl]);
      memset(values[deepLvl], 0, sizeof(values[deepLvl]));
      memset(keys[deepLvl], 0, sizeof(keys[deepLvl]));
    }
  }
}
