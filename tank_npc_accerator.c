// http://blog.livedoor.jp/akf0/archives/51585502.html

#include <stdio.h>
#include <winsock2.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

typedef struct
{
  int x;
  int y;
  char *name;
} Position;

int attackByY(SOCKET s, int y)
{
  char cannon[20];
  char buffer[1024];
  memset(cannon, '\0', sizeof(cannon));
  sprintf(cannon, "cannon:%d\n", y);
  send(s, cannon, strlen(cannon), 0);
  memset(buffer, '\0', sizeof(buffer));
  recv(s, buffer, sizeof(buffer), 0);
  printf("attack: %s", buffer);
  char *p = strtok(buffer, ":");
  p = strtok(NULL, ":");
  // printf("strcmp %d\n", strcmp(p, "null"));
  if (strcmp(p, "null") == 1)
  {
    return 2;
  }
  else
  {
    return 1;
  }
}

void goToY(SOCKET s, int y)
{
  char move[20];
  char buffer[1024];
  memset(move, '\0', sizeof(move));
  sprintf(move, "move:%d\n", y);
  send(s, move, strlen(move), 0);
  memset(buffer, '\0', sizeof(buffer));
  recv(s, buffer, sizeof(buffer), 0);
  // printf("%s", buffer);
}

void search(SOCKET s, int y, Position *pos)
{
  char search[20];
  char buffer[1024];
  memset(search, '\0', sizeof(search));
  memset(buffer, '\0', sizeof(buffer));
  sprintf(search, "search:%d\n", y);
  send(s, search, strlen(search), 0);
  recv(s, buffer, sizeof(buffer), 0);
  // printf("%s", buffer);
  char *p = strtok(buffer, ":");
  p = strtok(NULL, ":");
  // printf("strcmp %d\n", strcmp(p, "null"));
  if (strcmp(p, "null") == 1)
  {
    pos->x = 0;
    pos->y = 0;
    pos->name = "null";
  }
  else
  {
    char *ps = strtok(p, ",");
    pos->x = atoi(ps);
    p = strtok(NULL, ",");
    pos->y = atoi(p);
    p = strtok(NULL, ",");
    p[strlen(p) - 1] = '\0';
    pos->name = p;
  }
  // printf("%d: x: %d, y: %d, name: %s\n", y, pos->x, pos->y, pos->name);
}

int bestHeight(SOCKET s, int my_cannon)
{
  int i = 0;
  Position pos;
  Position *p = &pos;

  for (i = 0; i < 11; i++)
  {
    search(s, i * 100, p);
    if (strcmp(p->name, "enemy") == 0 && my_cannon >= 3)
    {
      attackByY(s, p->y);
      printf("shot!! Cneter p->y: %d\n", p->y);
      return p->y;
    }
  }
  return 0;
}

int nextCannon(SOCKET s, int y, int flag_cannon)
{
  int i = 0;
  Position pos;
  Position *p = &pos;
  int y_h = 0;
  int y_l = 0;
  int attack_flag = 0;
  if (y < 200)
  {
    y_l = y;
  }
  else
  {
    y_l = y - 100;
  }
  if (y > 800)
  {
    y_h = y;
  }
  else
  {
    y_h = y + 100;
  }

  if (flag_cannon == 1)
  {
    attack_flag = attackByY(s, y_h);
    printf("attack_flag: %d\n", attack_flag);
    if (attack_flag == 1)
    {
      printf("shot!! Second y_h: %d\n", y_h);
      return 1;
    }
    // printf("shot!! Second y_h: %d\n", y_h);
    // return 1;
  }
  else if (flag_cannon == 2)
  {
    attack_flag = attackByY(s, y_l);
    printf("attack_flag: %d\n", attack_flag);
    if (attack_flag == 1)
    {
      printf("shot!! Third y_l: %d\n", y_l);
      return 1;
    }
    // printf("shot!! Third y_l: %d\n", y_l);
    // return 1;
  }
  return 0;
}

int getMyHp(SOCKET s)
{
  char my[20];
  char buffer[1024];
  memset(my, '\0', sizeof(my));
  memset(buffer, '\0', sizeof(buffer));
  sprintf(my, "state:hp\n");
  send(s, my, strlen(my), 0);
  recv(s, buffer, sizeof(buffer), 0);
  printf("%s", buffer);
  char *p = strtok(buffer, ":");
  p = strtok(NULL, ":");
  return atoi(p);
}

