#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
/*
*������־
* 1.������Ŀ
* 2.�����ز�
* 3.ʵ���ʼ����Ϸ����
* 4.ʵ����Ϸ�������Ĺ�����
* 5.ʵ��ֲ��Ĺ��ܿ���
*/
#include<stdio.h>
#include<graphics.h>//easyxͼ�ο��ͷ�ļ�����Ҫ����
#include"tools.h"
#include<time.h>
#include"vector2.h"
#include<mmsystem.h>
#pragma comment(lib, "winmm.lib")
#define WIN_WIDTH 900
#define WIN_HEIGHT 600
//ö��ֲ��õ�ֲ������
enum { WAN_DOU, XIANG_RI_KUI, ZHI_WU_COUNT };
IMAGE imgBg;//��ʾ����ͼƬ
IMAGE imgBar;//������
IMAGE imgCards[ZHI_WU_COUNT];
IMAGE* imgZhiWu[ZHI_WU_COUNT][20];//ָ��

int curX, curY; //��ǰ�ƶ�����λ��
int curZhiWu = 0;// 0 :û��ֲ�� 1: ��һ��ֲ��

struct zhiwu {
	int type;// 0��û��ֲ�� 1�� ��һ��ֲ��
	int frameIndex;//����֡�ġ����
	int timer;
	int x, y;
	bool catched;//�Ƿ񱻽�ʬ����
	int deadTimer;
};
struct zhiwu map[3][9];

enum { SUNSHINE_DOWN, SUNSHINE_GROUND, SUNSHINE_COLLECT, SUNSHINE_PRODUCT };

struct sunshineBall {
	int x, y;//��������Ʈ������е�����λ�ã�x���䣩
	int frameIndex;//��ǰ��ʾͼƬ֡�����
	int destY; //Ʈ�������Ŀ��λ�õ�y����
	int used; //������������Ƿ���ʹ��
	int timer;
	//������������ص�
	float t;
	vector2 p1, p2, p3, p4;
	vector2 pCur;//��ǰλ��
	float speed;//�������ٶ�
	int status;
};

//�صĸ���
struct sunshineBall balls[10];
IMAGE imgSunshineBall[29];
int sunshine;

struct zm {
	int x, y;
	int speed;
	int row;
	int blood;
	bool dead;
	bool eating;//���ڳ�ֲ��
	bool used;
	int frameIndex;
};
struct zm zms[10];
IMAGE imgZM[22];
IMAGE imgZMDead[20];
IMAGE imgZMEat[21];

struct bullet {
	int x, y;
	int row;
	int used;
	int speed;
	bool blast;//�Ƿ�ը
	int frameIndex;//֡���

};
struct bullet bullets[30];
IMAGE imgBulletNormal;
IMAGE imgBulletBlast[4];


bool fileExist(const char* name) {
	FILE* fp = fopen(name, "r");
	if (fp == NULL) {
		return false;
	}
	else {
		fclose(fp);
		return true;
	}
}
void gameInit() {
	//�����ļ�����ͼƬ
	//���ַ����޸�Ϊ�����ֽڡ��ַ���
	loadimage(&imgBg, "res/bg.jpg");
	loadimage(&imgBar, "res/bar2.png");

	memset(imgZhiWu, 0, sizeof(imgZhiWu));
	memset(map, 0, sizeof(map));

	//��ʼ��ֲ�￨��
	char name[64];
	for (int i = 0; i < ZHI_WU_COUNT; i++) {
		//����ֲ�￨�Ƶ��ļ���
		sprintf_s(name, sizeof(name), "res/cards/card_%d.png", i + 1);
		loadimage(&imgCards[i], name);
		for (int j = 0; j < 20; j++) {
			sprintf_s(name, sizeof(name), "res/zhiwu/%d/%d.png", i, j + 1);
			//�ж��ļ��Ƿ����
			if (fileExist(name)) {
				imgZhiWu[i][j] = new IMAGE;
				loadimage(imgZhiWu[i][j], name);
			}
			else {
				break;
			}
		}
	}
	curZhiWu = 0;
	sunshine = 50;
	memset(balls, 0, sizeof(balls));
	for (int i = 0; i < 29; i++) {
		sprintf_s(name, sizeof(name), "res/sunshine/%d.png", i + 1);
		loadimage(&imgSunshineBall[i], name);
	}
	//�����������
	srand(time(NULL));

	//������Ϸ��ͼ�δ���
	initgraph(WIN_WIDTH, WIN_HEIGHT, 1);

	//��������
	LOGFONT f;
	gettextstyle(&f);
	f.lfHeight = 35;
	f.lfWeight = 20;
	strcpy(f.lfFaceName, "Segoe UI Black");
	f.lfQuality = ANTIALIASED_QUALITY;//�����
	settextstyle(&f);
	setbkmode(TRANSPARENT);//����ģʽ
	setcolor(BLACK);

	//��ʼ����ʬ

	memset(zms, 0, sizeof(zms));
	for (int i = 0; i < 22; i++) {
		sprintf_s(name, sizeof(name), "res/zm/%d.png", i + 1);
		loadimage(&imgZM[i], name);
	}

	//��ʼ����ʬ����
	for (int i = 0; i < 20; i++) {
		sprintf_s(name, sizeof(name), "res/zm_dead/%d.png", i + 1);
		loadimage(&imgZMDead[i], name);
	}
	//��ʼ����ʬ��ֲ��
	for (int i = 0; i < 21; i++) {
		sprintf_s(name, "res/zm_eat/%d.png", i + 1);
		loadimage(&imgZMEat[i], name);
	}

	// �����㶹�ӵ�ͼƬ
	loadimage(&imgBulletNormal, "res/bullets/bullet_normal.png");
	memset(bullets, 0, sizeof(bullets));

	//��ʼ���㶹�ӵ���֡ͼƬ
	loadimage(&imgBulletBlast[3], "res/bullets/bullet_blast.png");
	for (int i = 0; i < 3; i++) {
		float k = (i + 1) * 0.2;
		loadimage(&imgBulletBlast[i], "res/bullets/bullet_blast.png",
			imgBulletBlast[3].getwidth() * k,
			imgBulletBlast[3].getheight() * k, true);
	}
}



