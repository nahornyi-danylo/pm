#include <ncurses.h>
#include <stdlib.h>
#include <locale.h>
#include <stdio.h>
#include <time.h>
#include "pm.h"

int selector = 0;
time_t t1;
extern pm instance;

int getnstrbreakonesc(char *s, int n){
  char c;
  int x, y;
  for(int i = 0; i<n; i++){
    c = getch();
    switch(c){
      case 27:
        return -1;
      case '\n':
        s[i] = '\0';
        return 0;
      case 127:
        if(i>0){
          i-=2;
          s[i+1] = '\0';
          getyx(stdscr, y, x);
          mvdelch(y, x - 1); 
          mvdelch(y, x - 2);
          mvdelch(y, x - 3);
          refresh();
        }
        break;
      default:
        s[i] = c;
        break;
    }
  }
  s[n-1] = '\0';
  return 0;
}

void pointTime(int i){
  t1 = time(NULL);
  time_t t2;
  int c;
  move(LINES-1, 0);
  addstr("(stopwatch mode) press t to stop");
  do{
    c = getch();
  } while(c!='t');
  t2 = time(NULL);
  move(LINES-1, 0);
  clrtoeol();
  int m = (t2-t1)/60;
  int s = (t2-t1)%60; 
  printw("time elapsed: %dm %ds, (s)ave | (c)ancel", m, s); 
again: c = getch();
  switch(c){
    case 's':
      if(i!=-1) instance.entries[selector].points[i].time += m;      
      instance.entries[selector].time += m;      
      break;
    case 'c':
      break;
    default:
      goto again;
  }
  move(LINES-1, 0);
  clrtoeol();
}

void ensureGrade(){
  instance.entries[selector].gradeMax = 0;
  instance.entries[selector].grade = 0;
  for(int i = 0; i<instance.entries[selector].npoints; i++){
    instance.entries[selector].gradeMax += instance.entries[selector].points[i].gradeMax;
    instance.entries[selector].grade += instance.entries[selector].points[i].grade;
  }
}

void setStatus(int i){
  int stat = 0;
  move(LINES-1, 0);
  addstr("enter:");
  echo();
  char s[10] = {0};
  int check = getnstrbreakonesc(s, 10);
  if(check) goto ret;
  sscanf(s, "%d", &stat);
  instance.entries[selector].points[i].status = (unsigned char)stat;
ret:
  noecho();
  clear();
}

void gradePoint(int i){
  int g = 0;
  move(LINES-1, 0);
  addstr("enter:");
  echo();
  char s[10] = {0};
  int check = getnstrbreakonesc(s, 10);
  if(check) goto ret;
  if(s[0] == 'm'){
    sscanf(s, "%*c%d", &g);
    instance.entries[selector].points[i].gradeMax = g;
  }
  else {
    sscanf(s, "%d", &g);
    instance.entries[selector].points[i].grade = g<=instance.entries[selector].points[i].gradeMax?g:0;
  }
  ensureGrade();
ret:
  clear();
  noecho();
}

void newPoint(){
  move(LINES-1, 0);
  addstr("enter name:");
  echo();
  char *s = malloc(MAX_NAMELEN);
  int check = getnstrbreakonesc(s, MAX_NAMELEN);
  if(!check) addPoint(selector, s);
  else free(s);
  clear();
  noecho();
}

void enterEntry(){
  clear();
  move(0, 0);
  int s = 0;
  int c;
  do{
    for(int i = 0; i<instance.entries[selector].npoints; i++){
      move(i, 0);
      if(i==s){
        attron(COLOR_PAIR(10));
        addstr("->");
        attroff(COLOR_PAIR(10));
      }
      else {
        addstr("  ");
      }
      addnstr(instance.entries[selector].points[i].description, COLS/3);
      move(i, COLS/3+2);
      printw("%d", instance.entries[selector].points[i].status);
      printw(" %d/%d", instance.entries[selector].points[i].grade, instance.entries[selector].points[i].gradeMax);
      printw(" %d:%d", instance.entries[selector].points[i].time/60, instance.entries[selector].points[i].time%60);
    }
    c = getch();
    switch(c){
      case 'j':
        if(!instance.entries[selector].npoints) break;
        s++;
        if(s == instance.entries[selector].npoints) s = 0;
        break;
      case 'k':
        if(!instance.entries[selector].npoints) break;
        s--;
        if(s<0) s = instance.entries[selector].npoints-1;
        break;
      case 'a':
        newPoint();
        clear();
        break;
      case 'd':
        pointDelete(selector, s);
        s = 0;
        clear();
        break;
      case 'g':
        gradePoint(s);
        break;
      case 's':
        setStatus(s);
        break;
      case 't':
        pointTime(s);
        break;
    }
  } while (c!='q');
}


void newEntry(){
  move(LINES-1, 0);
  addstr("enter name:");
  echo();
  char *s = malloc(MAX_NAMELEN);
  int check = getnstrbreakonesc(s, MAX_NAMELEN);
  if(!check) addEntry(s);
  else free(s);
  clear();
  noecho();
}


int main(int argc, char **argv){
  if(argc != 2){
    puts("provide a filename or fuck off");
    return -1;
  }
  pmLoad(argv[1]);
  pmSave("backup.pm");
  setlocale(LC_ALL, "");
  initscr();
  cbreak();
  start_color();
  //init_color(11, 0, 0, 1000);
  //init_color(17, 0, 1000, 0);
  //init_color(10, 0, 1000, 0);
  init_pair(10, 165, 0);
  init_pair(1, 0, 196);
  init_pair(2, 0, 46);
  init_pair(3, 0, 190);
  noecho();
  int c;
  do{
    for(int i = 0; i<instance.n; i++){
      move(i, 0);
      if(i==selector){
        attron(COLOR_PAIR(10));
        addstr("->");
        attroff(COLOR_PAIR(10));
      }
      else {
        addstr("  ");
      }
      addnstr(instance.entries[i].name, COLS/3);
      move(i, COLS/3);
      for(int j = 0; j<COLS/3-2; j++){
        if(j<instance.entries[i].npoints){
          //printw("%d", instance.entries[i].points[j].status);
          attron(COLOR_PAIR(instance.entries[i].points[j].status+1));
          addch('#');
          attroff(COLOR_PAIR(instance.entries[i].points[j].status+1));
        }
        else addch(' ');
      }
      printw("%d/%d", instance.entries[i].grade, instance.entries[i].gradeMax);
      move(i, COLS*7/9);
      printw("%d", instance.entries[i].progress);
      move(i, COLS*8/9);
      printw("%d:%d", instance.entries[i].time/60, instance.entries[i].time%60);
    }
    c = getch();
    switch(c){
      case 'j':
        if(!instance.n) break;
        selector++;
        if(selector == instance.n) selector = 0;
        break;
      case 'k':
        if(!instance.n) break;
        selector--;
        if(selector<0) selector = instance.n-1;
        break;
      case 'e':
        enterEntry();
        clear();
        break;
      case 'a':
        newEntry();
        break;
      case 'd':
        entryDelete(selector);
        selector = 0;
        clear();
        break;
      case 't':
        pointTime(-1);
        break;
      case 's':
        pmSave(argv[1]);
        break;
    }
  } while(c!='q');

  endwin();
}
