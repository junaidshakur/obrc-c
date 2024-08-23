#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include <pthread.h>
#include <stdbool.h>


#define TOTAL_THREADS 6
#define MIN_TEMP -200
#define MAX_TEMP 200

typedef struct weatherEntry {
  int id;
  short min;
  short max;
  long int rc;
  long int sum;
  long int fileCursorStart;
  long int fileCursorEnd;
} weather;

short get_short_measurement(char *str, int s, int z){
  short val = 0;
  bool ng = 0;
  int i = 0;
  char c1, c2;

  if (str[i] == '-'){
          ng = 1;
          i++;
  }
  c1 = str[i++];
  c2 = str[i++];

  if(c2 == '.'){
    val += (c1-48)*10;
  } else {
    val += (c1-48)*100;
    val += (c2-48)*10;
    i++;
  }
  val += (str[i]-48);

  if (ng)
    val *= -1;

 // if (z < 10)
//        printf("measure final val %d\n", val);

  return val;
}

void readFile_by_full_line(FILE *fp, weather *pData) {

  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  int z = 0;
  long int rc = 0;
  long int sum = 0;
  char *station;
  char *measurementStr;
  short measurement;
  short minMeas = MAX_TEMP, maxMeas = MIN_TEMP;
  char *tokPtr;
  //char *measurementI1, *measurementI2;
  getline(&line, &len, fp);//to move to next stable line instead of reading between the line.
  while ((read = getline(&line, &len, fp)) >=0 && ftell(fp) < pData->fileCursorEnd) {
    if (z < 10){
      z++;
      //printf("complete line and length is %s:%ld\n", line, read);
    }

    //split text by ; and then first part will be station and seocnd will be measurement
    tokPtr=NULL;
    station = strtok_r(line, ";", &tokPtr);
    measurementStr = strtok_r(NULL, "\n", &tokPtr);
    //measurement = strtod(measurementStr, NULL);

    measurement = get_short_measurement(measurementStr, sizeof(measurementStr), z);
    // //measurementI1 = (strtok(NULL, "."));
    // //measurementI2 = (strtok(NULL, "\n"));
    sum += measurement;
    if (measurement > maxMeas)
      maxMeas = measurement;
    if (measurement < minMeas)
      minMeas = measurement;
    rc++;
    //if (z < 10)
    //  printf("station and measurementStr, measurement :: %s:%s:%d\n", station, measurementStr, measurement);
  }

  if (line)
    free(line);

  pData->rc = rc;
  pData->max = maxMeas;
  pData->min = minMeas;
  pData->sum = sum;
  return;
}


void *read_file_chunk(void *args) {

  weather *pData = (weather*)args;

  FILE *fp;
  long int cc, lc;
  fp = fopen("./measurements-1000000000.txt", "r");
  if (fp == NULL)
    exit(EXIT_FAILURE);

  fseek(fp, 0, SEEK_END);
  long int size = ftell(fp);

  pData->fileCursorStart = (size / TOTAL_THREADS) * pData->id;
  pData->fileCursorEnd = (size / TOTAL_THREADS) * (pData->id + 1);
  fseek(fp, pData->fileCursorStart, SEEK_SET);

  readFile_by_full_line(fp, pData);
  //cc = readFile_by_delimeter(fp);

  fclose(fp);
  pthread_exit(NULL);
}

int main() {

  pthread_t thread[TOTAL_THREADS];
  weather pData[TOTAL_THREADS];

  for(int i = 0; i < TOTAL_THREADS; i++){
    pData[i].id = i;
    pData[i].rc = 0;
    pData[i].min = MAX_TEMP;
    pData[i].max = MIN_TEMP;
    pData[i].sum = 0;

    pthread_create(&thread[i], NULL, read_file_chunk, &pData[i]);
  }

  for(int i = 0; i < TOTAL_THREADS; i++){
    pthread_join(thread[i], NULL);
  }

  long int rc=0, sum = 0;
  short min=MAX_TEMP, max=MIN_TEMP;

  for(int i = 0; i < TOTAL_THREADS; i++){
    printf("File cursors of threads %i are %ld, %ld\n", i, pData[i].fileCursorStart, pData[i].fileCursorEnd);
    printf("%d:: Records:%ld:: Sum:%ld:: MIN:%d:: MAX:%d\n", pData[i].id, pData[i].rc, pData[i].sum, pData[i].min, pData[i].max);
    rc += pData[i].rc;
    sum += pData[i].sum;
    if (pData[i].min < min)
      min = pData[i].min;
    if (pData[i].max > max)
      max = pData[i].max;
  }

  printf("\n\nTotal record count, sum, avg, min and max %ld:%ld:%f:%f:%f\n", rc, sum, (sum/(double)rc)/10.0, min/10.0, max/10.0);


  exit(EXIT_SUCCESS);
}
