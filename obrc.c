#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include <pthread.h>

#define TOTAL_THREADS 6

typedef struct weatherEntry {
  int id;
  double min;
  double max;
  long int rc;
  double sum;
  long int fileCursorStart;
  long int fileCursorEnd;
} weather;



void readFile_by_full_line(FILE *fp, weather *pData) {

  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  int z = 0;
  long int rc = 0;
  double sum = 0.0;
  char *station;
  char *measurementStr;
  double measurement;
  double minMeas = DBL_MAX, maxMeas = DBL_MIN;
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
    measurement = strtod(measurementStr, NULL);

    // //measurementI1 = (strtok(NULL, "."));
    // //measurementI2 = (strtok(NULL, "\n"));
    sum += measurement;
    if (measurement > maxMeas)
      maxMeas = measurement;
    if (measurement < minMeas)
      minMeas = measurement;
    rc++;
    //if (z < 10)
    //  printf("station and measurement :: %s:%s\n", station, measurementStr);
  }

  if (line)
    free(line);

  pData->rc = rc;
  pData->max = maxMeas;
  pData->min = minMeas;
  pData->sum = sum;
  return;
}

int readFile_by_delimeter(FILE *fp) {
  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  int z = 0;
  long long int cc = 0;

  char *station;
  char *measurementStr;
  double measurement;

   while ((read = getdelim(&line, &len, ';', fp)) > 0){
    if (z < 10) z++;
    cc += read;

    station = strtok(line, ";");

    read = getline(&line, &len, fp);
    measurementStr = strtok(line, "\n");
    measurement = atof(measurementStr);
    cc += read;

     if (z < 10)
       printf("station and measurement :: %s:%f\n", station, measurement);

    //split text by ; and then first part will be station and seocnd will be measurement
  }

  if (line)
    free(line);

  return cc;
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
    pData[i].min = 0.0;
    pData[i].max = 0.0;
    pData[i].sum = 0.0;

    pthread_create(&thread[i], NULL, read_file_chunk, &pData[i]);
  }

  for(int i = 0; i < TOTAL_THREADS; i++){
    pthread_join(thread[i], NULL);
  }

  long int rc=0;
  double min=DBL_MAX, max=DBL_MIN, sum = 0;

  for(int i = 0; i < TOTAL_THREADS; i++){
    printf("File cursors of threads %i are %ld, %ld\n", i, pData[i].fileCursorStart, pData[i].fileCursorEnd);
    printf("%d:: Records:%ld:: Sum:%f:: MIN:%f:: MAX:%f\n", pData[i].id, pData[i].rc, pData[i].sum, pData[i].min, pData[i].max);
    rc += pData[i].rc;
    sum += pData[i].sum;
    if (pData[i].min < min)
      min = pData[i].min;
    if (pData[i].max > max)
      max = pData[i].max;
  }

  printf("\n\nTotal record count, sum, avg, min and max %ld:%f:%f:%f:%f\n", rc, sum, sum/(double)rc, min, max);


  exit(EXIT_SUCCESS);
}