void drawSunshine() {
	int ballMax = sizeof(balls) / sizeof(balls[0]);
	for (int i = 0; i < ballMax; i++) {
		if (balls[i].used) {
			IMAGE* img = &imgSunshineBall[balls[i].frameIndex];
			putimagePNG(balls[i].pCur.x, balls[i].pCur.y, img);
		}
	}
	char scoreText[8];
	sprintf_s(scoreText, sizeof(scoreText), "%d", sunshine);
	outtextxy(270, 50, scoreText);
}

void drawZM() {
	int zmCount = sizeof(zms) / sizeof(zms[0]);
	for (int i = 0; i < zmCount; i++) {
		if (zms[i].used) {
			IMAGE* img = NULL;
			if (zms[i].dead) img = imgZMDead;
			else if (zms[i].eating) img = imgZMEat;
			else img = imgZM;
			img += zms[i].frameIndex;
			putimagePNG(
				zms[i].x,
				zms[i].y - img->getheight(),
				img);
		}
	}
}

void drawCards() {
	for (int i = 0; i < ZHI_WU_COUNT; i++) {
		int x = 316 + i * 65;
		int y = 5;
		putimage(x, y, &imgCards[i]);
	}
}

void drawZhiWu() {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 9; j++) {
			if (map[i][j].type > 0) {
				/*int x = 250 + j * 83;
				int y = 170 + i * 107;*/
				int zhiWuType = map[i][j].type - 1;
				int index = map[i][j].frameIndex;
				putimagePNG(map[i][j].x, map[i][j].y, imgZhiWu[zhiWuType][index]);
			}
		}
	}
	// ��Ⱦ�϶������е�ֲ��
	if (curZhiWu) {
		IMAGE* img = imgZhiWu[curZhiWu - 1][0];
		putimagePNG(curX - img->getwidth() / 2, curY - img->getheight() / 2, imgZhiWu[curZhiWu - 1][0]);
	}

}

void drawBullets() {
	int bulletMax = sizeof(bullets) / sizeof(bullets[0]);
	for (int i = 0; i < bulletMax; i++) {
		if (bullets[i].used) {
			if (bullets[i].blast) {
				IMAGE* img = &imgBulletBlast[bullets[i].frameIndex];
				putimagePNG(bullets[i].x, bullets[i].y, img);
			}
			else {
				putimagePNG(bullets[i].x, bullets[i].y, &imgBulletNormal);
			}
		}

	}

}
//ȷ��λ��
void updateWindow() {
	BeginBatchDraw();//��ʼ����
	putimage(0, 0, &imgBg);
	putimagePNG(250, 0, &imgBar);

	drawCards();
	drawZhiWu();
	drawSunshine();
	drawBullets();
	drawZM();

	EndBatchDraw();//����˫����
}