void getMyY(SOCKET s, Position *pos)
{
  char my[20];
  char buffer[1024];
  memset(my, '\0', sizeof(my));
  memset(buffer, '\0', sizeof(buffer));
  sprintf(my, "state:hight\n");
  send(s, my, strlen(my), 0);
  recv(s, buffer, sizeof(buffer), 0);
  // printf("%s", buffer);
  char *p = strtok(buffer, ":");
  p = strtok(NULL, ":");
  pos->x = 0;
  pos->y = atoi(p);
  pos->name = "my";
  // printf("my: x: %d, y: %d, name: %s\n", pos->x, pos->y, pos->name);
}

int getMyCannon(SOCKET s)
{
  char my[20];
  char buffer[1024];
  memset(my, '\0', sizeof(my));
  memset(buffer, '\0', sizeof(buffer));
  sprintf(my, "state:cannon\n");
  send(s, my, strlen(my), 0);
  recv(s, buffer, sizeof(buffer), 0);
  printf("%s", buffer);
  char *p = strtok(buffer, ":");
  p = strtok(NULL, ":");
  return atoi(p);
}

void escape(SOCKET s, int y, int flag_0)
{
  char move[20];
  char buffer[1024];
  if (flag_0)
    y -= 50;
  else
    y += 50;
  memset(move, '\0', sizeof(move));
  sprintf(move, "move:%d\n", y);
  send(s, move, strlen(move), 0);
  memset(buffer, '\0', sizeof(buffer));
  recv(s, buffer, sizeof(buffer), 0);
  // printf("%s", buffer);
}

int main(void)
{

  struct sockaddr_in dest;
  memset(&dest, 0, sizeof(dest));

  char destination[] = "127.0.0.1";
  dest.sin_port = htons(11000);
  dest.sin_family = AF_INET;
  dest.sin_addr.s_addr = inet_addr(destination);

  WSADATA data;
  WSAStartup(MAKEWORD(2, 0), &data);
  SOCKET s = socket(AF_INET, SOCK_STREAM, 0);

  if (connect(s, (struct sockaddr *)&dest, sizeof(dest)))
  {
    printf("%s: was disconnected!!\n", destination);
    return -1;
  }

  char buffer[1024];
  char name[] = "name:AA\n";
  printf("%s: was connected!!\n", destination);
  send(s, name, strlen(name), 0);

  memset(buffer, '\0', sizeof(buffer));
  recv(s, buffer, sizeof(buffer), 0);
  printf("%s\n\n", buffer);

  memset(buffer, '\0', sizeof(buffer));
  recv(s, buffer, sizeof(buffer), 0);
  printf("%s\n\n", buffer);

  time_t t = time(NULL);
  srand(t);

  goToY(s, 100);
  int search_val = 1000;
  Position myPos;
  int flag_0 = 0;
  int my_y = 0;
  int my_hp = 0;
  int my_cannon = 0;
  int pre_my_hp = 1;
  int flag_is_attacked = 0;
  int flag_cannon = 0;
  int next_Y = 0;

  while (1)
  {

    Position pos;
    Position *p = &pos;
    getMyY(s, p);
    my_y = p->y;
    my_hp = getMyHp(s);
    my_cannon = getMyCannon(s);

    if (flag_cannon == 2)
    {
      if (nextCannon(s, next_Y, flag_cannon) == 1)
      {
        flag_cannon = 0;
        next_Y = 0;
      }
    }
    else if (flag_cannon == 1)
    {
      if (nextCannon(s, next_Y, flag_cannon) == 1)
      {
        flag_cannon++;
      }
    }
    else
    {
      next_Y = bestHeight(s, my_cannon);
      if (next_Y != 0)
      {
        flag_cannon++;
      }
    }
    printf("flag_cannon: %d\n", flag_cannon);
    printf("next_Y: %d\n", next_Y);

    if (my_hp < pre_my_hp)
      flag_is_attacked = 1;
    if (flag_is_attacked)
    {
      escape(s, my_y, flag_0);
      flag_is_attacked = 0;
    };
    Position cannonPos;
    Position *cp = &cannonPos;
    search(s, flag_0, cp);
    if (my_y < 150)
    {
      goToY(s, 900);
      flag_0 = 1000;
    }
    else if (my_y > 850)
    {
      goToY(s, 100);
      flag_0 = 0;
    }
    pre_my_hp = my_hp;
  }

  closesocket(s);
  WSACleanup();

  return 0;
}