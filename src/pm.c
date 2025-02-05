#include "pm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define enul(x) ((x)[strlen((x))-1]=0)
pm instance = {0};

int pmLoad(const char *filename){
  FILE *fp = fopen(filename, "rb");
  if(!fp) return -1;
  uint32_t sign = 0;
  uint8_t versionM = 0;
  uint8_t versionm = 0;
  fread(&sign, 3, 1, fp);
  if(sign != 0x6d702e){
    puts("signature missmatch");
    fclose(fp);
    return -2;
  }
  fread(&versionM, 1, 1, fp);
  fread(&versionm, 1, 1, fp);
  if(versionm != 1 || versionM != 0){
    puts("version missmatch");
    fclose(fp);
    return -2;
  }
  fread(&instance.n, 4, 1, fp);
  instance.entries = malloc(sizeof(entry)*instance.n);
  if(!instance.entries){
    puts("buy more ram");
    return -3;
  }
  for(int i = 0; i<instance.n; i++){
    instance.entries[i].name = malloc(MAX_NAMELEN);
    if(!instance.entries[i].name){
      puts("buy more ram");
      return -3;
    }
    fgets(instance.entries[i].name, MAX_NAMELEN, fp);
    enul(instance.entries[i].name);
    fread(&instance.entries[i].npoints, 4, 1, fp);
    instance.entries[i].points = malloc(sizeof(point)*instance.entries[i].npoints);
    if(!instance.entries[i].points){
      puts("buy more ram");
      return -3;
    }
    for(int j = 0; j<instance.entries[i].npoints; j++){
      fread(&instance.entries[i].points[j].status, 1, 1, fp);
      fread(&instance.entries[i].points[j].time, 4, 1, fp);
      fread(&instance.entries[i].points[j].grade, 4, 1, fp);
      fread(&instance.entries[i].points[j].gradeMax, 1, 1, fp);
      instance.entries[i].points[j].description = malloc(MAX_NAMELEN);
      if(!instance.entries[i].points[j].description){
        puts("buy more ram");
        return -3;
      }
      fgets(instance.entries[i].points[j].description, MAX_NAMELEN, fp);
      enul(instance.entries[i].points[j].description);
    }
    fread(&instance.entries[i].time, 4, 1, fp);
    fread(&instance.entries[i].grade, 4, 1, fp);
    fread(&instance.entries[i].gradeMax, 1, 1, fp);
    fread(&instance.entries[i].progress, 1, 1, fp);
  }
  fclose(fp);
  return 0;
}

int pmSave(const char *filename){
  FILE *fp = fopen(filename, "wb");
  if(!fp) return -1;
  //fprintf(fp, "%lx", 0x2e706d0000);
  fputc(0x2e, fp);
  fputc(0x70, fp);
  fputc(0x6d, fp);
  fputc(0, fp);
  fputc(1, fp);
  fwrite(&instance.n, 4, 1, fp);
  for(int i = 0; i<instance.n; i++){
    fputs(instance.entries[i].name, fp);
    fputc('\n', fp);
    fwrite(&instance.entries[i].npoints, 4, 1, fp);
    for(int j = 0; j<instance.entries[i].npoints; j++){
      fwrite(&instance.entries[i].points[j].status, 1, 1, fp);
      fwrite(&instance.entries[i].points[j].time, 4, 1, fp);
      fwrite(&instance.entries[i].points[j].grade, 4, 1, fp);
      fwrite(&instance.entries[i].points[j].gradeMax, 1, 1, fp);
      fputs(instance.entries[i].points[j].description, fp);
      fputc('\n', fp);
    }
    fwrite(&instance.entries[i].time, 4, 1, fp);
    fwrite(&instance.entries[i].grade, 4, 1, fp);
    fwrite(&instance.entries[i].gradeMax, 1, 1, fp);
    fwrite(&instance.entries[i].progress, 1, 1, fp);
  }
  fclose(fp);
  return 0;
}

void addEntry(char *name){
  instance.entries = realloc(instance.entries, ++instance.n * sizeof(entry));
  instance.entries[instance.n-1].name = name;
  instance.entries[instance.n-1].npoints = 0;
  instance.entries[instance.n-1].points = NULL;
  instance.entries[instance.n-1].time = 0;
  instance.entries[instance.n-1].grade = 0;
  instance.entries[instance.n-1].gradeMax = 0;
  instance.entries[instance.n-1].progress = 0;
}

void entryDelete(int i){
  if(!(i+1<=instance.n)) return;
  for(int j = i+1; j<instance.n; j++){
    instance.entries[j-1] = instance.entries[j];
  }
  instance.n--;
}

void pointDelete(int i, int j){
  if(!(j+1<=instance.entries[i].npoints)) return;
  for(int k = j+1; k<instance.entries[i].npoints; k++){
    instance.entries[i].points[k-1] = instance.entries[i].points[k];
  }
  instance.entries[i].npoints--;
}

void addPoint(int entryIndex, char *description){
  instance.entries[entryIndex].points = realloc(instance.entries[entryIndex].points, ++instance.entries[entryIndex].npoints * sizeof(point));
  instance.entries[entryIndex].points[instance.entries[entryIndex].npoints-1].status = 0;
  instance.entries[entryIndex].points[instance.entries[entryIndex].npoints-1].time = 0;
  instance.entries[entryIndex].points[instance.entries[entryIndex].npoints-1].grade = 0;
  instance.entries[entryIndex].points[instance.entries[entryIndex].npoints-1].gradeMax = 0;
  instance.entries[entryIndex].points[instance.entries[entryIndex].npoints-1].description = description;
}

void pmPrint(){
  printf("number of entries: %d\n", instance.n);
  for(int i = 0; i<instance.n; i++){
    printf("entry index: %d\n", i);
    printf("name %s\n", instance.entries[i].name);
    printf("number of points %d\n", instance.entries[i].npoints);
    for(int l = 0; l<instance.entries[i].npoints; l++){
      printf("%d", instance.entries[i].points[l].status);
    }
    putchar('\n');
    for(int j = 0; j<instance.entries[i].npoints; j++){
      printf("\tpoint index: %d\n", j);
      printf("\tstatus: %d\n", instance.entries[i].points[j].status);
      printf("\ttime: %d\n", instance.entries[i].points[j].time);
      printf("\tgrade: %d\n", instance.entries[i].points[j].grade);
      printf("\tgradeMax: %d\n", instance.entries[i].points[j].gradeMax);
      printf("\tdescription: %s\n", instance.entries[i].points[j].description);
    }
    printf("time: %d\n", instance.entries[i].time);
    printf("grade: %d\n", instance.entries[i].grade);
    printf("gradeMax: %d\n", instance.entries[i].gradeMax);
    printf("progress %d\n", instance.entries[i].progress);
  }
}