void collectSunshine(ExMessage* msg) {
	int count = sizeof(balls) / sizeof(balls[0]);
	int w = imgSunshineBall[0].getwidth();
	int h = imgSunshineBall[0].getheight();
	for (int i = 0; i < count; i++) {
		if (balls[i].used) {
			int x = balls[i].pCur.x;
			int y = balls[i].pCur.y;

			if (msg->x > x && msg->x < x + w &&
				msg->y > y && msg->y < y + h) {
				balls[i].status = SUNSHINE_COLLECT;
				PlaySound("res/sunshine.wav", NULL, SND_FILENAME | SND_ASYNC);
				balls[i].p1 = balls[i].pCur;
				balls[i].p4 = vector2(262, 0);
				balls[i].t = 0;
				float distance = dis(balls[i].p1 - balls[i].p4);
				float off = 8;
				balls[i].speed = 1.0 / (distance / off);
				break;
			}
		}
	}
}
void userClick() {
	ExMessage msg;
	static int status = 0;
	//�ж���û����Ϣ
	if (peekmessage(&msg)) {
		if (msg.message == WM_LBUTTONDOWN) {
			if (msg.x > 316 && msg.x < 316 + 65 * ZHI_WU_COUNT && msg.y < 87) {
				int index = (msg.x - 316) / 65;
				status = 1;
				curZhiWu = index + 1;
			}
			else {
				collectSunshine(&msg);
			}
		}
		else if (msg.message == WM_MOUSEMOVE && status == 1) {
			curX = msg.x;
			curY = msg.y;
		}
		else if (msg.message == WM_LBUTTONUP) {
			if (msg.x > 250 && msg.y > 177 && msg.y < 489) {
				int row = (msg.y - 170) / 107;
				int col = (msg.x - 250) / 82;
				printf("%d %d\n", row, col);
				//��ֲֲ��..����������
				if (map[row][col].type == 0) {
					map[row][col].type = curZhiWu;
					map[row][col].frameIndex = 0;

					map[row][col].x = 250 + col * 80;
					map[row][col].y = 177 + row * 103 + 14;
				}

			}
			curZhiWu = 0;
			status = 0;
		}
	}
}

void createSunshine() {
	static int count = 0;
	static int fre = 500;
	count++;
	if (count > fre) {
		fre = 200 + rand() % 200;
		count = 0;
		//�������������
		int ballMax = sizeof(balls) / sizeof(balls[0]);
		int i;
		for (i = 0; i < ballMax && balls[i].used; i++);
		if (i >= ballMax)return;
		balls[i].used = true;
		balls[i].frameIndex = 0;
		balls[i].timer = 0;
		balls[i].status = SUNSHINE_DOWN;

		balls[i].t = 0;
		balls[i].p1 = vector2(260 + rand() % (900 - 260), 60);
		balls[i].p4 = vector2(balls[i].p1.x, 200 + (rand() % 4) * 90);
		int off = 2;
		float diatance = balls[i].p4.y - balls[i].p1.y;
		balls[i].speed = 1.0 / (diatance / off);
	}
	//���տ���������
	int ballMax = sizeof(balls) / sizeof(balls[0]);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 9; j++) {
			if (map[i][j].type == XIANG_RI_KUI + 1) {
				map[i][j].timer++;
				if (map[i][j].timer > 300) {
					map[i][j].timer = 0;
					int k;
					for (k = 0; k < ballMax && balls[k].used; k++);
					if (k >= ballMax)return;
					balls[k].used = true;
					balls[k].p1 = vector2(map[i][j].x, map[i][j].y);
					int w = (100 + rand() % 50 * (rand() % 2 ? 1 : -1));
					balls[k].p4 = vector2(map[i][j].x + w,
						map[i][j].y + imgZhiWu[XIANG_RI_KUI][0]->getheight() -
						imgSunshineBall[0].getheight());
					balls[k].p2 = vector2(balls[k].p1.x + w * 0.33, balls[k].p1.y - 50);
					balls[k].p3 = vector2(balls[k].p1.x + w * 0.7, balls[k].p1.y - 100);
					balls[k].status = SUNSHINE_PRODUCT;
					balls[k].speed = 0.08;
					balls[k].t = 0;
				}
			}
		}
	}

}

