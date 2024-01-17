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

void attackByY(SOCKET s, int y) {
    char cannon[20];
    char buffer[1024];
    memset(cannon, '\0', sizeof(cannon));
    sprintf(cannon, "cannon:%d\n", y);
    send(s, cannon, strlen(cannon), 0);
    memset(buffer, '\0', sizeof(buffer));
	recv(s, buffer, sizeof(buffer), 0);
	printf("%s", buffer);
}

void goToY(SOCKET s, int y) {
    char move[20];
    char buffer[1024];
    memset(move, '\0', sizeof(move));
    sprintf(move, "move:%d\n", y);
    send(s, move, strlen(move), 0);
    memset(buffer, '\0', sizeof(buffer));
	recv(s, buffer, sizeof(buffer), 0);
	printf("%s", buffer);
}

void search(SOCKET s, int y, Position *pos) {
    char search[20];
    char buffer[1024];
    memset(search, '\0', sizeof(search));
    memset(buffer, '\0', sizeof(buffer));
    sprintf(search, "search:%d\n", y);
    send(s, search, strlen(search), 0);
    recv(s, buffer, sizeof(buffer), 0);
    printf("%s", buffer);
    char *p = strtok(buffer, ":");
    p = strtok(NULL, ":");
    printf("strcmp %d\n", strcmp(p, "null"));
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
    printf("%d: x: %d, y: %d, name: %s\n", y, pos->x, pos->y, pos->name);
}

int bestHeight(SOCKET s) {
    int i = 0;
    Position pos;
    Position* p = &pos;
    // int bestList[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int bestList[11] = {10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10};
    for (i = 0; i < 11; i++) {
        search(s, i * 100, p);
        if (strcmp(p -> name, "cannon") == 0) {
            int x = p -> x;
            x = x / 10;
            x = 100 - x;
            int y = p -> y;
            y = y / 100;
            bestList[y] += x;
            bestList[y-1] += x/2;
            bestList[y+1] += x/2;
        } else if (strcmp(p -> name, "enemy") == 0) {
            attackByY(s, p -> y);
            // Sleep(1000);
        }
    }
    int min = 100000;
    int minIndex = 0;
    for (i = 1; i < 10; i++) {
        if (bestList[i] < min) {
            min = bestList[i];
            minIndex = i;
        }
    }
    for (i = 1; i < 11; i++) {
        printf("%d: %d ", i, bestList[i]);
    }
    printf("\n");
    return minIndex * 100;
}

void getMyY(SOCKET s, Position *pos) {
    char my[20];
    char buffer[1024];
    memset(my, '\0', sizeof(my));
    memset(buffer, '\0', sizeof(buffer));
    sprintf(my, "state:hight\n");
    send(s, my, strlen(my), 0);
    recv(s, buffer, sizeof(buffer), 0);
    printf("%s", buffer);
    char *p = strtok(buffer, ":");
    p = strtok(NULL, ":");
    pos->x = 0;
    pos->y = atoi(p);
    pos->name = "my";
    printf("my: x: %d, y: %d, name: %s\n", pos->x, pos->y, pos->name);
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
    char name[] = "name:SAMPLE\n";
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
    int flag_100 = 0;

    while(1){
        // bestHeight(s);
        Position pos;
        Position* p = &pos;
        getMyY(s, p);
        int y = p -> y;
        printf("%d\n", y);
        Position cannonPos;
        Position* cp = &cannonPos;
        search(s, 0, cp);
        if (y < 150) {
            goToY(s, 900);
            flag_100 = 1000;
        } else if (y > 850) {
            goToY(s, 100);
            flag_100 = 0;
        }
    }

	closesocket(s);
	WSACleanup();

	return 0;

}