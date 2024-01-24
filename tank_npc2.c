//http://blog.livedoor.jp/akf0/archives/51585502.html

#include <stdio.h>
#include <winsock2.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

typedef struct {
    int x;
    int y;
    char *name;
} Position;

void attackToY(SOCKET s, int y) {
    // printf("attackToY: %d\n", y);
    char cannon[20];
    char buffer[1024];
    memset(cannon, '\0', sizeof(cannon));
    sprintf(cannon, "cannon:%d\n", y);
    send(s, cannon, strlen(cannon), 0);
    memset(buffer, '\0', sizeof(buffer));
	recv(s, buffer, sizeof(buffer), 0);
}

void goToY(SOCKET s, int y) {
    // printf("goToY: %d\n", y);
    char move[20];
    char buffer[1024];
    memset(move, '\0', sizeof(move));
    sprintf(move, "move:%d\n", y);
    send(s, move, strlen(move), 0);
    memset(buffer, '\0', sizeof(buffer));
	recv(s, buffer, sizeof(buffer), 0);
}

void search(SOCKET s, int y, Position *pos) {
    // printf("search: %d\n", y);
    char search[20];
    char buffer[1024];
    memset(search, '\0', sizeof(search));
    memset(buffer, '\0', sizeof(buffer));
    sprintf(search, "search:%d\n", y);
    send(s, search, strlen(search), 0);
    recv(s, buffer, sizeof(buffer), 0);
    char *p = strtok(buffer, ":");
    p = strtok(NULL, ":");
    if (strcmp(p, "null") == 1) {
        pos->x = 0;
        pos->y = 0;
        pos->name = "null";
    } else {
        char *ps = strtok(p, ",");
        pos->x = atoi(ps);
        p = strtok(NULL, ",");
        pos->y = atoi(p);
        p = strtok(NULL, ",");
        p[strlen(p) - 1] = '\0';
        pos->name = p;
    }
}

void attackToEnemy(SOCKET s) {
    // printf("attackToEnemy\n");
    int i = 0;
    Position pos;
    Position* p = &pos;
    for (i = 1; i < 10; i++) {
        search(s, i * 100, p);
        if (strcmp(p -> name, "enemy") == 0) {
            attackToY(s, p -> y);
            break;
        }
    }
}

int searchEnemy(SOCKET s) {
    // printf("searchEnemy\n");
    int i = 0;
    Position pos;
    Position* p = &pos;
    for (i = 1; i < 10; i++) {
        search(s, i * 100, p);
        if (strcmp(p -> name, "enemy") == 0) {
            return p -> y;
        }
    }
    return -1;
}

int getMyHp(SOCKET s) {
    // printf("getMyHp\n");
    char my[20];
    char buffer[1024];
    memset(my, '\0', sizeof(my));
    memset(buffer, '\0', sizeof(buffer));
    sprintf(my, "state:hp\n");
    send(s, my, strlen(my), 0);
    recv(s, buffer, sizeof(buffer), 0);
    char *p = strtok(buffer, ":");
    p = strtok(NULL, ":");
    return atoi(p);
}

int getMyY(SOCKET s) {
    // printf("getMyY\n");
    char my[20];
    char buffer[1024];
    memset(my, '\0', sizeof(my));
    memset(buffer, '\0', sizeof(buffer));
    sprintf(my, "state:hight\n");
    send(s, my, strlen(my), 0);
    recv(s, buffer, sizeof(buffer), 0);
    char *p = strtok(buffer, ":");
    p = strtok(NULL, ":");
    return atoi(p);
}


void escape(SOCKET s, int y, int destination) {
    // printf("escape: %d\n", y);
    char move[20];
    char buffer[1024];
    if (destination) y -= 50;
    else y += 50;
    memset(move, '\0', sizeof(move));
    sprintf(move, "move:%d\n", y);
    send(s, move, strlen(move), 0);
    memset(buffer, '\0', sizeof(buffer));
    recv(s, buffer, sizeof(buffer), 0);
    printf("%s", buffer);
}

int getCannonOnMyLine(SOCKET s, int y) {
    // printf("getCannonOnMyLine: %d\n", y);
    char search[20];
    char buffer[1024];
    memset(search, '\0', sizeof(search));
    memset(buffer, '\0', sizeof(buffer));
    sprintf(search, "search:%d\n", y);
    send(s, search, strlen(search), 0);
    recv(s, buffer, sizeof(buffer), 0);
    char *p = strtok(buffer, ":");
    p = strtok(NULL, ":");
    if (strcmp(p, "null") == 1) {
        return -100;
    } else {
        char *ps = strtok(p, ",");
        ps = strtok(NULL, ",");
        return atoi(ps);
    }
}