void updateSunshine() {
	int ballMax = sizeof(balls) / sizeof(balls[0]);
	for (int i = 0; i < ballMax; i++) {
		if (balls[i].used) {
			balls[i].frameIndex = (balls[i].frameIndex + 1) % 29;
			if (balls[i].status == SUNSHINE_DOWN) {
				struct sunshineBall* sun = &balls[i];
				sun->t += sun->speed;
				sun->pCur = sun->p1 + sun->t * (sun->p4 - sun->p1);
				if (sun->t >= 1) {
					sun->status = SUNSHINE_GROUND;
					sun->timer = 0;
				}
			}
			else if (balls[i].status == SUNSHINE_GROUND) {
				balls[i].timer++;
				if (balls[i].timer > 100) {
					balls[i].used = false;
					balls[i].timer = 0;
				}
			}
			else if (balls[i].status == SUNSHINE_COLLECT) {
				struct sunshineBall* sun = &balls[i];
				sun->t += sun->speed;
				sun->pCur = sun->p1 + sun->t * (sun->p4 - sun->p1);
				if (sun->t > 1) {
					sun->used = false;
					sunshine += 25;
				}
			}
			else if (balls[i].status == SUNSHINE_PRODUCT) {
				struct sunshineBall* sun = &balls[i];
				sun->t += sun->speed;
				sun->pCur = calcBezierPoint(sun->t, sun->p1, sun->p2, sun->p3, sun->p4);
				if (sun->t > 1) {
					sun->status = SUNSHINE_GROUND;
					sun->timer = 0;
				}
			}

		}
	}
}

void createZM() {
	int zmMax = sizeof(zms) / sizeof(zms[0]);

	static int zmFre = 200;
	static int count = 0;
	count++;
	if (count > zmFre) {
		count = 0;
		zmFre = rand() % 200 + 300;
		int i;

		for (i = 0; i < zmMax && zms[i].used; i++);
		if (i > zmMax) return;
		memset(&zms[i], 0, sizeof(zms[i]));
		zms[i].used = true;
		zms[i].x = WIN_WIDTH;
		zms[i].row = rand() % 3;
		zms[i].y = 177 + (1 + zms[i].row) * 100;
		zms[i].speed = 1;
		zms[i].blood = 100;
		zms[i].dead = false;

	}

}

void updateZM() {
	int zmMax = sizeof(zms) / sizeof(zms[0]);
	static int count = 0;
	count++;
	if (count > 5) {
		count = 0;
		//���½�ʬ��λ��
		for (int i = 0; i < zmMax; i++) {
			if (zms[i].used) {
				zms[i].x -= zms[i].speed;
				if (zms[i].x < 170) {
					printf("GAME OVER\n");
					MessageBox(NULL, "over", "over", 0);
					exit(0);
				}
			}
		}
	}
	static int count2 = 0;
	count2++;
	if (count2 > 5) {
		count2 = 0;
		for (int i = 0; i < zmMax; i++) {
			if (zms[i].used) {
				if (zms[i].dead) {
					zms[i].frameIndex++;
					if (zms[i].frameIndex >= 20) {
						zms[i].used = false;
					}
				}
				else if (zms[i].eating) {
					zms[i].frameIndex = (zms[i].frameIndex + 1) % 21;
				}
				else {
					zms[i].frameIndex = (zms[i].frameIndex + 1) % 22;
				}

			}
		}
	}
}

void shoot() {
	static int count[3] = { 0 };//�ֱ�Ϊÿһ������һ����̬�洢��
	int lines[3] = { 0 };
	int zmCount = sizeof(zms) / sizeof(zms[0]);
	int bulletMax = sizeof(bullets) / sizeof(bullets[0]);
	int dangerX = WIN_WIDTH;
	for (int i = 0; i < zmCount; i++) {
		if (zms[i].used && zms[i].x < dangerX) {
			lines[zms[i].row] = 1;

		}
	}
	for (int i = 0; i < 3; i++) {
		count[i]++;
		for (int j = 0; j < 9; j++) {
			if (map[i][j].type == WAN_DOU + 1 && lines[i]) {
				if (count[i] > 70) {
					count[i] = 0;
					int k;
					for (k = 0; k < bulletMax && bullets[k].used; k++);
					if (k > bulletMax)return;
					if (k < bulletMax) {
						bullets[k].used = true;
						bullets[k].frameIndex = 0;
						bullets[k].blast = false;
						bullets[k].row = i;
						bullets[k].speed = 16;
						int zwX = 250 + j * 83;
						int zwY = 177 + i * 103 + 14;
						bullets[k].x = zwX + imgZhiWu[map[i][j].type - 1][0]->getwidth() - 10;
						bullets[k].y = zwY + 7;
					}
				}
			}
		}
	}
}
void updateBullets() {
	static int count = 0;
	if (++count < 3)return;
	count = 0;
	int countMax = sizeof(bullets) / sizeof(bullets[0]);
	for (int i = 0; i < countMax; i++) {
		if (bullets[i].used) {
			bullets[i].x += bullets[i].speed;
			if (bullets[i].x > WIN_WIDTH) {
				bullets[i].used = false;
			}
			if (bullets[i].blast) {
				bullets[i].frameIndex++;
				if (bullets[i].frameIndex >= 4) {
					bullets[i].used = false;
				}
			}
		}
	}
}

