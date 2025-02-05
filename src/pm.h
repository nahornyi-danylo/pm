#include <stdint.h>
#define MAX_NAMELEN 256

typedef  struct {
  uint8_t status;
  uint32_t time;
  float grade;
  uint8_t gradeMax;
  char *description;
} point;

typedef struct {
  char *name;
  uint32_t npoints;
  point *points;
  uint32_t time;
  float grade;
  uint8_t gradeMax;
  uint8_t progress;
} entry;

typedef struct {
  uint32_t n;
  entry *entries;
} pm;

int pmLoad(const char *filename);
int pmSave(const char *filename);
void pmPrint();
void addEntry(char *name);
void entryDelete(int i);
void addPoint(int i, char *name);
void pointDelete(int i, int j);