int judgeStop(int c_y, int m_y, int i_a, int d) {
    int distance = abs(c_y - m_y);
    printf("c: %d, my: %d, dis: %d, des:%d, atk: %d\n", c_y, m_y, distance, d, i_a);
    if (d) {
        if (c_y < m_y) return 0;
    } else {
        if (c_y > m_y) return 0;
    }
    if (i_a) return 0;
    if (distance > 90) return 0;
    return 1;
}

int getMyCannon(SOCKET s) {
    // printf("getMyCannon\n");
    char my[20];
    char buffer[1024];
    memset(my, '\0', sizeof(my));
    memset(buffer, '\0', sizeof(buffer));
    sprintf(my, "state:cannon\n");
    send(s, my, strlen(my), 0);
    recv(s, buffer, sizeof(buffer), 0);
    char *p = strtok(buffer, ":");
    p = strtok(NULL, ":");
    return atoi(p);
}

int main(void) {

	struct sockaddr_in dest;
	memset(&dest, 0, sizeof(dest));

	char destination[] = "127.0.0.1";
	dest.sin_port = htons(11000);
	dest.sin_family = AF_INET;
	dest.sin_addr.s_addr = inet_addr(destination);


	WSADATA data;
	WSAStartup(MAKEWORD(2, 0), &data);
	SOCKET s = socket(AF_INET, SOCK_STREAM, 0);

	if (connect(s, (struct sockaddr *) &dest, sizeof(dest))) {
		printf("%s: was disconnected!!\n", destination);
		return -1;
	}

    char buffer[1024];
    char name[] = "name:3attack\n";
	printf("%s: was connected!!\n", destination);
	send(s, name, strlen(name), 0);

	memset(buffer, '\0', sizeof(buffer));
	recv(s, buffer, sizeof(buffer), 0);
	printf("%s\n\n", buffer);

	memset(buffer, '\0', sizeof(buffer));
	recv(s, buffer, sizeof(buffer), 0);
	printf("%s\n\n", buffer);

    int d = 0;
    int enemy_y = 0;
    int pre_enemy_y = 0;
    int my_y = 0;
    int my_hp = 0;
    int my_cannon = 0;
    int pre_my_hp = 1;
    int flag_is_attacked = 0;
    int flag_is_cannon_existed = 0;
    int move_to_y = 100;
    int is_back = 1;
    int cannon_y_my_on_line = 0;
    int cannon_count = 0;
    int distance = 0;
    int flag_is_once = 0;
    goToY(s, move_to_y);

    while(1){
        my_y = getMyY(s);
        my_hp = getMyHp(s);
        my_cannon = getMyCannon(s);
        if (my_cannon > 2) {
            enemy_y = searchEnemy(s);
            if (enemy_y != -1) {
                if (flag_is_once) {
                    int range = (enemy_y - pre_enemy_y) > 0 ? (enemy_y - pre_enemy_y): (pre_enemy_y - enemy_y);
                    attackToY(s, enemy_y+range);
                    Sleep(900);
                    attackToY(s, enemy_y+100+range);
                    Sleep(1000);
                    attackToY(s, enemy_y-100+range);
                    flag_is_once = 0;
                }
                flag_is_once = 1;
            } else {
                flag_is_once = 0;
            }
            pre_enemy_y = enemy_y;
        } else {
            flag_is_once = 0;
        }
        if (my_hp < pre_my_hp) {
            flag_is_attacked = 1;
        } else {
            flag_is_attacked = 0;
        }
        cannon_y_my_on_line = getCannonOnMyLine(s, d);
        flag_is_cannon_existed =  (cannon_y_my_on_line == -100) ? 0 : 1;
        if (flag_is_cannon_existed && judgeStop(cannon_y_my_on_line, my_y, flag_is_attacked, d)) {
            printf("----------stay------------\n");
            goToY(s, my_y);
        } else {
            if (my_y < 120) {
                d = 1000;
                move_to_y = 900;
            } else if (my_y > 870) {
                d = 0;
                move_to_y = 100;
            }
            goToY(s, move_to_y);
        }
        pre_my_hp = my_hp;
    }

	closesocket(s);
	WSACleanup();

	return 0;

}