void checkBullet2ZM() {
	int bCount = sizeof(bullets) / sizeof(bullets[0]);
	int zCount = sizeof(zms) / sizeof(zms[0]);
	for (int i = 0; i < bCount; i++) {
		if (bullets[i].used == false || bullets[i].blast)continue;
		for (int k = 0; k < zCount; k++) {
			//if (zms[i].used == false)continue;
			if (zms[k].used == false)continue;
			int x1 = zms[k].x + 80;
			int x2 = zms[k].x + 100;
			int x = bullets[i].x;
			if (zms[k].dead == false &&
				bullets[i].row == zms[k].row && x > x1 && x < x2) {
				zms[k].blood -= 20;
				bullets[i].blast = true;
				bullets[i].speed = 0;

				if (zms[k].blood <= 0) {
					zms[k].dead = true;
					zms[k].speed = 0;
					zms[k].frameIndex = 0;
				}
				break;
			}
		}
	}
}

void checkZM2ZhiWu() {
	int zCount = sizeof(zms) / sizeof(zms[0]);
	for (int i = 0; i < zCount; i++) {
		if (zms[i].dead)continue;
		int row = zms[i].row;
		for (int k = 0; k < 9; k++) {
			if (map[row][k].type == 0) continue;
			int zhiWuX = 256 + k * 80;

			//     x1   x2
			//     [     ]
			//       [     ]
			//       x3
			int x1 = zhiWuX + 10;
			int x2 = zhiWuX + 60;
			int x3 = zms[i].x + 80;
			if (x3 > x1 && x3 < x2) {
				if (map[row][k].catched) {
					map[row][k].deadTimer++;
					if (map[row][k].deadTimer > 150) {
						map[row][k].deadTimer = 0;
						map[row][k].type = 0;
						zms[i].eating = false;
						zms[i].frameIndex = 0;
						zms[i].speed = 1;
					}
				}
				else {
					map[row][k].catched = true;
					map[row][k].deadTimer = 0;
					zms[i].eating = true;
					zms[i].speed = 0;
					zms[i].frameIndex = 0;
				}


			}
		}
	}
}
void collisionCheck() {
	checkBullet2ZM();//�ӵ��Խ�ʬ��ײ
	checkZM2ZhiWu();//��ʬ��ֲ����ײ
}

void updateZhiWu() {
	static int count = 0;
	if (++count < 3)return;
	count = 0;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 9; j++) {
			if (map[i][j].type > 0) {
				map[i][j].frameIndex++;
				int zhiWuType = map[i][j].type - 1;
				int index = map[i][j].frameIndex;
				if (imgZhiWu[zhiWuType][index] == NULL) {
					map[i][j].frameIndex = 0;
				}
			}
		}
	}
}

void updateGame() {

	updateZhiWu();//����ֲ��

	createSunshine();//��������
	updateSunshine();//��Ⱦ����

	createZM();//������ʬ
	updateZM();//���½�ʬ

	shoot();//�����ӵ�
	updateBullets();//�����ӵ�

	collisionCheck();//ʵ����ײ���
}





//ui����,�˵����
void startUI() {
	IMAGE imgBg, imgMenu1, imgMenu2;
	loadimage(&imgBg, "res/MainMenu.png");
	loadimage(&imgMenu1, "res/menu1.png");
	loadimage(&imgMenu2, "res/menu2.png");
	int flag = 0;
	while (1) {
		BeginBatchDraw();
		putimage(0, 0, &imgBg);
		putimagePNG(475, 75, flag ? &imgMenu2 : &imgMenu1);

		ExMessage msg;
		if (peekmessage(&msg)) {
			if (msg.message == WM_LBUTTONDOWN &&
				msg.x > 487 && msg.x < 484 + 300 &&
				msg.y > 82 && msg.y < 82 + 170) {
				flag = 1;
			}
			else if (msg.message == WM_LBUTTONUP && flag == 1) {
				Sleep(250);
				return;
			}
		}
		EndBatchDraw();
	}
}

int main(void) {
	gameInit();
	startUI();
	int timer = 0;
	bool flag = true;
	while (1) {
		userClick();
		//getDaly ��tool��Ĺ���
		timer += getDelay();
		if (timer > 15) {
			flag = true;
			timer = 0;
		}
		if (flag) {
			flag = false;
			updateWindow();
			updateGame();
		}
	}
	system("pause");
	return 0;
}